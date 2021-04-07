/*
 *
 *  This file is part of the Virtual Leaf.
 *
 *  VirtualLeaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VirtualLeaf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the Virtual Leaf.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2010 Roeland Merks.
 *
 */

#include <QDebug>

#include <string>
#include "cell.h"
#include "node.h"
#include "mesh.h"
#include "tiny.h"
#include "nodeset.h"
#include "cellitem.h"
#include "nodeitem.h"
#include "qcanvasarrow.h"
#include "parameter.h"


static const std::string _module_id("$Id$");

extern Parameter par;

double Cell::factor=1.;
double Cell::offset[3]={0,0,0};

Cell::Cell(void) : CellBase()
{
  m=0;
}

Cell::Cell(double x, double y, double z) : CellBase(x,y,z)
{
  m=0;
}

Cell::Cell(const Cell &src) :  CellBase(src)
{
  m=src.m;
}

bool Cell::Cmp(Cell *c) const { return this->Index() < c->Index(); }
bool Cell::Eq(Cell *c) const { return this->Index() == c->Index(); }

Cell Cell::operator=(const Cell &src) 
{
  CellBase::operator=(src);
  m=src.m;
  return *this;
}
//Cell(void) : CellBase() {}

void Cell::DivideOverAxis(Vector axis) 
{
  // Build a wall
  // ->  find the position of the wall

  // better look for intersection with a simple line intersection algorithm as below?
  // this leads to some exceptions: e.g. dividing a horizontal rectangle.
  // leaving it like this for the time being

  if (dead) return;

  Vector centroid=Centroid();
  double prev_cross_z=(axis * (centroid - *(nodes.back()) ) ).z ;

  ItList new_node_locations;

  for (list<Node *>::iterator i=nodes.begin(); i!=nodes.end(); i++) {

    // cross product to detect position of division
    Vector cross = axis * (centroid - *(*i));

    if (cross.z * prev_cross_z < 0 ) {

      new_node_locations.push_back(i);

    }		
    prev_cross_z=cross.z;
  }

  DivideWalls(new_node_locations, centroid, centroid+axis);
}

double Cell::MeanArea(void)
{
  return m->MeanArea();
}


void Cell::Apoptose(void)
{
  // First kill walls
#ifdef QDEBUG
  qDebug() << "This is cell " << Index() << endl;
  qDebug() << "Number of walls: " << walls.size() << endl;
#endif
  for (list<Wall *>::iterator w=walls.begin(); w!=walls.end(); w++) {
#ifdef QDEBUG
    qDebug() << "Before apoptosis, wall " << (*w)->Index() << " says: c1 = "
	     << (*w)->c1->Index() << ", c2 = " << (*w)->c2->Index() << endl;
#endif
  }
  for (list<Wall *>::iterator w=walls.begin(); w!=walls.end(); w++) {

#ifdef QDEBUG
    bool illegal_flag = false;
    if ((*w)->c1 == (*w)->c2 )
      illegal_flag=true;
#endif
    if ((*w)->c1 == this) {

      // invert wall?
      (*w)->c1 = (*w)->c2;      
      (*w)->c2 = m->boundary_polygon;

      Node *n1 = (*w)->n1;
      (*w)->n1 = (*w)->n2;
      (*w)->n2 = n1;

    } else {
      (*w)->c2 = m->boundary_polygon;
    }

#ifdef QDEBUG
    if (illegal_flag && (*w)->c1==(*w)->c2) {
      qDebug() << "I created an illegal wall." << endl;
    }
#endif

    if ( ((*w)->N1()->DeadP() || (*w)->N2()->DeadP()) ||
	 ((*w)->C1() == (*w)->C2() ) ){
      // kill wall
#ifdef QDEBUG
      qDebug() << "Killing wall." << endl;
#endif
      (*w)->Kill();

#ifdef QDEBUG
      if ((*w)) {
	qDebug() << "Wall " << (*w)->Index() << " says: c1 = " 
		 << (*w)->c1->Index() << ", c2 = " << (*w)->c2->Index() << endl;
      }
#endif
      (*w)=0;
    } else {
#ifdef QDEBUG
      qDebug() << "Not killing wall." << endl;
      qDebug() << "Wall " << (*w)->Index() << " says: c1 = " 
	       << (*w)->c1->Index() << ", c2 = " << (*w)->c2->Index() << endl;
#endif
    }
  }
  walls.remove(0);

  // Unregister me from my nodes, and delete the node if it no longer belongs to any cells
  list<Node *> superfluous_nodes;
  for (list<Node *>::iterator n=nodes.begin(); n!=nodes.end(); n++) {

    Node &no(*(*n));
    // locate myself in the node's owner list
    list<Neighbor>::iterator cellpos;
    bool cell_found=false;
    for (list<Neighbor>::iterator nb=no.owners.begin(); nb!=no.owners.end(); nb++) {
      if (nb->cell == this) {
	cellpos = nb;
	cell_found = true;
	break;
      }
    }

    if (!cell_found) {
      // I think this cannot happen; but if I am wrong, unpredictable things would happen. So throw an exception.
      throw ("Cell not found in CellBase::Apoptose()\n\rPlease correct the code to handle this situation.");
    }

    Neighbor tmp = *cellpos;
    no.owners.erase(cellpos);

    // if node has no owners left, or only has a connection to special cell -1 (outside world), mark it as dead.

    if (no.owners.size()==0 || (no.owners.size()==1 && no.owners.front().cell->BoundaryPolP()) ) {
      no.MarkDead();
    } else {
      // register node with outside world
      if (find_if( no.owners.begin(), no.owners.end(), 
		   bind2nd ( mem_fun_ref(&Neighbor::CellEquals), m->boundary_polygon->Index() ) ) == no.owners.end() ) {

	tmp.cell = m->boundary_polygon;
	no.owners.push_back(tmp);
      }
    }
  }

  // mark cell as dead
  MarkDead();
}

void Cell::ConstructConnections(void)
{
  // Tie up the nodes of this cell, assuming they are correctly ordered

  //cerr << "Constructing connections of cell " << index << endl;

  for (list<Node *>::iterator i=nodes.begin(); i!=nodes.end(); i++) {

    //cerr << "Connecting node " << *i << endl;
    //cerr << "Node " << *i << endl << " = " << *(*i) << endl;
    // 1. Tidy up existing connections (which are part of this cell)
    if ((*i)->owners.size()>0) {
      list<Neighbor>::iterator neighb_with_this_cell=
	// remove myself from the neighbor list of the node
	find_if((*i)->owners.begin(),
		(*i)->owners.end(),
		bind2nd(mem_fun_ref( &Neighbor::CellEquals ),this->Index() )  );
      if (neighb_with_this_cell!=(*i)->owners.end()) 
	(*i)->owners.erase(neighb_with_this_cell);
    }

    Node *previous;
    if (i!=nodes.begin()) {
      list<Node *>::iterator previous_iterator=i;
      previous_iterator--;
      previous=*previous_iterator;
    } else {
      previous=nodes.back();
    }

    Node *next;
    list<Node *>::iterator next_iterator=i;
    next_iterator++;
    if (next_iterator==nodes.end()) {
      next=nodes.front();
    } else {
      next=*next_iterator;
    }
    (*i)->owners.push_back( Neighbor( this, previous, next ) );
  }
}

bool Cell::DivideOverGivenLine(const Vector v1, const Vector v2, bool fix_cellwall, NodeSet *node_set )
{
  if (dead) return false;

  // check each edge for intersection with the line
  ItList new_node_locations;

#ifdef QDEBUG
  qDebug() << "Cell " << Index() << " is doing DivideOverGivenLine" << endl;
#endif
  for (list<Node *>::iterator i=nodes.begin(); i!=nodes.end(); i++) {

    Vector v3 = *(*i);
    list<Node *>::iterator nb=i;
    nb++;
    if (nb == nodes.end()) {
      nb = nodes.begin();
    }
    Vector v4 = *(*nb);

    double denominator = 
      (v4.y - v3.y)*(v2.x - v1.x) - (v4.x - v3.x)*(v2.y - v1.y);

    double ua = 
      ((v4.x - v3.x)*(v1.y - v3.y) - (v4.y - v3.y)*(v1.x -v3.x))/denominator;
    double ub = 
      ((v2.x - v1.x)*(v1.y-v3.y) - (v2.y- v1.y)*(v1.x - v3.x))/denominator;


    //cerr << "Edge " << *i << " to " << *nb << ": ua = " << ua << ", ub = " << ub << ":  ";
    // this construction with "TINY" should simulate open/closed interval <0,1]
    if ( ( TINY < ua && ua < 1.+TINY ) && ( TINY < ub && ub < 1.+TINY ) ) {
      // yes, intersection detected. Push the location to the list of iterators
      new_node_locations.push_back(nb);

    } 
  }

#ifdef QDEBUG
  if (new_node_locations.size()<2) { 
    qDebug() << "Line does not intersect with two edges of Cell " << Index() << endl;
    qDebug() << "new_node_locations.size() = " << new_node_locations.size() << endl;
    return false;
  }

  ItList::iterator i = new_node_locations.begin();
  list< Node *>::iterator j;
  qDebug() << "-------------------------------" << endl;
  qDebug() << "Location of new nodes: " << (**i)->Index() << " and ";

  ++i;
  j = *i; 
  if (j==nodes.begin()) j=nodes.end(); j--;

  qDebug() << (*j)->Index() << endl;
  qDebug() << "-------------------------------" << endl;

  if ( **new_node_locations.begin() == *j ) {
    qDebug() << "Rejecting proposed division (cutting off zero area)." << endl;
    return false;
  }
#endif

  DivideWalls(new_node_locations, v1, v2, fix_cellwall, node_set);

  return true;
}

// Core division procedure
void Cell::DivideWalls(ItList new_node_locations, const Vector from, const Vector to, bool fix_cellwall, NodeSet *node_set)
{

  if (dead) return;

  bool boundary_touched_flag=false;

  // Step 0: keep some data about the parent before dividing

  ParentInfo parent_info;
  parent_info.polarization = ReduceCellAndWalls<Vector>( PINdir );
  parent_info.polarization.Normalise();
  parent_info.PINmembrane = SumTransporters(1);
  parent_info.PINendosome = Chemical(1);

  //cerr << "Parent polarization before division: " << parent_info.polarization << endl;

  // Step 1: create a daughter cell
  Cell *daughter=m->AddCell(new Cell());

  // Step 2: Copy the basics of parent cell to daughter
  for (int i=0;i<NChem();i++) {
    daughter->chem[i]=chem[i];
  }

  daughter->cell_type = cell_type;

  for (int i=0;i<NChem();i++) {
    daughter->new_chem[i]=new_chem[i];
  }

  daughter->boundary=boundary;
  daughter->m=m;

  daughter->target_area=target_area/2.;

  target_area/=2;
  daughter->cellvec=cellvec;

  // Division currently only works for convex cells: i.e. if the division line
  // intersects the cells at two points only.
  if (new_node_locations.size()!=2) {

    // Note: if you would add the possibility of dividing non-convex
    // cells, remember to update the code below. There are some
    // fixed-size arrays over there!

    cerr << "Warning in Cell::Division: division of non-convex cells not fully implemented" << endl;

    // Reject the daughter cell and decrement the amount of cells
    // again. We can do this here because it is the last cell added.
    // Never, ever try to fully erase a cell elsewhere, because we
    // make heavy use of cell indices in this project; if you erase a
    // Cell somewhere in the middle of Mesh::Cells the indices will
    // get totally messed up...! (e.g. the indices used in Nodes::cells)

#ifdef QDEBUG
    qDebug() << "new_node_locations.size() = " << new_node_locations.size() <<endl;
    qDebug() << "daughter->index = " << daughter->index << endl;
    qDebug() << "cells.size() = " << m->cells.size() << endl;
#endif

    m->cells.pop_back();
    Cell::NCells()--;
    m->shuffled_cells.pop_back();
    return;
  }

  // We can be sure we only need two positions here because divisions
  // of non-convex cells are rejected above.
  Vector new_node[2];
  Node *new_node_ind[2];

  int new_node_flag[2];
  Edge div_edges[2];

  int nnc=0;

  Wall *div_wall[4];
  double orig_length[2];
  for (int i=0;i<4;i++) { div_wall[i]=0; orig_length[i/2] = 0.; }

  // construct new Nodes at the intersection points
  // unless they coincide with existing points
  for ( ItList::const_iterator i=new_node_locations.begin(); i!=new_node_locations.end(); i++) {

    // intersection between division axis
    // and line from this node to its predecessor

    // method used: http://astronomy.swin.edu.au/~pbourke/geometry/lineline2d/
    Vector v1 = from;
    Vector v2 = to;
    Vector v3 = *(**i);

    // get previous node
    list<Node *>::iterator nb=*i;
    if (nb == nodes.begin()) {
      nb = nodes.end();
    } 
    nb--;
    Vector v4=*( *nb ); 

    double denominator = 
      (v4.y - v3.y)*(v2.x - v1.x) - (v4.x - v3.x)*(v2.y - v1.y);

    double ua = 
      ((v4.x - v3.x)*(v1.y - v3.y) - (v4.y - v3.y)*(v1.x -v3.x))/denominator;

    double intersec_x = v1.x + ua*(v2.x-v1.x);
    double intersec_y = v1.y + ua*(v2.y-v1.y);

    // construct a new node at intersec
    // we construct a vector temporarily,
    // until we are sure we are going to keep the node...
    // Node count is damaged if we construct superfluous nodes
    Vector *n=new Vector(intersec_x,intersec_y,0);

    div_edges[nnc].first=*nb;
    div_edges[nnc].second=**i;

    // Insert this new Node if it is far enough (5% of element length)
    // from one of the two existing nodes, else use existing node
    //
    // old, fixed value was: par.collapse_node_threshold = 0.05
    double collapse_node_threshold = 0.05;
#ifdef FLEMING
    collapse_node_threshold = par.collapse_node_threshold;
#endif

    double elem_length = ( (*(**i)) - (*(*nb)) ).Norm();
    if ( ( *(**i) - *n ).Norm() < collapse_node_threshold  * elem_length ) {
      new_node_flag[nnc]=1;
      new_node[nnc] = *(**i);
      new_node_ind[nnc] = **i;
      //cerr << **i << endl ;
    } else 
      if ( (*(*nb) - *n).Norm() < collapse_node_threshold * elem_length ) {
	new_node_flag[nnc]=2;
	new_node[nnc] = *(*nb);
	new_node_ind[nnc] = *nb;
      } else {
	new_node_flag[nnc]=0;
	new_node[nnc] = *n;
      }

    nnc++;
    delete n;
  }


  for (int i=0;i<2;i++) {

    Cell *neighbor_cell=0; // we need this to split up the "Wall" objects.

    // for both divided edges: 
    //      insert its new node into all cells that own the divided edge
    // but only if it really is a new node:
    if (new_node_flag[i]!=0) {
      if (fix_cellwall) {
	(new_node_ind[i])->fixed = true;

	// all this we'll do later for the node set :-)
	/* (new_node_ind[i])->boundary = true;
	   (new_node_ind[i])->sam = true;
	   boundary = SAM;
	   daughter->boundary = SAM;
	   boundary_touched_flag = true;
	*/ 
      }

    } else {

      // (Construct a list of all owners:)
      // really construct the new node (if this is a new node)
      new_node_ind[i] = 
	m->AddNode(new Node (new_node[i]) );



      // if a new node is inserted into a fixed edge (i.e. in the petiole)
      // make the new node fixed as well
      (new_node_ind[i])->fixed = (div_edges[i].first)->fixed &&
	(div_edges[i].second)->fixed;

      // Insert Node into NodeSet if the div_edge is part of it.
      if (
	  (div_edges[i].first->node_set && div_edges[i].second->node_set) &&
	  (div_edges[i].first->node_set == div_edges[i].second->node_set))
	{
	  //cerr << "Inserting node into node set\n";
	  div_edges[i].first->node_set->AddNode( new_node_ind[i] );
	}

      // if the new wall should be fixed (i.e. immobile, or moving as
      // solid body), make it so, and make it part of the boundary. Using
      // this to make a nice initial condition by cutting off part of a
      // growing leaf.

      if (fix_cellwall) {
	(new_node_ind[i])->fixed = true;

	// All this we'll do later for the node set only
      }

      // if new node is inserted into the boundary
      // it will be part of the boundary, too

      new_node_ind[i]->UnsetBoundary();
      if ((div_edges[i].first->BoundaryP() && div_edges[i].second->BoundaryP()) && // Both edge nodes are boundary nodes AND
	  ((m->findNextBoundaryNode(div_edges[i].first))->Index() == div_edges[i].second->Index())){ // The boundary proceeds from first to second.

#ifdef QDEBUG
	qDebug() << "Index of the first node: " << div_edges[i].first->Index() << endl;
	qDebug() << "Index of the second node: " << div_edges[i].second->Index() << endl;
	qDebug() << "Boundary proceeds from: " <<  div_edges[i].first->Index() 
		 << "to: " << (m->findNextBoundaryNode(div_edges[i].first))->Index() << endl << endl;
#endif
	new_node_ind[i]->SetBoundary();

	// We will need to repair the boundary polygon later, since we will insert new nodes
	//cerr << "Boundary touched for Node " << new_node_ind[i]->Index() << "\n";
	boundary_touched_flag=true;

	// and insert it into the boundary_polygon
	// find the position of the first node in the boundary
	list<Node *>::iterator ins_pos = find
	  (m->boundary_polygon->nodes.begin(),
	   m->boundary_polygon->nodes.end(),
	   div_edges[i].first);
	// ... second node comes before or after it ...
	if (*(++ins_pos!=m->boundary_polygon->nodes.end()?
	      ins_pos:m->boundary_polygon->nodes.begin())!=div_edges[i].second) {

	  m->boundary_polygon->nodes.insert(((ins_pos--)!=m->boundary_polygon->nodes.begin()?ins_pos:(--m->boundary_polygon->nodes.end())), new_node_ind[i]);

	  // .. set the neighbors of the new node ...
	  // in this case e.second and e.first are inverted
	} else {
	  // insert before second node, so leave ins_pos as it is,
	  // that is: incremented
	  m->boundary_polygon->nodes.insert(ins_pos, new_node_ind[i]);	
	  // .. set the neighbors of the new node ...
	}
      }

      list<Neighbor> owners;

      // push all cells owning the two nodes of the divides edges
      // onto a list

      copy((div_edges[i].first)->owners.begin(),
	   (div_edges[i].first)->owners.end(),
	   back_inserter(owners));
      copy((div_edges[i].second)->owners.begin(),
	   (div_edges[i].second)->owners.end(),
	   back_inserter(owners));


      // find first non-self duplicate in the owners: 
      // cells owning the same two nodes
      // share an edge with me
      owners.sort( mem_fun_ref( &Neighbor::Cmp ) );


#ifdef QDEBUG  
      list<Neighbor> unique_owners;
      copy(owners.begin(), owners.end(), back_inserter(unique_owners));
      unique_owners.unique( mem_fun_ref( &Neighbor::Eq ) );
      qDebug() << "The dividing edge nodes: " << div_edges[i].first->Index() 
	       << " and " << div_edges[i].second->Index() << " are owned by cells: ";

      // spit out each owners' cell index
      foreach(Neighbor neighbor, unique_owners){
	qDebug() << neighbor.cell->Index() << "  ";
      }
      qDebug() << endl;
#endif

      // Search through the sorted list of edge node owners looking for duplicate pairs. Each pair represents an actual edge owner.
      list<Neighbor> edge_owners;
      list<Neighbor>::iterator it;
      for (it=owners.begin(); it!=owners.end(); it++) {
	it = adjacent_find(it, owners.end(), neighbor_cell_eq);
	if (it == owners.end()) break; // bail if reach the end of the list
#ifdef QDEBUG
	qDebug() << "Considering: " << it->cell->Index() << " as a possible edge owner." << endl;
#endif
	if (it->cell->Index() != this->Index()) {
#ifdef QDEBUG
	  qDebug() << "Adding: " << it->cell->Index() << " to the list of edge owners." << endl;
#endif
	  edge_owners.push_back(*it);
	}
      } 

      if (edge_owners.size() > 1){
	// Remove the boundary polygon - if its there
	list<Neighbor>::iterator it;
	if ((it = find_if (edge_owners.begin(), edge_owners.end(), bind2nd(mem_fun_ref(&Neighbor::CellEquals), -1)))
	    != edge_owners.end()) {
#ifdef QDEBUG
      qDebug() << "deleting: " << it->cell->Index() << " from the list of edge owners." << endl;
#endif
	  edge_owners.erase(it);
	}
      }

#ifdef QDEBUG
      qDebug() << "The edge owners list has: " << edge_owners.size() << " elements" << endl;
#endif

      // Since the list should always contain exactly one element, pass it on as an iterator
      list<Neighbor>::iterator c = (edge_owners.size() != 0) ? edge_owners.begin() : edge_owners.end();

      // (can we have more than one neighboring cell here??)
      if (c!=owners.end()) { 
	neighbor_cell = c->cell;
	if (!c->cell->BoundaryPolP()) {

	  // find correct position in the cells node list
	  // to insert the new node
	  list<Node *>::iterator ins_pos = find
	    (neighbor_cell->nodes.begin(),
	     neighbor_cell->nodes.end(),
	     div_edges[i].first);

	  neighbor_cell->nodes.insert(ins_pos, new_node_ind[i]);
	  neighbor_cell->ConstructConnections();

	  // give walls to daughter later
	}
      } else {
	neighbor_cell = 0;
      }
    }

    // Split the Wall with the neighboring cell

    // if the neighbor cell has not yet been identified above, do it now
    if (neighbor_cell == 0) {

      list<Neighbor> owners;

      // push all cells owning the two nodes of the divides edges
      // onto a list
      copy((div_edges[i].first)->owners.begin(),
	   (div_edges[i].first)->owners.end(),
	   back_inserter(owners));
      copy((div_edges[i].second)->owners.begin(),
	   (div_edges[i].second)->owners.end(),
	   back_inserter(owners));


      // find first non-self duplicate in the owners: 
      // cells owning the same two nodes
      // share an edge with me
      owners.sort( mem_fun_ref( &Neighbor::Cmp ) );

      list<Neighbor>::iterator c;
      for (c=owners.begin(); c!=owners.end(); c++) {
	c=adjacent_find(c,owners.end(),neighbor_cell_eq);
	if (c->cell->Index() != this->Index() || c==owners.end()) break;
      }

      if (c!=owners.end())
	neighbor_cell = c->cell;
      else 
	neighbor_cell = 0;
    }


    if (neighbor_cell /* && !neighbor_cell->BoundaryPolP() */) {

      //cerr << "Cell "  << index << " says: neighboring cell is " << neighbor_cell->index << endl;

      /*************** 1. Find the correct wall element  ********************/

      list<Wall *>::iterator w, start_search;
      w = start_search = walls.begin();
      do {
	// Find wall between this cell and neighbor cell
	w = find_if( start_search, walls.end(), bind2nd (mem_fun( &Wall::is_wall_of_cell_p ), neighbor_cell ) );
	start_search = w; start_search++; // continue searching at next element
      } while ( w!=walls.end() && !(*w)->IntersectsWithDivisionPlaneP( from, to ) ); // go on until we find the right one.

      if (w == walls.end()) {
#ifdef QDEBUG
	qDebug() << "Whoops, wall element not found...!" << endl;
	qDebug() << "Cell ID: " << neighbor_cell->Index() << endl;
	qDebug() << "My cell ID: " << Index() << endl;
#endif
      } else {

	// 2. Split it up, if we should (sometimes, the new node coincides with an existing node so
	// we should not split up the Wall)

	if (new_node_ind[i]!=(*w)->n1 && new_node_ind[i]!=(*w)->n2) {

	  Wall *new_wall;

	  // keep the length of the original wall; we need it to equally divide the transporter concentrations
	  // over the two daughter walls
	  (*w)->SetLength(); // make sure we've got the current length
	  orig_length[i] = (*w)->Length();
	  //cerr << "Original length is " << orig_length[i] << endl;
	  if ((*w)->c1 == this ) {

	    //  cerr << "Cell " << (*w)->c1->Index() << " splits up wall " << *(*w) << ", into: " << endl;
	    new_wall = new Wall( (*w)->n1, new_node_ind[i], this, neighbor_cell);
	    (*w)->n1 = new_node_ind[i];

	    //  cerr << "wall " << *(*w) << ", and new wall " << *new_wall << endl;

	  } else {
	    new_wall = new Wall( (*w)->n1, new_node_ind[i], neighbor_cell, this);

	    (*w)->n1 = new_node_ind[i];
	  }


	  // 3. Give wall elements to appropriate cells
	  if (new_wall->n1 != new_wall->n2) {

	    if (par.copy_wall)
	      new_wall->CopyWallContents(**w);
	    else {
	      // If wall contents are not copied, decide randomly which wall will be the "parent"
	      // otherwise we will get biases (to the left), for example in the meristem growth model
	      if (RANDOM()<0.5) {
		new_wall->SwapWallContents(*w);
	      }
	    }
	    AddWall(new_wall);
	    // cerr << "Building new wall: this=" << Index() << ", neighbor_cell = " << neighbor_cell->Index() << endl;

	    neighbor_cell->AddWall( new_wall);
	    //cerr << "Existing wall: c1 = " << (*w)->c1->Index() << ", neighbor_cell = " << (*w)->c2->Index() << endl;

	    // Remember the addresses of the new walls
	    div_wall[2*i+0] = *w;
	    div_wall[2*i+1] = new_wall;

	    // we will correct the transporter concentrations later in this member function, after division
	    // First the new nodes should be inserted into the cells, before we can calculate wall lengths
	    // Remember that cell walls can be bent, so have a bigger length than the Euclidean distance n1->n2

	  } else {
	    delete new_wall;
	  }
	}
      }
    }
  }  // closing loop over the two divided edges (for (int i=0;i<2;i++) )

  // move half of the nodes to the daughter
  {
    //cerr << "Daughter: ";
    list<Node *>::iterator start, stop;

    start=new_node_locations.front();

    //cerr << "*new_node_locations.front() = " << *new_node_locations.front() << endl;
    if (new_node_flag[0]==1) {
      start++;
      if (start==nodes.end())
	start=nodes.begin();
    }  

    stop=new_node_locations.back();
    if (new_node_flag[1]==2) {
      if (stop==nodes.begin())
	stop=nodes.end();
      stop--;
    }
    list<Node *>::iterator i=start;
    while ( i!=stop) {

      // give the node to the daughter
      // (find references to parent cell from this node,
      // and remove them)
      list<Neighbor>::iterator neighb_with_this_cell=
	find_if((*i)->owners.begin(),
		(*i)->owners.end(),
		bind2nd(mem_fun_ref( &Neighbor::CellEquals ),this->Index() )  );
      if (neighb_with_this_cell==(*i)->owners.end()) {

#ifdef QDEBUG
	qDebug() << "not found" << endl;
#endif
	abort();
      }

      (*i)->owners.erase(neighb_with_this_cell);

      daughter->nodes.push_back( *i );


      i++;
      if (i==nodes.end())
	i=nodes.begin();
    };
  }

  // new node list of parent
  list<Node *> new_nodes_parent;

  // half of the nodes stay with the parent
  {
    list<Node *>::iterator start, stop;
    start=new_node_locations.back();
    if (new_node_flag[1]==1) {
      start++;
      if (start==nodes.end())
	start=nodes.begin();
    }
    stop=new_node_locations.front();
    if (new_node_flag[0]==2) {
      if (stop==nodes.begin())
	stop=nodes.end();
      stop--;
    }

    list<Node *>::iterator i=start;
    while (i!=stop) {
      new_nodes_parent.push_back( *i );

      i++;
      if (i==nodes.end()) 
	i = nodes.begin();
    };
  }

  // insert shared wall
  // insert shared nodes on surface of parent cell
  new_nodes_parent.push_back( new_node_ind[0] );
  daughter->nodes.push_back ( new_node_ind[1] );

  // optionally add the new node to the nodeset (passed by pointer)
  // (in this way we can move the NodeSet as a whole; useful for a fixed cutting line)
  if (node_set) {
    node_set->AddNode( new_node_ind[0] );
  }

#define MULTIPLE_NODES
#ifdef MULTIPLE_NODES
  // intermediate, extra nodes
  // Calculate distance between the two new nodes
  double dist=( new_node[1] - new_node[0] ).Norm();
  //bool fixed_wall = (new_node_ind[0])->fixed && (new_node_ind[1])->fixed;
  bool fixed_wall = false;

  // Estimate number of extra nodes in wall
  // factor 4 is to keep tension on the walls;
  // this is a hidden parameter and should be made explicit
  // later on.
  int n=(int)((dist/Node::target_length)/4+0.5);

  Vector nodevec = ( new_node[1]- new_node[0]).Normalised();

  double element_length = dist/(double)(n+1);

  // note that wall nodes need to run in inverse order in parent
  list<Node *>::iterator ins_pos = daughter->nodes.end();
  for (int i=1;i<=n;i++) {
    Node *node=
      m->AddNode( new Node( new_node[0] + i*element_length*nodevec ) );

    node->fixed=fixed_wall;

    if (!fix_cellwall)
      node->boundary = false;
    else {
      node->fixed = true;
    }

    ins_pos=daughter->nodes.insert(ins_pos, node );
    new_nodes_parent.push_back( node );

    // optionally add the new node to the nodeset (passed by pointer)
    // (in this way we can move the NodeSet as a whole; useful for a fixed cutting line)
    if (node_set) {
      node_set->AddNode( node );
    }

  }
#endif
  daughter->nodes.push_back( new_node_ind[0] );
  new_nodes_parent.push_back( new_node_ind[1] );

  // optionally add the new node to the nodeset (passed by pointer)
  // (in this way we can move the NodeSet as a whole; useful for a fixed cutting line)
  if (node_set) {
    node_set->AddNode( new_node_ind[1] );
  }

  // move the new nodes to the parent
  nodes.clear();
  copy( new_nodes_parent.begin(), 
	new_nodes_parent.end(), 
	back_inserter(nodes) );


  // Repair cell lists of Nodes, and node connectivities
  ConstructConnections();
  daughter->ConstructConnections();

  if (boundary_touched_flag) {
    m->boundary_polygon->ConstructConnections();
  } 

  // collecting neighbors of divided cell
  list<CellBase *> broken_neighbors;

  // this cell's old neighbors
  copy(neighbors.begin(), neighbors.end(), back_inserter(broken_neighbors) );

  // this cell
  broken_neighbors.push_back(this);

  // its daughter
  broken_neighbors.push_back(daughter);

  // Recalculate area of parent and daughter
  area = CalcArea();
  daughter->area = daughter->CalcArea();

  SetIntegrals();
  daughter->SetIntegrals();

  // Add a "Cell Wall" for diffusion algorithms
  Wall *wall = new Wall( new_node_ind[0], new_node_ind[1], this, daughter );

  AddWall( wall );

  daughter->AddWall( wall );

  //cerr << "Correct walls of cell " << Index() << " and daughter " << daughter->Index() << endl;

  // Move Walls to daughter cell
  list <Wall *> copy_walls = walls;
  for (list<Wall *>::iterator w = copy_walls.begin(); w!=copy_walls.end(); w++) {

    //cerr << "Doing wall, before:  " << **w << endl;

    //  checks the nodes of the wall and gives it away if appropriate
    (*w)->CorrectWall ( );

    //cerr << "and after: " << **w << endl;

  }

  // Correct tranporterconcentrations of divided walls
  for (int i=0;i<4;i++) {
    if (div_wall[i]) {
      div_wall[i]->SetLength();
      div_wall[i]->CorrectTransporters(orig_length[i/2]);
    }
  }

  //cerr << "Cell " << index << " has been dividing, and gave birth to Cell " << daughter->index << endl;

  // now reconstruct neighbor list for all "broken" neighbors

  for (list<CellBase *>::iterator i=broken_neighbors.begin(); i!=broken_neighbors.end(); i++) {
    ((Cell *)(*i))->ConstructNeighborList();
  }

  ConstructNeighborList();
  daughter->ConstructNeighborList();

  m->plugin->OnDivide(&parent_info, daughter, this);

  daughter->div_counter=(++div_counter);
}

// Move the whole cell
void Cell::Move(const Vector T) {

  for (list<Node *>::const_iterator i=nodes.begin(); i!=nodes.end(); i++) {
    *(*i)+=T;
  }
}

double Cell::Displace(double dx, double dy, double dh)
{

  // Displace whole cell, add resulting energy to dh,
  // and accept displacement if energetically favorable
  // 
  // Method is called if a "fixed" node is displaced

  // Warning: length constraint not yet  CORRECTLY implemented for this function

  // Attempt to move this cell in a random direction
  //  Vector movement(par.mc_cell_stepsize*(RANDOM()-0.5),par.mc_cell_stepsize*(RANDOM()-0.5),0);


  dh=0;

  Vector movement(dx,dy,0);

  vector< pair<Node *, Node *> > length_edges;
  vector<double> cellareas;
  cellareas.reserve(neighbors.size());

  // for the length constraint, collect all edges to this cell's nodes,
  // which are not part of the cell
  // the length of these edges will change

  double old_length=0.;
  for (list<Node *>::const_iterator i=nodes.begin(); i!=nodes.end(); i++) {
    for (list<Neighbor>::const_iterator n=(*i)->owners.begin(); n!=(*i)->owners.end(); n++) {
      if (n->getCell()!=this) {
	length_edges.push_back( pair <Node *,Node *> (*i, n->nb1) );
	length_edges.push_back( pair <Node *,Node *> (*i, n->nb2) );
	old_length += 
	  DSQR(Node::target_length-(*(*i)-*(n->nb1)).Norm())+
	  DSQR(Node::target_length-(*(*i)-*(n->nb2)).Norm());
      }
    }
  }

  // calculate area energy difference of neighboring cells
  // (this cells' shape remains unchanged)
  double old_area_energy=0., old_length_energy=0.;
  for (list<CellBase *>::const_iterator i=neighbors.begin(); i!=neighbors.end(); i++) {
    old_area_energy += DSQR((*i)->Area()-(*i)->TargetArea());
    old_length_energy += DSQR((*i)->Length()-(*i)->TargetLength());
  }

  Move(movement);

  double new_area_energy=0., new_length_energy=0.;
  for (list<CellBase *>::const_iterator i=neighbors.begin(); i!=neighbors.end(); i++) {
    cellareas.push_back((*i)->CalcArea());
    new_area_energy += DSQR(cellareas.back()-(*i)->TargetArea());
    new_length_energy += DSQR((*i)->CalcLength()-(*i)->TargetLength());
  }

  double new_length=0;
  for ( vector< pair< Node *, Node * > >::const_iterator e = length_edges.begin(); e != length_edges.end(); e++) {
    new_length +=  DSQR(Node::target_length - (*(e->first) - *(e->second)).Norm());
  }


  dh += (new_area_energy - old_area_energy) + (new_length_energy - old_length_energy) * lambda_celllength +
    par.lambda_length * (new_length - old_length);

  if (dh<0 || RANDOM()<exp(-dh/par.T)) {

    // update areas of cells
    //cerr << "neighbors: ";
    list<CellBase *>::const_iterator nb_it = neighbors.begin();
    for (vector<double>::const_iterator ar_it = cellareas.begin(); ar_it!=cellareas.end(); ( ar_it++, nb_it++) ) {
      ((Cell *)(*nb_it))->area = *ar_it;
      (*nb_it)->SetIntegrals(); 
    }

    //cerr << endl;

  } else {

    Move ( -1*movement);

  }

  return dh;
}


void Cell::Displace (void)
{
  Displace(par.mc_cell_stepsize*(RANDOM()-0.5),par.mc_cell_stepsize*(RANDOM()-0.5),0);
}

// Get energy level of whole cell (excluding length constraint?)
double Cell::Energy(void) const
{
  double energy = 0.;
  double length_contribution = 0.;

  for (list<Node *>::const_iterator i=nodes.begin(); i!=nodes.end(); i++) {
    for (list<Neighbor>::const_iterator n=(*i)->owners.begin(); n!=(*i)->owners.end(); n++) {
      if (n->getCell()==this) {
	length_contribution += 
	  DSQR(Node::target_length-(*(*i)-*(n->nb1)).Norm()) +
	  DSQR(Node::target_length-(*(*i)-*(n->nb2)).Norm());
      }
    }
  }

  // wall elasticity constraint
  energy += par.lambda_length * length_contribution;

  // area constraint
  energy += DSQR(CalcArea() - target_area);

  // cell length constraint
  energy += lambda_celllength * DSQR(Length() - target_length);

  return energy;
}





bool Cell::SelfIntersect(void)
{
  // The (obvious) O(N*N) algorithm

  // Compare each edge against each other edge

  // An O(N log(N)) algorithm by Shamos & Hoey (1976) supposedly exists;
  // it was mentioned on comp.graphics.algorithms

  // But I haven't been able to lay my hand on the paper.
  // Let's try whether we need it....

  // method used: http://astronomy.swin.edu.au/~pbourke/geometry/lineline2d/

  for (list<Node *>::const_iterator i=nodes.begin(); i!=nodes.end(); i++) {

    list<Node *>::const_iterator j=i; 
    ++j;
    for (; j!=nodes.end(); j++) 
      {
	
	Vector v1 = *(*i);
	list<Node *>::const_iterator nb=i;
	nb++;
	if (nb == nodes.end()) {
	  nb = nodes.begin();
	} 
	Vector v2 = *(*nb);
	Vector v3 = *(*j);
	nb=j;
	nb++;
	if (nb == nodes.end()) {
	  nb = nodes.begin();
	} 
	Vector v4=*( *nb ); 

	double denominator = 
	  (v4.y - v3.y)*(v2.x - v1.x) - (v4.x - v3.x)*(v2.y - v1.y);

	double ua = 
	  ((v4.x - v3.x)*(v1.y - v3.y) - (v4.y - v3.y)*(v1.x -v3.x))/denominator;
	double ub = 
	  ((v2.x - v1.x)*(v1.y-v3.y) - (v2.y- v1.y)*(v1.x - v3.x))/denominator;


	if ( ( TINY < ua && ua < 1.-TINY ) && ( TINY < ub && ub < 1.-TINY ) ) {
	  //cerr << "ua = " << ua << ", ub = " << ub << endl;
	  return true;
	}
      }
  }

  return false;
}


/*
bool Cell::MoveSelfIntersectsP(Node *moving_node_ind, Vector new_pos)
{

  // Check whether the polygon will self-intersect if moving_node_ind 
  // were displaced to new_pos

  // Compare the two new edges against each other edge

  // O(2*N)

  // method used for segment intersection:
  // http://astronomy.swin.edu.au/~pbourke/geometry/lineline2d/

  Vector neighbor_of_moving_node[2];

  //cerr << "list<Node *>::const_iterator moving_node_ind_pos = find (nodes.begin(),nodes.end(),moving_node_ind);\n";
  list<Node *>::const_iterator moving_node_ind_pos = find (nodes.begin(),nodes.end(),moving_node_ind);

  list<Node *>::const_iterator nb = moving_node_ind_pos;
  //cerr << "Done\n";
  nb++;
  if (nb == nodes.end()) {
    nb = nodes.begin();
  } 

  neighbor_of_moving_node[0]=*(*nb); 

  nb=moving_node_ind_pos;
  if (nb == nodes.begin()) {
    nb = nodes.end();
  }
  nb--;

  neighbor_of_moving_node[1]=*( *nb ); 


  for (list<Node *>::const_iterator i=nodes.begin(); i!=nodes.end(); i++) {
    for (int j=0;j<2;j++) { // loop over the two neighbors of moving node
      list<Node *>::const_iterator nb=i;
      nb++;
      if (nb == nodes.end()) {
	nb = nodes.begin();
      } 
      if (*i == moving_node_ind || *nb == moving_node_ind) {
	// do not compare to self
	continue;
      }

      Vector v3 = *(*i);
      Vector v4 = *(*nb);

      double denominator = 
	(v4.y - v3.y)*(neighbor_of_moving_node[j].x - new_pos.x) - (v4.x - v3.x)*(neighbor_of_moving_node[j].y - new_pos.y);

      double ua = 
	((v4.x - v3.x)*(new_pos.y - v3.y) - (v4.y - v3.y)*(new_pos.x -v3.x))/denominator;
      double ub = 
	((neighbor_of_moving_node[j].x - new_pos.x)*(new_pos.y-v3.y) - (neighbor_of_moving_node[j].y- new_pos.y)*(new_pos.x - v3.x))/denominator;

      if ( ( TINY < ua && ua < 1.-TINY ) && ( TINY < ub && ub < 1.-TINY ) ) {
	//cerr << "ua = " << ua << ", ub = " << ub << endl;
	return true;
      }
    }
  }
  return false;
}



*/

bool Cell::MoveSelfIntersectsP(Node *moving_node_ind, Vector new_pos)
{
    
    // Check whether the polygon will self-intersect if moving_node_ind
    // were displaced to new_pos
    
    // Compare the two new edges against each other edge
    
    // O(2*N)
    
    // method used for segment intersection:
    // http://astronomy.swin.edu.au/~pbourke/geometry/lineline2d/
    
    Vector neighbor_of_moving_node[2];
    
    //cerr << "list<Node *>::const_iterator moving_node_ind_pos = find (nodes.begin(),nodes.end(),moving_node_ind);\n";
    list<Node *>::const_iterator moving_node_ind_pos = find (nodes.begin(),nodes.end(),moving_node_ind);
    
    list<Node *>::const_iterator nb = moving_node_ind_pos;
    //cerr << "Done\n";
    nb++;
    if (nb == nodes.end()) {
        nb = nodes.begin();
    }
    
    neighbor_of_moving_node[0]=*(*nb);
    
    nb=moving_node_ind_pos;
    if (nb == nodes.begin()) {
        nb = nodes.end();
    }
    nb--;
    
    neighbor_of_moving_node[1]=*( *nb );
    
    
    for (list<Node *>::const_iterator i=nodes.begin(); i!=nodes.end(); i++) {
        for (int j=0;j<2;j++) { // loop over the two neighbors of moving node
            list<Node *>::const_iterator nb=i;
            nb++;
            if (nb == nodes.end()) {
                nb = nodes.begin();
            }
            if (*i == moving_node_ind || *nb == moving_node_ind) {
                // do not compare to self
                continue;
            }
            
            Vector v3 = *(*i);
            Vector v4 = *(*nb);
            
            double denominator =
            (v4.y - v3.y)*(neighbor_of_moving_node[j].x - new_pos.x) - (v4.x - v3.x)*(neighbor_of_moving_node[j].y - new_pos.y);
            
           //  double ua =
           //  ((v4.x - v3.x)*(new_pos.y - v3.y) - (v4.y - v3.y)*(new_pos.x -v3.x))/denominator;
           //  double ub =
           //  ((neighbor_of_moving_node[j].x - new_pos.x)*(new_pos.y-v3.y) - (neighbor_of_moving_node[j].y- new_pos.y)*(new_pos.x - v3.x))/denominator;

            double numera = ((v4.x - v3.x)*(new_pos.y - v3.y) - (v4.y - v3.y)*(new_pos.x -v3.x));
            double numerb = ((neighbor_of_moving_node[j].x - new_pos.x)*(new_pos.y-v3.y) - (neighbor_of_moving_node[j].y- new_pos.y)*(new_pos.x - v3.x));
            
            // Are the wall elements coincident?
            if (fabs(numera) < TINY && fabs(numerb) < TINY && fabs(denominator) < TINY) {
                return true;
            }
            
            // Are the wall elements parallel?
            if (fabs(denominator) < TINY) {
                continue;
            }
            double ua = numera / denominator;
            double ub = numerb / denominator;
            
            
            //if ( ( TINY < ua && ua < 1.-TINY ) && ( TINY < ub && ub < 1.-TINY ) ) {
            if ( ( 0 < ua && ua < 1. ) && ( 0 < ub && ub < 1.) ) {
                //cerr << "ua = " << ua << ", ub = " << ub << endl;
                return true;
            }
        }
    }
    return false;
}




bool Cell::LinePieceIntersectsP(const Vector n1, const Vector n2) const
{
    
    // Check whether the polygon will self-intersect if moving_node_ind
    // were displaced to new_pos
    
    // Compare the two new edges against each other edge
    
    // O(2*N)
    
    // method used for segment intersection:
    // http://astronomy.swin.edu.au/~pbourke/geometry/lineline2d/
    
    /*Vector neighbor_of_moving_node[2];
    
    //cerr << "list<Node *>::const_iterator moving_node_ind_pos = find (nodes.begin(),nodes.end(),moving_node_ind);\n";
    list<Node *>::const_iterator moving_node_ind_pos = find (nodes.begin(),nodes.end(),moving_node_ind);
    
    list<Node *>::const_iterator nb = moving_node_ind_pos;
    //cerr << "Done\n";
    nb++;
    if (nb == nodes.end()) {
        nb = nodes.begin();
    }
    
    neighbor_of_moving_node[0]=*(*nb);
    
    nb=moving_node_ind_pos;
    if (nb == nodes.begin()) {
        nb = nodes.end();
    }
    nb--;
    
    neighbor_of_moving_node[1]=*( *nb );
    */
    
    for (list<Node *>::const_iterator i=nodes.begin(); i!=nodes.end(); i++) {
        list<Node *>::const_iterator nb=i;
        nb++;
        if (nb == nodes.end()) {
            nb = nodes.begin();
        }
        
       /* if (*i == moving_node_ind || *nb == moving_node_ind) {
            // do not compare to self
            continue;
        }*/
        
        Vector v3 = *(*i);
        Vector v4 = *(*nb);
        
        double denominator =
        (v4.y - v3.y)*(n1.x - n2.x) - (v4.x - v3.x)*(n1.y - n2.y);
        
        /* double ua =
         ((v4.x - v3.x)*(new_pos.y - v3.y) - (v4.y - v3.y)*(new_pos.x -v3.x))/denominator;
         double ub =
         ((neighbor_of_moving_node[j].x - new_pos.x)*(new_pos.y-v3.y) - (neighbor_of_moving_node[j].y- new_pos.y)*(new_pos.x - v3.x))/denominator;*/
        double numera = ((v4.x - v3.x)*(n1.y - v3.y) - (v4.y - v3.y)*(n1.x -v3.x));
        double numerb = ((n2.x - n1.x)*(n1.y-v3.y) - (n2.y- n1.y)*(n1.x - v3.x));
        
        /* Are the wall elements coincident? */
        if (fabs(numera) < TINY && fabs(numerb) < TINY && fabs(denominator) < TINY) {
            return true;
        }
        
        /* Are the wall elements parallel? */
        if (fabs(denominator) < TINY) {
            continue;
        }
        double ua = numera / denominator;
        double ub = numerb / denominator;
        
        
        //if ( ( TINY < ua && ua < 1.-TINY ) && ( TINY < ub && ub < 1.-TINY ) ) {
        if ( ( 0 < ua && ua < 1. ) && ( 0 < ub && ub < 1.) ) {
            //cerr << "ua = " << ua << ", ub = " << ub << endl;
            return true;
        }
        
    }
    return false;
}

/*! \brief Test if this cell intersects with the given line.

 */
bool Cell::IntersectsWithLineP(const Vector v1, const Vector v2)
{
  // Compare the line against each edge
  // method used: http://astronomy.swin.edu.au/~pbourke/geometry/lineline2d/

  for (list<Node *>::const_iterator i=nodes.begin(); i!=nodes.end(); i++) 
    {
      Vector v3 = *(*i);
      list<Node *>::const_iterator nb=i;
      nb++;
      if (nb == nodes.end()) {
	nb = nodes.begin();
      }
      Vector v4 = *(*nb);

      double denominator = 
	(v4.y - v3.y)*(v2.x - v1.x) - (v4.x - v3.x)*(v2.y - v1.y);

      double ua = 
	((v4.x - v3.x)*(v1.y - v3.y) - (v4.y - v3.y)*(v1.x -v3.x))/denominator;
      double ub = 
	((v2.x - v1.x)*(v1.y-v3.y) - (v2.y- v1.y)*(v1.x - v3.x))/denominator;

      if ( ( TINY < ua && ua < 1.-TINY ) && ( TINY < ub && ub < 1.-TINY ) ) {
	return true;
      }
    }
  return false;
}
/*! \brief Constructs Walls, but only one per cell boundary.

  Standard method constructs a Wall for each cell wall element,
  making transport algorithms computationally more intensive than needed.

  We can remove this? Well, let's leave it in the code in case we need it for something else. E.g. for importing leaf architectures in different formats than our own... :-)

*/
void Cell::ConstructWalls(void)
{
  return;
  if (dead) return;

  walls.clear();
  neighbors.clear();

  // Get "corner points; i.e. nodes where more than 2 cells are connected
  list<Node *> corner_points;

  for (list<Node *>::const_iterator i=nodes.begin(); i!=nodes.end();i++) {

    // look for nodes belonging to >2 cells
    if ((*i)->owners.size()>2) {

      // push onto list
      corner_points.push_back(*i);
    }
  }

  // Construct Walls between corner points

  // previous one in list
  list<Node *>::const_iterator nb = (--corner_points.end());

  // loop over list, 
  for (list<Node *>::const_iterator i=corner_points.begin(); i!=corner_points.end(); ( i++, nb++) ) {

    if (nb==corner_points.end()) nb=corner_points.begin();
    // add owning cells to a list
    list<Cell *> owning_cells;
    Node &n(*(*i));

    for (list<Neighbor>::const_iterator j=n.owners.begin(); j!=n.owners.end(); j++) {
      owning_cells.push_back(j->cell);
    }

    Node &n2(*(*nb));
    for (list<Neighbor>::const_iterator j=n2.owners.begin(); j!=n2.owners.end(); j++) {
      owning_cells.push_back(j->cell);
    }

    // sort cell owners
    owning_cells.sort( mem_fun( &Cell::Cmp ));

    // find duplicates
    vector<Cell *> duplicates;
    list<Cell *>::const_iterator prevj = (--owning_cells.end());
    for (list<Cell *>::const_iterator j=owning_cells.begin(); j!=owning_cells.end(); ( j++, prevj++) ) {
      if (prevj==owning_cells.end())
	prevj=owning_cells.begin();
      if (*j==*prevj) 
	duplicates.push_back(*j);
    }

    if (duplicates.size()==3) { // ignore cell boundary (this occurs only after the first division, I think)
      vector<Cell *>::iterator dup_it = find_if(duplicates.begin(),duplicates.end(),mem_fun(&Cell::BoundaryPolP) );
      if (dup_it!=duplicates.end()) 
	duplicates.erase(dup_it);
      else {
	return;
      }
    }


    // One Wall for each neighbor, so we should be able to correctly construct neighbor lists here.
    if (duplicates[0]==this) {
      AddWall(  new Wall(*nb,*i,duplicates[0],duplicates[1]) );
      if (!duplicates[1]->BoundaryPolP()) {

	neighbors.push_back(duplicates[1]);
      }
    } else {
      AddWall ( new Wall(*nb,*i,duplicates[1],duplicates[0]) );
      if (!duplicates[0]->BoundaryPolP()) {
	neighbors.push_back(duplicates[0]);

      }
    }
  }
}


void BoundaryPolygon::Draw(QGraphicsScene *c, QString tooltip)
{

  // Draw the BoundaryPolygon on a QCanvas object

  CellItem* p = new CellItem(this, c);

  QPolygonF pa(nodes.size());
  int cc=0;

  for (list<Node *>::const_iterator n=nodes.begin(); n!=nodes.end(); n++) {
    Node *i=*n;
    pa[cc++] = QPoint((qreal)((Offset().x+i->x)*Factor()), (qreal)((Offset().y+i->y)*Factor()) );
  }

  p->setPolygon(pa);
  p->setPen(par.outlinewidth>=0?QPen( QColor(par.cell_outline_color),par.outlinewidth):QPen(Qt::NoPen));
  p->setBrush( Qt::NoBrush );
  p->setZValue(1);

  if (!tooltip.isEmpty()) {
    p->setToolTip(tooltip);
  }

  c->addItem(p);
  p->show();
}

void Cell::Flux(double *flux, double *D)
{
  // loop over cell edges

  for (int c=0;c<NChem();c++)
    flux[c]=0.;

  for (list<Wall *>::iterator i=walls.begin(); i!=walls.end(); i++) {

    // leaf cannot take up chemicals from environment ("no flux boundary")
    if ((*i)->c2->BoundaryPolP()) continue;


    // flux depends on edge length and concentration difference
    for (int c=0;c<NChem();c++) {
      double phi = (*i)->length * ( D[c] ) * ( ((Cell *)(*i)->c2)->chem[c] - chem[c] );

#ifdef QDEBUG
      if ((*i)->c1!=this) {
	qDebug() << "Warning, bad cells boundary: " << (*i)->c1->Index() << ", " << index << endl;
      }
#endif

      flux[c] += phi;
    }    
  }
}


// graphics stuff, not compiled for batch versions
#ifdef QTGRAPHICS

#include "canvas.h"

void Cell::Draw(QGraphicsScene *c, QString tooltip)
{

  // Draw the cell on a QCanvas object

  if (DeadP()) { 
#ifdef QDEBUG
    qDebug() << "Cell " << index << " not drawn, because dead." << endl;
#endif
    return;
  }

  CellItem* p = new CellItem(this, c);

  QPolygonF pa(nodes.size());
  int cc=0;

  for (list<Node *>::const_iterator n=nodes.begin(); n!=nodes.end(); n++) {
    Node *i=*n;

    pa[cc++] = QPointF((qreal)((offset[0]+i->x)*factor),
              (qreal)((offset[1]+i->y)*factor) );
  }


  QColor cell_color;

  m->plugin->SetCellColor(this,&cell_color);

  p->setPolygon(pa);
  p->setPen(par.outlinewidth>=0?QPen( QColor(par.cell_outline_color),par.outlinewidth):QPen(Qt::NoPen));
  p->setBrush( cell_color );
  p->setZValue(1);

  if (!tooltip.isEmpty()) {
    p->setToolTip(tooltip);
  }
  c->addItem(p);
  p->show();
}


void Cell::DrawCenter(QGraphicsScene *c) const {
  // Maginfication derived similarly to that in nodeitem.cpp
  // Why not use Cell::Magnification()?
  const double mag = par.node_mag;

  // construct an ellipse
  QGraphicsEllipseItem *disk = new QGraphicsEllipseItem ( -1*mag, -1*mag, 2*mag, 2*mag, 0);
  disk->setBrush( QColor("forest green") );
  disk->setZValue(5);
  Vector centroid=Centroid();
  disk -> setPos((offset[0]+centroid.x)*factor,(offset[1]+centroid.y)*factor);
    c->addItem(disk);
      disk->show();
}

void Cell::DrawNodes(QGraphicsScene *c) const {

  for (list<Node *>::const_iterator n=nodes.begin(); n!=nodes.end(); n++) {
    Node *i=*n;

    NodeItem *item = new NodeItem ( &(*i), c );
    item->setColor();
    item->setZValue(5);
    item ->setPos(((offset[0]+i->x)*factor), ((offset[1]+i->y)*factor) );
      c->addItem(item);
          item->show();
      
  }
}

void Cell::DrawIndex(QGraphicsScene *c) const {

  DrawText( c, QString("%1").arg(index));
}

// Draw any text in the cell's center
void Cell::DrawText(QGraphicsScene *c, const QString &text) const {
    
    Vector centroid = Centroid();
    QGraphicsSimpleTextItem *ctext = new QGraphicsSimpleTextItem ( text, 0);
   // ctext->setPen( QPen(QColor(par.textcolor)) );
    ctext->setBrush( QBrush(QColor(par.textcolor)) );
    ctext->setZValue(20);
    ctext->setFont( QFont( "Helvetica", par.cellnumsize, QFont::Normal) );
    
    ctext ->setPos(((offset[0]+centroid.x)*factor),
                   ((offset[1]+centroid.y)*factor) );
    c->addItem(ctext);
    ctext->show();
    
}


void Cell::DrawAxis(QGraphicsScene *c) const {

  Vector long_axis;
  double width;
  Length(&long_axis, &width);

  //cerr << "Length is "  << length << endl;
  long_axis.Normalise();
  Vector short_axis=long_axis.Perp2D();


  Vector centroid = Centroid();
  Vector from = centroid - 0.5 * width * short_axis;
  Vector to = centroid + 0.5 * width *short_axis;


  QGraphicsLineItem *line = new QGraphicsLineItem(0);
  line->setPen( QPen(QColor(par.arrowcolor),2) );
  line->setZValue(2);

  line->setLine( ( (offset[0]+from.x)*factor ),
		 ( (offset[1]+from.y)*factor ), 
		 ( (offset[0]+to.x)*factor ),
		 ( (offset[1]+to.y)*factor ) );
  line->setZValue(10);
    c->addItem(line);
  line->show();
    
}

void Cell::DrawStrain(QGraphicsScene *c) const {
  MyWarning::warning("Sorry, Cell::DrawStrain temporarily not implemented.");
}


void Cell::DrawFluxes(QGraphicsScene *c, double arrowsize)
{

  // get the mean flux through this cell
  Vector vec_flux = ReduceCellAndWalls<Vector>( PINdir );

  vec_flux.Normalise();

  vec_flux *= arrowsize;

  QGraphicsArrowItem *arrow = new QGraphicsArrowItem(0);

  Vector centroid = Centroid();
  Vector from = centroid - vec_flux/2.;
  Vector to = centroid + vec_flux/2.;


  arrow->setPen( QPen(QColor(par.arrowcolor),par.outlinewidth));
  arrow->setZValue(2);

  arrow->setLine( ( (offset[0]+from.x)*factor ),
		  ( (offset[1]+from.y)*factor ), 
		  ( (offset[0]+to.x)*factor ),
		  ( (offset[1]+to.y)*factor ) );
  arrow->setZValue(10);
    c->addItem(arrow);
  arrow->show();
}


void Cell::DrawWalls(QGraphicsScene *c) const {

  for_each(walls.begin(), walls.end(), bind2nd ( mem_fun ( &Wall::Draw ) , c ) );

  // to see the cells connected the each wall (for debugging), uncomment the following
  //for_each(walls.begin(), walls.end(), bind2nd ( mem_fun ( &Wall::ShowStructure ), c ) );
}


void Cell::DrawValence(QGraphicsScene *c) const {

  DrawText(c, QString("%1").arg(walls.size()) );
}

#endif // QTGRAPHICS !

/*! \brief Recalculate the lengths of the cell's Walls.

  Call this function after the Monte Carlo updates, and before doing the reaction-diffusion iterations.

*/
void Cell::SetWallLengths(void)
{

  for (list<Wall *>::iterator de=walls.begin(); de!=walls.end(); de++) {

    // Step 1: find the path of nodes leading along the Wall.
    // A Wall often represents a curved cell wall: we want the total
    // length _along_ the wall here...


    // Locate first and second nodes of the edge in list of nodes
    list<Node *>::const_iterator first_node_edge = find(nodes.begin(), nodes.end(), (*de)->n1);
    list<Node *>::const_iterator second_node_edge_plus_1 = ++find(nodes.begin(), nodes.end(), (*de)->n2);

    double sum_length = 0.;

    // Now, walk to the second node of the edge in the list of nodes
    for (list<Node *>::const_iterator n=++first_node_edge; n!=second_node_edge_plus_1; ++n ) {
      if (n==nodes.end())
	n=nodes.begin(); /* wrap around */ 
      list<Node *>::const_iterator prev_n = n; 
      if (prev_n==nodes.begin())
	prev_n=nodes.end();
      --prev_n;

      // Note that Node derives from a Vector, so we can do vector calculus as defined in vector.h 
      sum_length += (*(*prev_n) - *(*n)).Norm(); 

      //cerr << "Node " << *prev_n << " to " << *n << ", cumulative length = " << sum_length << endl;
    }

    // We got the total length of the Wall now, store it:
    (*de)->length = sum_length;

    //cerr << endl;
    // goto next de
  }
}


//! Add Wall w to the list of Walls
void Cell::AddWall( Wall *w )
{

  // if necessary, we could try later inserting it at the correct position
#ifdef QDEBUG
  if (w->c1 == w->c2 ){
    qDebug() << "Wall between identical cells: " << w->c1->Index()<< endl;
  }
#endif

  // Add Wall to Cell's list
  walls.push_back( w );

  // Add wall to Mesh's list if it isn't there yet
  if (find ( m->walls.begin(), m->walls.end(), w ) == m->walls.end() ) { m->walls.push_back(w);
  }
}

//! Remove Wall w from the list of Walls
list<Wall *>::iterator Cell::RemoveWall( Wall *w )
{

  // remove wall from Mesh's list
  m->walls.erase( find( m->walls.begin(), m->walls.end(), w ) );

  // remove wall from Cell's list
  return walls.erase (find( walls.begin(), walls.end(), w ));
}


void Cell::EmitValues(double t)
{
  //  cerr << "Attempting to emit " << t << ", " << chem[0] << ", " << chem[1] << endl;
  emit ChemMonValue(t, chem);
}

/* finis */
