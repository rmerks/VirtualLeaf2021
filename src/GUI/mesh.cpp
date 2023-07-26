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

#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <cstdlib>
//#include <cerrno>
#include <cstring>
#include <numeric>
#include <functional>
#include <fstream>
#include <QPair>
#include "mesh.h"
#include "tiny.h"
#include "parameter.h"
#include "random.h"
#include "pi.h"
#include "parse.h"
#include "matrix.h"
#include "sqr.h"
#include "nodeset.h"
#include "nodeitem.h"
#include "simplugin.h"
#include "forwardeuler.h"

#include <QDebug>
#include <set>
#include <iostream>
#include <iterator>
#include <QTextStream>

static const std::string _module_id("$Id$");

extern Parameter par;

void Mesh::AddNodeToCellAtIndex(Cell *c, Node *n, Node *nb1, Node *nb2, list<Node *>::iterator ins_pos) {
  c->nodes.insert(ins_pos, n);        
  n->owners.push_back( Neighbor(c, nb1, nb2 ) );
}


void Mesh::AddNodeToCell(Cell *c, Node *n, Node *nb1, Node *nb2) {

  c->nodes.push_back( n );
  n->owners.push_back( Neighbor(c, nb1, nb2 ) );
}

void Mesh::PerturbChem(int chemnum, double range) {

  for (vector<Cell *>::iterator i=cells.begin(); i!=cells.end(); i++) {
    (*i)->chem[chemnum] += range*(RANDOM()-0.5);
    if ((*i)->chem[chemnum]<0.) (*i)->chem[chemnum]=0.;
  }
}

void Mesh::CellFiles(const Vector ll, const Vector ur) {

  Cell *cell = RectangularCell(ll,ur,0.001); 

  for (int c=0;c<Cell::NChem();c++) {
    cell->SetChemical(c,par.initval[c]);
  }

  cell->SetTargetArea(cell->CalcArea());

  Vector axis(1,0,0);

  // divide rectangle a number of times
  for (int i=0;i<6;i++) {
    IncreaseCellCapacityIfNecessary();

    vector <Cell *> current_cells = cells;
    for (vector<Cell *>::iterator j=current_cells.begin(); j!=current_cells.end();j++) {
      (*j)->DivideOverAxis(axis);
    }
    axis=axis.Perp2D();

  }

  IncreaseCellCapacityIfNecessary();

  axis=axis.Perp2D();

  vector <Cell *> current_cells = cells;
  for (vector<Cell *>::iterator j=current_cells.begin(); j!=current_cells.end();j++) {
    (*j)->DivideOverAxis(axis);
  }


  double sum_l=0; int n_l=0;
  for (list<Node *>::const_iterator i=cell->nodes.begin(); i!=cell->nodes.end(); i++) {
    list<Node *>::const_iterator nb=i; nb++; 
    if (nb==cell->nodes.end()) 
      nb=cell->nodes.begin();

    double l = (**nb-**i).Norm();

    sum_l += l;
    n_l++;

  }


  Node::target_length = sum_l/(double)n_l;
  // a bit more tension
  Node::target_length/=4.;

  SetBaseArea();
}

Cell *Mesh::RectangularCell(const Vector ll, const Vector ur, double rotation) {

  Cell *cell=AddCell(new Cell());
  cell->m=this;

  Matrix rotmat;
  rotmat.Rot2D(rotation); // rotation over 0,0

  Node *n1=AddNode(new Node(rotmat * ll));
  Node *n2=AddNode(new Node(rotmat * Vector(ll.x, ur.y,0)));
  Node *n3=AddNode(new Node(rotmat * ur));
  Node *n4=AddNode(new Node(rotmat * Vector(ur.x, ll.y,0)));

  n1->boundary=true;
  n2->boundary=true;
  n3->boundary=true;
  n4->boundary=true;

  //n1.fixed=n2.fixed=n3.fixed=n4.fixed=true;

  AddNodeToCell(cell, n4, 
		n1,
		n3);

  AddNodeToCell(cell, n3, 
		n4,
		n2);

  AddNodeToCell(cell, n2, 
		n3,
		n1);

  AddNodeToCell(cell, n1, 
		n2,
		n4);


  AddNodeToCell(boundary_polygon, n4, 
		n1,
		n3);
  AddNodeToCell(boundary_polygon, n3, 
		n4,
		n2);
  AddNodeToCell(boundary_polygon, n2, 
		n3,
		n1);
  AddNodeToCell(boundary_polygon, n1, 
		n2,
		n4);

  cell->setCellVec(Vector(0,1,0));

  boundary_polygon->m = this;
  boundary_polygon->area = 0;

  cell->area = cell->CalcArea();
  // target length is the length of the elements

  Node::target_length = ur.y-ll.y;
  // a bit more tension
  Node::target_length/=2;

  cell->SetIntegrals(); 
  cell->ConstructNeighborList();

  return cell;
}

Cell &Mesh::EllipticCell(double xc, double yc, double ra, double rb,  int nnodes, double rotation) {

  int first_node=Node::nnodes;
  //  nodes.reserve(nodes.size()+nnodes);


  //cells.push_back(Cell(xc,yc));
  Cell *c=AddCell(new Cell(xc,yc));
  c->m=this;

  for (int i=0;i<nnodes;i++) {

    double angle=2*Pi*(i/(double)nnodes);
    double x=xc+ra*cos(angle)*cos(rotation) - rb*sin(angle)*sin(rotation);
    double y=yc+ra*cos(angle)*sin(rotation) + rb*sin(angle)*cos(rotation);


    Node *n=AddNode(new Node(x,y,0));
    n->boundary = true;

  } 

  for (int i=0;i<nnodes;i++) {

    AddNodeToCell(c,
		  nodes[first_node + i],
		  nodes[first_node+ (nnodes+i-1)%nnodes],
		  nodes[first_node+ (i + 1)%nnodes]);
    AddNodeToCell(boundary_polygon,
		  nodes[first_node + i],
		  nodes[first_node+ (nnodes+i-1)%nnodes],
		  nodes[first_node+ (i + 1)%nnodes]);
  }

  boundary_polygon->m = this;
  boundary_polygon->area = 0;

  c->area = c->CalcArea();
  // target length is the length of the elements

  Node::target_length = (2 * ((ra +rb)/2.) * sin (Pi/nnodes));
  // a bit more tension
  Node::target_length/=2;

  c->SetIntegrals(); 
  c->at_boundary=true;

  return *c;
}

Cell &Mesh::LeafPrimordium(int nnodes, double pet_length) {

  // first leaf cell

  int first_node=Node::nnodes;

  Cell *circle=AddCell(new Cell(0,0));
  circle->m=this;
  const double ra=10, rb=10;
  const double xc=0,yc=0;
  const double rotation=0;
  for (int i=0;i<nnodes;i++) {

    double angle=2*Pi*(i/(double)nnodes);
    double x=xc+ra*cos(angle)*cos(rotation) - rb*sin(angle)*sin(rotation);
    double y=yc+ra*cos(angle)*sin(rotation) + rb*sin(angle)*cos(rotation);


    Node *n=AddNode(new Node(x,y,0));

    /* if (angle > 1.25*Pi && angle < 1.75*Pi ) {
       n.sam = true;
       }*/

    AddNodeToCell(circle,
		  n,
		  nodes[first_node+ (nnodes+i-1)%nnodes],
		  nodes[first_node+ (i + 1)%nnodes]);

  }

  boundary_polygon->m = this;
  boundary_polygon->area = 0;

  circle->area = circle->CalcArea();
  // target length is the length of the elements

  Node::target_length = (2 * ((ra +rb)/2.) * sin (Pi/nnodes));
  // a bit more tension
  Node::target_length/=2;

  circle->SetIntegrals(); 

  //return c;

  circle->SetTargetArea(2*circle->Area());

  // Petiole: starts at both sides of the circular cell
  // get position of the (n/4)'th and (3*(n/4))'th node.

  list<Node *>::reverse_iterator it_n1=circle->nodes.rbegin();
  for (int i=0; i<nnodes/2; i++) 
    it_n1++;
  it_n1--;

  list<Node *>::reverse_iterator it_n2=--circle->nodes.rend();

  Cell *petiole=AddCell(new Cell());

  Node *n1 = *it_n1;
  Node *n2 = *it_n2;

  Node *n3=AddNode( new Node ( *n2 + Vector( 0, pet_length, 0) ) );
  Node *n4=AddNode( new Node ( *n1 + Vector( 0, pet_length, 0) ) );

  n3->boundary=true;
  n4->boundary=true;

  AddNodeToCell(petiole, *it_n1, 
		n4,
		nodes[(*it_n2)->Index() 
		      + (( (*it_n1)->Index() - (*it_n2)->Index() )-1+nnodes)%nnodes]);



  list<Node *>::reverse_iterator i=it_n1; i++;
  for (; i!=it_n2; ++i) {
    AddNodeToCell(petiole, *i,
		  nodes[(*it_n2)->Index() + (((*i)->Index()-(*it_n2)->Index()) + 1)%nnodes],
		  nodes[(*it_n2)->Index() + (((*i)->Index()-(*it_n2)->Index())-1+nnodes)%nnodes]);
  }

  AddNodeToCell(petiole, *it_n2, *it_n2 + 1, n3);

  (*it_n2)->boundary=true;

  AddNodeToCell(petiole, n3, n2, n4);
  AddNodeToCell(petiole, n4, n3, n1);


#ifdef QDEBUG  
  qDebug() << circle << endl;
  qDebug() << petiole << endl;
#endif

  AddNodeToCell(boundary_polygon, *it_n1, n4, *it_n2 + ((*it_n1-*it_n2)+1)%nnodes); // is this gonna work?

  (*it_n1)->boundary=true;

  for (int i=0;i<nnodes;i++) {

    if (nodes[(first_node + i)]->owners.size()==1) {
      AddNodeToCell(boundary_polygon,
		    nodes[first_node +i],
		    nodes[first_node+ (nnodes+i-1)%nnodes],
		    nodes[first_node+ (i + 1)%nnodes]);

      nodes[first_node+i]->boundary=true;
    }
  }

  AddNodeToCell(boundary_polygon, *it_n2, nodes[(nnodes+(*it_n2)->Index() - 1)%nnodes], n3);
  AddNodeToCell(boundary_polygon, n3, n2, n4);
  AddNodeToCell(boundary_polygon, n4, n3, n1);

  // make petiole solid
  for (list<Node *>::iterator i=petiole->nodes.begin(); i!=petiole->nodes.end(); i++) {
    (*i)->Fix();
  }
  petiole->Fix();

  petiole->area=petiole->CalcArea();
  petiole->target_area=petiole->area;  
  petiole->ConstructNeighborList();
  circle->ConstructNeighborList();
  boundary_polygon->ConstructConnections();
  boundary_polygon->ConstructNeighborList();

  circle->setCellVec(Vector(0,1,0));

  return *circle;
}

/*Cell &Mesh::Box() {
  }*/


// return bounding box of mesh
void Mesh::BoundingBox(Vector &LowerLeft, Vector &UpperRight) {

  LowerLeft = **nodes.begin();
  UpperRight = **nodes.begin();
  for (vector<Node *>::iterator c=nodes.begin(); c!=nodes.end(); c++) {
    if ((*c)->x < LowerLeft.x)
      LowerLeft.x = (*c)->x;
    if ((*c)->y < LowerLeft.y)
      LowerLeft.y = (*c)->y;
    if ((*c)->z < LowerLeft.z)
      LowerLeft.z = (*c)->z;
    if ((*c)->x > UpperRight.x) 
      UpperRight.x = (*c)->x;
    if ((*c)->y > UpperRight.y) 
      UpperRight.y = (*c)->y;
    if ((*c)->z > UpperRight.z)
      UpperRight.z = (*c)->z;
  }
}


double Mesh::Area(void) {

  double area=0;
  vector<Cell *>::iterator i=cells.begin();
  while (i != cells.end()) {
    area += (*(i++))->Area();
  }
  return area;
}

void Mesh::SetBaseArea(void) {

  // Set base area to mean area. 
  // This method is typically called during initiation, after
  // defining the first cell
  Cell::BaseArea() = Area()/cells.size();
}

// for optimization, we moved Displace to Mesh

class DeltaIntgrl {

public:
  double area;
  double ix, iy;
  double ixx,ixy,iyy;
  DeltaIntgrl(double sarea,double six,double siy,double sixx,double sixy,double siyy) {
    area=sarea;
    ix=six;
    iy=siy;
    ixx=sixx;
    ixy=sixy;
    iyy=siyy;
  }
};

void Mesh::Clear(void) {

  // clear nodes
  for (vector<Node *>::iterator i=nodes.begin(); i!=nodes.end(); i++) {
    delete *i;
  }

  nodes.clear();
  Node::nnodes=0;

  node_insertion_queue.clear();
  // Clear NodeSets
  for (vector<NodeSet *>::iterator i=node_sets.begin(); i!=node_sets.end(); i++) {
    delete *i;
  }

  node_sets.clear();
  time = 0;

  // clear cells

  for (vector<Cell *>::iterator i=cells.begin(); i!=cells.end(); i++) {
    delete *i;
  }

  cells.clear();
  Cell::NCells() = 0;

  if (boundary_polygon) {
    delete boundary_polygon;
    boundary_polygon=0;
  }

  // Clear walls
  for (list<Wall *>::iterator i=walls.begin(); i!=walls.end(); i++) {
    delete *i;
  }

  walls.clear();
  WallBase::nwalls = 0;
  //tmp_walls->clear();

  shuffled_cells.clear();
  shuffled_nodes.clear();
 
#ifdef QDEBUG
  qDebug() << "cells.size() = " << cells.size() << endl;
  qDebug() << "walls.size() = " << walls.size() << endl;
  qDebug() << "nodes.size() = " << nodes.size() << endl;
#endif
}

double Mesh::CellSpecificStiffnessOneSide(Node *nb,set<int> &nodeown) {
    // determines the list of cells belonging to the node and its neighbors
    set<int> nb1own;
    vector<int> intersection_no_nb1;
    for (list<Neighbor>::iterator c=nb->owners.begin(); c!=nb->owners.end(); c++) {
      nb1own.insert(c->cell->Index());
    }
    set_intersection(nodeown.begin(), nodeown.end(), nb1own.begin(), nb1own.end(), back_inserter(intersection_no_nb1));

	double cell_w=0.;
    // uses the cell specific stiffness values to calculate length_dh as introduced in Lebovka et al
    for (list<Neighbor>::iterator c=nb->owners.begin(); c!=nb->owners.end(); c++) {
      if (std::find(intersection_no_nb1.begin(), intersection_no_nb1.end(), c->cell->Index()) != intersection_no_nb1.end()) {
    	  cell_w += (c->cell->Index() !=-1?c->cell->GetWallStiffness():par.rel_perimeter_stiffness);
      }
      if (std::isnan(cell_w)) {
    	  break;
      }
    }
    return cell_w;
}

double Mesh::DisplaceNodes(void) {

  MyUrand r(shuffled_nodes.size());
  vl_shuffle(shuffled_nodes.begin(),shuffled_nodes.end(),r);

  double sum_dh=0;

  list<DeltaIntgrl> delta_intgrl_list;

  for_each( node_sets.begin(), node_sets.end(), mem_fn( &NodeSet::ResetDone ) );

  for (vector<Node *>::const_iterator i=shuffled_nodes.begin(); i!=shuffled_nodes.end(); i++) {

    //int n=shuffled_nodes[*i];
    Node &node(**i);

    // Do not allow displacement if fixed
    if (node.fixed) continue;

    if (node.DeadP()) continue;

    // Attempt to move this cell in a random direction
    double rx=par.mc_stepsize*(RANDOM()-0.5); // was 100.
    double ry=par.mc_stepsize*(RANDOM()-0.5);

    // Uniform with a circle of radius par.mc_stepsize
    /* double r = RANDOM() * par.mc_stepsize;
       double th = RANDOM()*2*Pi;

       double rx = r * cos(th);
       double ry = r * sin(th);
    */
    Vector new_p(node.x+rx,node.y+ry,0);
    Vector old_p(node.x,node.y,0);

    /* if (node.boundary  && boundary_polygon->MoveSelfIntersectsP(n,  new_p )) {
    // reject if move of boundary results in self intersection
    continue;
    }*/


    if (node.node_set) {
      // move each node set only once
      if (!node.node_set->DoneP()) 
	node.node_set->AttemptMove(rx,ry);

    } else {

      // for all cells to which this node belongs:
      //   calculate energy difference

      double area_dh=0.;
      double length_dh=0.;
      double bending_dh=0.;
      double cell_length_dh=0.;
      double alignment_dh=0.;

      double old_l1=0.,old_l2=0.,new_l1=0.,new_l2=0.;

      double sum_stiff=0.;
      double dh=0.;

      for (list<Neighbor>::const_iterator cit=node.owners.begin(); cit!=node.owners.end(); cit++) {

	
	Cell &c=*((Cell *)(cit->cell));

	if (c.MoveSelfIntersectsP(&node,  new_p )) {
		
	  // reject if move results in self intersection
	  //
	  // I know: using goto's is bad practice... except when jumping out
	  // of deeply nested loops :-)
	  //cerr << "Rejecting due to self-intersection\n";
	  goto next_node;
	}

	// summing stiffnesses of cells. Move has to overcome this minimum required energy.
	sum_stiff += c.stiffness;
	// area - (area after displacement): see notes for derivation
	
	Vector i_min_1 = *(cit->nb1);
	//Vector i_plus_1 = m->getNode(cit->nb2);
    Vector i_plus_1 = *(cit->nb2);

	//if (cit->cell>=0) {
	if (!cit->cell->BoundaryPolP()) {
	  double delta_A = 0.5 * ( ( new_p.x - old_p.x ) * (i_min_1.y - i_plus_1.y) +
				   ( new_p.y - old_p.y ) * ( i_plus_1.x - i_min_1.x ) );

	  area_dh +=  delta_A * (2 * c.target_area - 2 * c.area + delta_A);


	  // cell length constraint
	  // expensive and not always needed
	  // so we check the value of lambda_celllength

	  if (/* par.lambda_celllength */  cit->cell->lambda_celllength) {

	    double delta_ix = 
	      (i_min_1.x + new_p.x)
	      * (new_p.x * i_min_1.y- i_min_1.x * new_p.y) +
	      (new_p.x + i_plus_1.x)
	      * (i_plus_1.x * new_p.y- new_p.x * i_plus_1.y) -

	      (i_min_1.x + old_p.x)
	      * (old_p.x * i_min_1.y- i_min_1.x * old_p.y) -
	      (old_p.x + i_plus_1.x)
	      * (i_plus_1.x * old_p.y - old_p.x * i_plus_1.y);


	    double delta_iy =
	      (i_min_1.y + new_p.y)
	      * (new_p.x * i_min_1.y- i_min_1.x * new_p.y) +
	      (new_p.y + i_plus_1.y)
	      * (i_plus_1.x * new_p.y- new_p.x * i_plus_1.y) -

	      (i_min_1.y + old_p.y)
	      * (old_p.x * i_min_1.y- i_min_1.x * old_p.y) -
	      (old_p.y + i_plus_1.y)
	      * (i_plus_1.x * old_p.y - old_p.x * i_plus_1.y);


	    double delta_ixx = 
	      (new_p.x*new_p.x+
	       i_min_1.x*new_p.x+
	       i_min_1.x*i_min_1.x ) *
	      (new_p.x*i_min_1.y - i_min_1.x*new_p.y) +

	      (i_plus_1.x*i_plus_1.x+
	       new_p.x*i_plus_1.x+
	       new_p.x*new_p.x ) *
	      (i_plus_1.x*new_p.y - new_p.x*i_plus_1.y) -

	      (old_p.x*old_p.x+
	       i_min_1.x*old_p.x+
	       i_min_1.x*i_min_1.x ) *
	      (old_p.x*i_min_1.y - i_min_1.x*old_p.y) -

	      (i_plus_1.x*i_plus_1.x+
	       old_p.x*i_plus_1.x+
	       old_p.x*old_p.x ) *
	      (i_plus_1.x*old_p.y - old_p.x*i_plus_1.y);


	    double delta_ixy =
	      (i_min_1.x*new_p.y-
	       new_p.x*i_min_1.y)*
	      (new_p.x*(2*new_p.y+i_min_1.y)+
	       i_min_1.x*(new_p.y+2*i_min_1.y)) +

	      (new_p.x*i_plus_1.y-
	       i_plus_1.x*new_p.y)*
	      (i_plus_1.x*(2*i_plus_1.y+new_p.y)+
	       new_p.x*(i_plus_1.y+2*new_p.y)) -

	      (i_min_1.x*old_p.y-
	       old_p.x*i_min_1.y)*
	      (old_p.x*(2*old_p.y+i_min_1.y)+
	       i_min_1.x*(old_p.y+2*i_min_1.y)) -

	      (old_p.x*i_plus_1.y-
	       i_plus_1.x*old_p.y)*
	      (i_plus_1.x*(2*i_plus_1.y+old_p.y)+
	       old_p.x*(i_plus_1.y+2*old_p.y));


	    double delta_iyy = 
	      (new_p.x*i_min_1.y-
	       i_min_1.x*new_p.y)*
	      (new_p.y*new_p.y+
	       i_min_1.y*new_p.y+
	       i_min_1.y*i_min_1.y ) + 

	      (i_plus_1.x*new_p.y-
	       new_p.x*i_plus_1.y)*
	      (i_plus_1.y*i_plus_1.y+
	       new_p.y*i_plus_1.y+
	       new_p.y*new_p.y ) -

	      (old_p.x*i_min_1.y-
	       i_min_1.x*old_p.y)*
	      (old_p.y*old_p.y+
	       i_min_1.y*old_p.y+
	       i_min_1.y*i_min_1.y ) -

	      (i_plus_1.x*old_p.y-
	       old_p.x*i_plus_1.y)*
	      (i_plus_1.y*i_plus_1.y+
	       old_p.y*i_plus_1.y+
	       old_p.y*old_p.y );

	    delta_intgrl_list.push_back(DeltaIntgrl(delta_A,delta_ix,delta_iy,delta_ixx,delta_ixy,delta_iyy));

	    Vector old_axis;
	    double old_celllength = c.Length(&old_axis);
	    old_axis=old_axis.Normalised().Perp2D();

	    // calculate length after proposed update
	    double intrx=(c.intgrl_x-delta_ix)/6.;
	    double intry=(c.intgrl_y-delta_iy)/6.;
	    double ixx=((c.intgrl_xx-delta_ixx)/12.)-(intrx*intrx)/(c.area-delta_A);
	    double ixy=((c.intgrl_xy-delta_ixy)/24.)+(intrx*intry)/(c.area-delta_A);
	    double iyy=((c.intgrl_yy-delta_iyy)/12.)-(intry*intry)/(c.area-delta_A);

	    double rhs1=(ixx+iyy)/2., rhs2=sqrt( (ixx-iyy)*(ixx-iyy)+4*ixy*ixy )/2.;

	    double lambda_b=rhs1+rhs2;


	    double new_celllength=4*sqrt(lambda_b/(c.area-delta_A));
	    //cerr << "new_celllength = "  << new_celllength << endl;
	    //cerr << "target_length = "  << c.target_length << endl;

	    cell_length_dh += c.lambda_celllength * ( DSQR(c.target_length - new_celllength) - DSQR(c.target_length-old_celllength) );

	    Vector norm_long_axis(lambda_b - ixx, ixy, 0);
	    norm_long_axis.Normalise();

	    double alignment_before = InnerProduct(old_axis, c.cellvec);
	    double alignment_after = InnerProduct(norm_long_axis, c.cellvec);

	    /* cerr << "Delta alignment = " << alignment_before - alignment_after << endl;
	       cerr << "Old alignment is " << alignment_before << ", new alignment is " << alignment_after << endl;
	       cerr << "Old axis is " << old_axis << ", new axis is " << norm_long_axis << endl; 
	    */
	    alignment_dh += alignment_before - alignment_after;

	    /* cerr << "alignment_dh  = " << alignment_dh << endl;
	       cerr << "cellvec = " << c.cellvec << endl;*/

	  } else {
	    // if we have no length constraint, still need to update area
	    delta_intgrl_list.push_back(DeltaIntgrl(delta_A,0,0,0,0,0));

	  }

	  old_l1=(old_p-i_min_1).Norm();
	  old_l2=(old_p-i_plus_1).Norm();
	  new_l1=(new_p-i_min_1).Norm();
	  new_l2=(new_p-i_plus_1).Norm();




	  static int count=0;
	  // Insertion of nodes (cell wall yielding)
	  if (!node.fixed) {
	    if (old_l1 > par.yielding_threshold*Node::target_length && !cit->nb1->fixed) {
	      node_insertion_queue.push( Edge(cit->nb1, &node) );
	    }
	    if (old_l2 > par.yielding_threshold*Node::target_length && !cit->nb2->fixed) {
	      node_insertion_queue.push( Edge(&node, cit->nb2 ) );
	    }
	    count++;

	  }


    // We must double the weights for the perimeter (otherwise they start bulging...)
    double w1, w2;
#ifdef FLEMING
  	if (node.boundary && cit->nb1->boundary)
  	  w1 = par.rel_perimeter_stiffness;
  	else
  	  w1 = 1;
  	if (node.boundary && cit->nb2->boundary)
  	  w2 = par.rel_perimeter_stiffness;
  	else
  	  w2 = 1;

#else
  	if (node.boundary && cit->nb1->boundary)
  		w1=2;
  	else
  		w1 = 1;
  	if (node.boundary && cit->nb2->boundary)
  		w2 = 2;
  	else
  		w2 = 1;
#endif


    // Cell specific wall stiffness
    double cell_w = cit->cell->GetWallStiffness();

    double w_w1 = 1;
    double w_w2 = 1;
    int w_count1 = 0;
    int w_count2 = 0;
    double bl_minus_1 = 0.0;
    double bl_plus_1 = 0.0;

    calculateWallStiffness(&c, *i, &w_count1,&w_count2,&w_w1,&w_w2, &bl_minus_1, &bl_plus_1);



    if (!std::isnan(w_w1)&&!std::isnan(w_w2)&& w_count1>0&& w_count2>0) {
        w1 = cell_w * w_w1;
        w2 = cell_w * w_w2;
    }


     //check if wall elements are defined and pick the appropriate length_dh
      if(bl_minus_1>0 && bl_plus_1>0){
        double elastic_modulus = 10;
        length_dh = elastic_modulus * w1 * bl_minus_1 *(DSQR(new_l1/bl_minus_1 - 1)-DSQR(old_l1/bl_minus_1 - 1))+
                elastic_modulus * w2 * bl_plus_1 *(DSQR(new_l2/bl_plus_1 - 1)-DSQR(old_l2/bl_plus_1 - 1));
    } else {
      length_dh +=2*Node::target_length * ( w1*(old_l1 - new_l1) +
                                            w2*(old_l2 - new_l2) ) +
                           w1*(DSQR(new_l1)
                           - DSQR(old_l1))
                           + w2*(DSQR(new_l2)
                             - DSQR(old_l2));



        }
/*
    length_dh +=2*Node::target_length * ( w1*(old_l1 - new_l1) +
                                          w2*(old_l2 - new_l2) ) +
                         w1*(DSQR(new_l1)
                         - DSQR(old_l1))
                         + w2*(DSQR(new_l2)
                           - DSQR(old_l2));
    cout << node << "\t" << bl_minus_1 <<  "\t" << bl_plus_1 <<  "\t" << w_w1 <<  "\t" << w_w2 << "\n";

    */
	}

	// bending energy also holds for outer boundary
	// first implementation. Can probably be done more efficiently
	// calculate circumcenter radius (gives local curvature)
	// the ideal bending state is flat... (K=0)
		  {
	  // strong bending energy to resist "cleaving" by division planes
	  double r1, r2, xc, yc;
	  CircumCircle(i_min_1.x, i_min_1.y, old_p.x, old_p.y, i_plus_1.x, i_plus_1.y,
		       &xc,&yc,&r1);
	  CircumCircle(i_min_1.x, i_min_1.y, new_p.x, new_p.y, i_plus_1.x, i_plus_1.y,
		       &xc,&yc, &r2);

	  if (r1<0 || r2<0) {
	    MyWarning::warning("r1 = %f, r2 = %f",r1,r2);
	  }
	  bending_dh += DSQR(1/r2 - 1/r1);

	}
      }
      dh = 	area_dh + cell_length_dh +
	par.lambda_length * length_dh + par.bend_lambda * bending_dh + par.alignment_lambda * alignment_dh;

         //(length_constraint_after - length_constraint_before);

      if (node.fixed) {

	// search the fixed cell to which this node belongs
	// and displace these cells as a whole
	// WARNING: undefined things will happen for connected fixed cells...
	for (list<Neighbor>::iterator c=node.owners.begin(); c!=node.owners.end(); c++) {
	  if (!c->cell->BoundaryPolP() && c->cell->FixedP()) {
	    sum_dh+=c->cell->Displace(rx,ry,0);
	  }
	}
      } else {


	if (dh<-sum_stiff || RANDOM()<exp((-dh-sum_stiff)/par.T)) {

	  // update areas of cells
	  list<DeltaIntgrl>::const_iterator di_it = delta_intgrl_list.begin();
	  for (list<Neighbor>::iterator cit=node.owners.begin(); cit!=node.owners.end(); ( cit++) ) {
	    if (!cit->cell->BoundaryPolP()) {
	      cit->cell->area -= di_it->area;
	      if (par.lambda_celllength) {
		cit->cell->intgrl_x -= di_it->ix;
		cit->cell->intgrl_y -= di_it->iy;
		cit->cell->intgrl_xx -= di_it->ixx;
		cit->cell->intgrl_xy -= di_it->ixy;
		cit->cell->intgrl_yy -= di_it->iyy;
	      }
	      di_it++;
	    }
	  }

//	  double old_nodex, old_nodey;

    //  old_nodex=node.x;
     // old_nodey=node.y;

	  node.x = new_p.x;
	  node.y = new_p.y;

	  for (list<Neighbor>::iterator cit=node.owners.begin();
	       cit!=node.owners.end();
	       ( cit++) ) {

	    /*   if (cit->cell >= 0 && cells[cit->cell].SelfIntersect()) {
		 node.x = old_nodex;		       
		 node.y = old_nodey;
		 goto next_node;
		 }*/


	  }

      // check lengths of wall elements and apply plastic deformation
      for (list<Neighbor>::iterator c=node.owners.begin(); c!=node.owners.end(); c++) {
          Vector *point = &node;
          c->cell->LoopWallElements([point](auto wallElementInfo){
            if(wallElementInfo->isFrom(point)){
                if(wallElementInfo->hasWallElement()){
                    if(wallElementInfo->plasticStretch()){
                        wallElementInfo->updateBaseLength();
                    }
                }
            } else if(wallElementInfo->isTo(point)){
                if(wallElementInfo->hasWallElement()){
                    if(wallElementInfo->plasticStretch()){
                        wallElementInfo->updateBaseLength();
                    }
                }
            }
          }
      );
      }

	  sum_dh += dh;
	}  
      }
    } 
  next_node:
    delta_intgrl_list.clear();//dA_list.clear();

  }

  return sum_dh;
}

void extractWallData(WallElementInfo* wallElementInfo, int* count,double *w,double* bl){
	double stiffness=.0;
	double base_length=.0;
	if (wallElementInfo->hasWallElement()) {
		stiffness = wallElementInfo->getWallElement()->getStiffness();
		base_length = wallElementInfo->getBaseLength();
	} else {
		stiffness = wallElementInfo->getCell()->GetWallStiffness();
	}
	if (!std::isnan(stiffness)){
		(*w) += stiffness;
		(*count)++;
		(*bl) += base_length;
	}
}

void Mesh::calculateWallStiffness(CellBase* c, Node* node, int* count_p1,int* count_p2,double *w_p1,double *w_p2, double* bl_minus_1, double* bl_plus_1) {
        c->LoopWallElements([node,count_p1,count_p2,w_p1,w_p2,bl_minus_1,bl_plus_1](auto wallElementInfo){
    		if (wallElementInfo->isTo(node)) {
                extractWallData(wallElementInfo,count_p1,w_p1,bl_plus_1);
                // on the outside there is no counter wall
			} else	if (wallElementInfo->isFrom(node)) {
                extractWallData(wallElementInfo,count_p2,w_p2,bl_minus_1);
				// on the outer wall we need to include this wallelement as we wont see it from the other side
				// we are past the node so we can end the LoopWallElements
			}
        });
    }


void splitWallElements(WallElementInfo *base,Node* new_node) {
	WallElement * newWallElement = new_node->insertWallElement(base->getCell());
	WallElementInfo sub;
	base->getCell()->fillWallElementInfo(&sub, new_node, (Node*)base->getTo());
	base->divide(&sub);
}

void Mesh::InsertNode(Edge &e) {


  // Construct a new node in the middle of the edge
  Node *new_node = AddNode( new Node ( ( *e.first + *e.second )/2 ) );

  // if new node is inserted into the boundary
  // it will be part of the boundary, fixed, and source, too

  // The new node is part of the boundary only if both its neighbors are boundary nodes and the boundray proceeds from first to second.
  new_node->boundary = (e.first->BoundaryP() && e.first->BoundaryP()) && ((findNextBoundaryNode(e.first))->Index() == e.second->Index());
  new_node->fixed = e.first->fixed && e.second->fixed;
  new_node->sam = new_node->boundary && (e.first->sam || e.second->sam);

  // insert it into the boundary polygon;
  /* if (new_node->boundary) {

  // find the position of the first node in the boundary
  list<Node *>::iterator ins_pos = find
  (boundary_polygon->nodes.begin(),
  boundary_polygon->nodes.end(),
  e.first);
  // ... second node comes before or after it ...
  if (*(++ins_pos!=boundary_polygon->nodes.end()?
  ins_pos:boundary_polygon->nodes.begin())!=e.second) {

  boundary_polygon->nodes.insert(((ins_pos--)!=boundary_polygon->nodes.begin()?ins_pos:(--boundary_polygon->nodes.end())), new_node);

  // .. set the neighbors of the new node ...
  // in this case e.second and e.first are inverted
  new_node->owners.push_back( Neighbor(boundary_polygon, e.second, e.first ) );
  //cerr << "pushing back " << Neighbor(boundary_polygon->index, e.second, e.first ) << endl;
  } else {
  // insert before second node, so leave ins_pos as it is,
  // that is incremented
  boundary_polygon->nodes.insert(ins_pos, new_node);

  // .. set the neighbors of the new node ...
  new_node->owners.push_back( Neighbor(boundary_polygon, e.first, e.second ) );
  // cerr << "pushing back " << Neighbor(boundary_polygon->index, e.second, e.first ) << endl;
  }

  }*/


  list<Neighbor> owners;

  // push all cells owning the two nodes of the divided edges
  // onto a list
  copy(e.first->owners.begin(),
       e.first->owners.end(),
       back_inserter(owners));
  copy(e.second->owners.begin(),
       e.second->owners.end(),
       back_inserter(owners));

  //copy(owners.begin(), owners.end(), ostream_iterator<Neighbor>(cerr, " "));
  //cerr << endl;

  // sort the nodes
  owners.sort( mem_fn( &Neighbor::Cmp ) );

  //  extern ofstream debug_stream;

  //  debug_stream << "Nodes " << e.first << " and " << e.second << endl;
  //  copy(owners.begin(), owners.end(), ostream_iterator<Neighbor>(debug_stream, " "));
  //  debug_stream << endl;

  // the duplicates in this list indicate cells owning this edge  
  list<Neighbor>::iterator c=owners.begin();
  while (c!=owners.end()) {
    c=adjacent_find(c,owners.end(),  neighbor_cell_eq);


    if (c!=owners.end()) { // else break;

      //      debug_stream << "Cell " << c->cell << " owns Edge " << e << endl;

 
      // find the position of the edge's first node in cell c...
      list<Node *>::iterator ins_pos = find
	(c->cell->nodes.begin(),
	 c->cell->nodes.end(),
	 e.first);
      // ... second node comes before or after it ...

      // XXXX probably this test is always false XXXX: No, works okay.
      if (*(++ins_pos!=c->cell->nodes.end()?
	    ins_pos:c->cell->nodes.begin())!=e.second) {
	c->cell->nodes.insert(((ins_pos--)!=c->cell->nodes.begin()?ins_pos:(--c->cell->nodes.end())), new_node);
	//cells[c->cell].nodes.insert(--ins_pos, new_node->index);
	// .. set the neighbors of the new node ...
	// in this case e.second and e.first are inverted
	//  cerr << "Inverted\n";
	new_node->owners.push_back( Neighbor(c->cell, e.second, e.first ) );
      } else {
	// insert before second node, so leave ins_pos as it is,
	// that is incremented
	c->cell->nodes.insert(ins_pos, new_node);	
	// .. set the neighbors of the new node ...
	// cerr << "Not inverted\n";
	new_node->owners.push_back( Neighbor(c->cell, e.first, e.second ) );
      }

      // redo the neighbors:
      //}


      // - find cell c among owners of Node e.first
      list<Neighbor>::iterator cpos=
	find_if( e.first->owners.begin(),
		 e.first->owners.end(),
		 [c](auto neighbor){return neighbor.CellEquals(c->cell->Index());});

      // - correct the record
      if (cpos->nb1 == e.second) {
	cpos->nb1 = new_node;
      } else 
	if (cpos->nb2 == e.second) {
	  cpos->nb2 = new_node;
	}

      // - same for Node e.second
      cpos=
	find_if( e.second->owners.begin(),
		 e.second->owners.end(),
		 [c](auto neighbor){return neighbor.CellEquals(c->cell->Index());});

      // - correct the record
      if (cpos->nb1 == e.first) {
	cpos->nb1 = new_node;
      } else 
	if (cpos->nb2 == e.first) {
	  cpos->nb2 = new_node;
	}


    } else break;
    c++; 
  }

  // Repair neighborhood lists in a second loop, to make sure all
  // `administration' is up to date
  while (c!=owners.end()) {
    c=adjacent_find(c,owners.end(),  neighbor_cell_eq);
    // repair neighborhood lists of cell and Wall lists

    if (!c->cell->BoundaryPolP()) {
      c->cell->ConstructNeighborList();
    
    }
    c++;
  }


  for (list<Neighbor>::const_iterator owner=new_node->owners.begin(); owner!=new_node->owners.end(); owner++) {
	  if (e.first->getWallElement(owner->cell)!= NULL){
		  WallElementInfo info;
		  owner->cell->fillWallElementInfo(&info, e.first, e.second);
		  if (info.hasWallElement()) {
			  splitWallElements(&info,new_node);
		  }
	  }if(e.second->getWallElement(owner->cell)!= NULL){
		  WallElementInfo info;
		  owner->cell->fillWallElementInfo(&info, e.second, e.first);
		  if (info.hasWallElement()) {
			  splitWallElements(&info,new_node);
		  }
	  }
  }
}


/*
  Calculate circumcircle of triangle (x1,y1), (x2,y2), (x3,y3)
  The circumcircle centre is returned in (xc,yc) and the radius in r
  NOTE: A point on the edge is inside the circumcircle
*/
void Mesh::CircumCircle(double x1,double y1,double x2,double y2,double x3,double y3,
			double *xc,double *yc,double *r)
{
  double m1,m2,mx1,mx2,my1,my2;
  double dx,dy,rsqr;

  /* Check for coincident points */
  /*if (abs(y1-y2) < TINY && abs(y2-y3) < TINY)
    return(false);*/

  if (abs(y2-y1) < TINY) {
    m2 = - (x3-x2) / (y3-y2);
    mx2 = (x2 + x3) / 2.0;
    my2 = (y2 + y3) / 2.0;
    *xc = (x2 + x1) / 2.0;
    *yc = m2 * (*xc - mx2) + my2;
  } else if (abs(y3-y2) < TINY) {
    m1 = - (x2-x1) / (y2-y1);
    mx1 = (x1 + x2) / 2.0;
    my1 = (y1 + y2) / 2.0;
    *xc = (x3 + x2) / 2.0;
    *yc = m1 * (*xc - mx1) + my1;
  } else {
    m1 = - (x2-x1) / (y2-y1);
    m2 = - (x3-x2) / (y3-y2);
    mx1 = (x1 + x2) / 2.0;
    mx2 = (x2 + x3) / 2.0;
    my1 = (y1 + y2) / 2.0;
    my2 = (y2 + y3) / 2.0;
    *xc = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
    *yc = m1 * (*xc - mx1) + my1;
  }

  dx = x2 - *xc;
  dy = y2 - *yc;
  rsqr = dx*dx + dy*dy;
  *r = sqrt(rsqr);

  return;
  // Suggested
  // return((drsqr <= rsqr + EPSILON) ? TRUE : FALSE);
}

//

// return the total amount of chemical "ch" in the leaf
double Mesh::SumChemical(int ch) {

  double sum=0.;
  for (vector<Cell *>::iterator i=cells.begin(); i!=cells.end(); i++) {
    sum+=(*i)->chem[ch];
  }
  return sum;
}



void Mesh::CleanUpCellNodeLists(void) {

  typedef vector <vector<Cell *>::iterator> CellItVect;

  CellItVect cellstoberemoved;
  vector<int> cellind;

  // Start of by removing all stale walls.
  //DeleteLooseWalls();
  // collect all dead cells that need to be removed from the simulation
  for (vector<Cell *>::iterator i=cells.begin(); i!=cells.end(); i++) {
    if ((*i)->DeadP()) {
      // collect the iterators
      cellstoberemoved.push_back(i);

      // collect the indices
      cellind.push_back((*i)->index);
    } else {
      // Remove pointers to dead Walls
      for (list<Wall *>::iterator w=(*i)->walls.begin(); w!=(*i)->walls.end(); w++) {
	if ((*w)->DeadP()) {
	  (*w)=0;
	}
      }
      (*i)->walls.remove(0);
    }
  }

  // Remove pointers to dead Walls from BoundaryPolygon
  for (list<Wall *>::iterator w=boundary_polygon->walls.begin(); w!=boundary_polygon->walls.end(); w++) {
    if ((*w)->DeadP()) {
      (*w)=0;
    }
  }
  boundary_polygon->walls.remove(0);


  // Renumber cells; this is most efficient if the list of dead cell indices is sorted
  sort(cellind.begin(),cellind.end());


  // Reindexing of Cells
  for (vector<int>::reverse_iterator j=cellind.rbegin(); j!=cellind.rend(); j++) {
    for (vector<Cell *>::reverse_iterator i=cells.rbegin(); i!=cells.rend(); i++) {
      if (*j < (*i)->index) (*i)->index--;
    }
  }


  // Actual deleting of Cells
  // We must delete in reverse order, otherwise the iterators become redefined
  for ( CellItVect::reverse_iterator i=cellstoberemoved.rbegin(); i!=cellstoberemoved.rend(); i++) {
    Cell::NCells()--;
    cells.erase(*i);
  }


  // same for nodes
  typedef vector <vector<Node *>::iterator> NodeItVect;

  NodeItVect nodestoberemoved;
  vector<int> nodeindlist;

  // collect iterators and indices of dead nodes
  for (vector<Node *>::iterator i=nodes.begin(); i!=nodes.end(); i++) {
    if ((*i)->DeadP()) {
      nodestoberemoved.push_back( i );
      nodeindlist.push_back((*i)->index);
    }
  }

  // sort the list of dead nodes for renumbering
  sort(nodeindlist.begin(),nodeindlist.end());


  // Reindicing of Nodes
  for (vector<int>::reverse_iterator j=nodeindlist.rbegin(); j!=nodeindlist.rend(); j++) {
    for (vector<Node *>::reverse_iterator i=nodes.rbegin(); i!=nodes.rend(); i++) {
      if (*j < (*i)->index) { 
	(*i)->index--;
      } 
    }
  }

  // Actual deleting of nodes
  // We must delete in reverse order, otherwise the iterators become redefined
  for ( NodeItVect::reverse_iterator i=nodestoberemoved.rbegin(); i!=nodestoberemoved.rend(); i++) {
    Node::nnodes--;
    nodes.erase(*i);
  }



  for (list<Wall *>::iterator w=walls.begin(); w!=walls.end(); w++) {
    if ((*w)->DeadP()) {
      Wall::nwalls--;
      delete *w;
      *w = 0;
    }
  }

  walls.remove( 0 );



  // Clean up all intercellular connections and redo everything
  for (vector<Node *>::iterator i=nodes.begin(); i!=nodes.end(); i++) {
    (*i)->owners.clear();
  }

  for (vector<Cell *>::iterator i=cells.begin(); i!=cells.end(); i++) {
    (*i)->ConstructConnections();
  }

  boundary_polygon->ConstructConnections();

  // remake shuffled_nodes and shuffled cells
  shuffled_nodes.clear();
  shuffled_nodes = nodes;

  shuffled_cells.clear();
  shuffled_cells = cells;
}

void Mesh::CutAwayBelowLine( Vector startpoint, Vector endpoint) {

  // Kills all cells below the line startpoint -> endpoint

  Vector perp = (endpoint-startpoint).Perp2D().Normalised();

#ifdef QDEBUG
  qDebug() << "Before Apoptose" << endl;
#endif

  TestIllegalWalls();
  for (vector<Cell *>::iterator i=cells.begin(); i!=cells.end(); i++) {

    // do some vector geometry to check whether the cell is below the cutting line
    Vector cellvec = ((*i)->Centroid()-startpoint);

    if ( InnerProduct(perp, cellvec) < 0 ) {
      // remove those cells
      (*i)->Apoptose();
    }
  }

#ifdef QDEBUG
  qDebug() << "Before CleanUpCellNodeLists" << endl;
#endif
  TestIllegalWalls();

  CleanUpCellNodeLists();
}

void Mesh::CutAwaySAM(void) {

  for (vector<Cell *>::iterator i=cells.begin(); i!=cells.end(); i++) {
    if( (*i)->Boundary() == Cell::SAM ) {
      (*i)->Apoptose();
    }
  }
  TestIllegalWalls();
  CleanUpCellNodeLists();
}

void Mesh::TestIllegalWalls(void) {

  for (list<Wall *>::iterator w = walls.begin(); w!=walls.end(); w++) {
    if ((*w)->IllegalP() ) {
#ifdef QDEBUG
      cerr << "Wall " << **w << " is illegal." << endl;
#endif
    }
  }
}



class node_owners_eq : public unary_function<Node, bool> {
  int no;
public:

  explicit node_owners_eq(int nn) { no=nn; }

  bool operator() (const Node &n) const {
    if (n.CellsSize()==1) 
      return true;
    else 
      return false;
  }
};


void Mesh::RepairBoundaryPolygon(void) {

  // After serious manipulations (e.g. after cutting part off the
  // leaf) repair the boundary polygon. It assumes the cut line has
  // already been marked "boundary" and the original boundary marks
  // were not removed. 
  //
  // So, this function just puts boundary nodes into the boundary
  // polygon in the right order; it cannot detect boundaries from
  // scratch.

  Node *boundary_node=0, *next_boundary_node=0, *internal_node;
  set<int> original_boundary_nodes, repaired_boundary_nodes;
  vector<int> difference; // set difference result

  // Step 0: print out current boundary polygon
#ifdef QDEBUG
  qDebug() << endl << "Original Boundary Polygon node indices: ";
  foreach (Node* node, boundary_polygon->nodes) {
    qDebug() << node->Index() << " " ;
  }    
  qDebug() << endl << endl;
#endif

  // Step 1a: Create a set containing the current boundary polygon nodes' Indices.
  for (Node * node : boundary_polygon->nodes) {
    original_boundary_nodes.insert(node->Index());
  }

  // Step 1b: remove all nodes from boundary polygon
  boundary_polygon->nodes.clear();

  // Step 2: Remove all references to the boundary polygon from the Mesh's current list of nodes
  for (Node* node : nodes) {
    node->Unmark(); // remove marks, we need them to determine if we have closed the circle
    list<Neighbor>::iterator boundary_ref_pos;
    if ((boundary_ref_pos = find_if (node->owners.begin(), node->owners.end(), 
				     [](auto neighbor){return neighbor.CellEquals(-1);})) != node->owners.end()) {
      // i.e. if one of the node's owners is the boundary polygon 
      node->owners.erase(boundary_ref_pos); // remove the reference
    }
  }

  // Step 3: Search for the first boundary node.  We reconstruct the
  // boundary polygon by moving along the boundary nodes until we've
  // encircled the polygon. Since manually adding nodes may have
  // turned nodes previously along the boundary into internal nodes,
  // we search through all the node until we find first boundary node
  // and proceed from there. If findNextBoundaryNode() returns a node
  // other than the one passed to it, the original node is the first
  // boundary node.
  for (Node* node : nodes) {
    if ((findNextBoundaryNode(node))->index != node->index){
      next_boundary_node = node;
      break;
    }
  }

  // We have a problem if we arrive here without having found a boundary node.
  if (!next_boundary_node) throw("Cannot find a boundary node!.");

  // Reconstruct the list of boundary polygon nodes.
  do {
    boundary_node = next_boundary_node;
    boundary_node->Mark();
    boundary_polygon->nodes.push_back(boundary_node);
    next_boundary_node = findNextBoundaryNode(boundary_node);
  } while ( !next_boundary_node->Marked() );


  // Create a set containing the reconstructed boundary polygon nodes' Indices.
  for (list<Node *>::iterator it = boundary_polygon->nodes.begin(); it!=boundary_polygon->nodes.end(); ++it) {
    repaired_boundary_nodes.insert((*it)->Index());
  }

  // Calculate the difference between the original and repaired sets of boundary nodes
  // yielding the set of nodes that are no longer part of the boundary polygon.
  set_difference(original_boundary_nodes.begin(), original_boundary_nodes.end(),
                 repaired_boundary_nodes.begin(), repaired_boundary_nodes.end(), back_inserter(difference));

  // Tell each node in the difference that it's no longer part of the boundary polygon
  vector<Node *>::iterator internal_node_it;
  for (int i : difference){
    internal_node_it = find_if (nodes.begin(), nodes.end(), [i](auto node){return node->IndexEquals(i);});
    internal_node = *internal_node_it; // dereference the itterator to get to the node pointer
    if (!internal_node) throw("Found a null Node pointer.");
    internal_node->UnsetBoundary();
  }

  boundary_polygon->ConstructConnections();
  for (list<Wall *>::iterator w=boundary_polygon->walls.begin(); w!=boundary_polygon->walls.end(); w++) {
    if ((*w)->DeadP()) {
      (*w)=0;
    }
  }
  boundary_polygon->walls.remove(0);
  boundary_polygon->ConstructNeighborList();

#ifdef QDEBUG
  qDebug() << "Repaired Boundary Polygon node indices: ";
  foreach (Node* node, boundary_polygon->nodes){
    qDebug() << node->Index() << " " ;
  }
  qDebug() << endl ;

#ifdef _undefined_
  qDebug() << "NODES:" << endl;
  foreach(Node* node, nodes) {
    qDebug() << *node;
  }
  qDebug() << endl;

  qDebug() << "WALLS:" << endl;
  foreach(Wall* wall, walls) {
    qDebug() << *wall;
  }
  qDebug() << endl;

  qDebug() << "CELLS:" << endl;
  foreach(Cell* cell, cells) {
    qDebug() << *cell;
  }
  qDebug() << endl;
#endif
#endif
}


Node* Mesh::findNextBoundaryNode(Node* boundary_node) {
  //bool found_next_boundary_node = false;
  Node *next_boundary_node = 0;
  set<int> boundary_node_owners; // This is a list of the current boundary node's owners' Ids
  vector<int> neighborIds; // A list of the current boundary node's owners' 2nd neighbor Ids
  vector<set<int> *>  nodeOwners; // A vector of set pointers where each set contains the owner Ids of the nodes in the neighborIds list.
  vector<int> intersection; // set intersection result

  // The next boundary node is that which has only one owner in common with the current boundary node
  for (list<Neighbor>::iterator it=boundary_node->owners.begin(); it!=boundary_node->owners.end(); ++it) {
    if (it->cell->Index() != -1) boundary_node_owners.insert(it->cell->Index()); // Save each of the current boundary node's owners' Ids - except the boundary polygon 
    set<int> *owners = new set<int>; // create a set to hold a 2nd neighbor's owners' Ids
    nodeOwners.push_back(owners);
    neighborIds.push_back(it->nb2->Index());
    for (Neighbor neighbor : it->nb2->owners){
      if (neighbor.cell->Index() != -1) owners->insert(neighbor.cell->Index()); // Save second neighbors' owners' Ids - except the boundary polygon 
    }
  }
  vector<int>::iterator itt = neighborIds.begin();
  vector<set<int> *>::iterator it = nodeOwners.begin();

#ifdef QDEBUG  
  qDebug() << "Boundary node: " <<  boundary_node->Index() << " is owned by the following cells: ";
  foreach (int i, boundary_node_owners){
    qDebug() << i << "  ";
  }
  qDebug() << endl;
#endif

  for (; it < nodeOwners.end(); it++, itt++) {
    intersection.clear();
    set_intersection(boundary_node_owners.begin(), boundary_node_owners.end(), (*it)->begin(), (*it)->end(), back_inserter(intersection));

#ifdef QDEBUG  
    qDebug() << "The intersection of the boundary node(" << boundary_node->Index() << ") owners and its 2nd neighbor(" <<  *itt << ") owners is: ";
    foreach (int i, intersection){
      qDebug() << i << "  ";
    }
    qDebug() << endl;
#endif

    if (intersection.size() == 1){
      //found_next_boundary_node = true;
      vector<Node *>::iterator next_boundary_node_it = find_if (nodes.begin(), nodes.end(), [itt](auto node){return node->IndexEquals(*itt);});
      next_boundary_node = *next_boundary_node_it; // defeference the itterator to get to the node pointer

#ifdef QDEBUG  
      qDebug() << "The Current boundary node is: " << boundary_node->Index()
	       << ". The Next boundary node is: " << *itt << ((next_boundary_node->Marked()) ? " Marked" : " Unmarked") << endl << endl;
#endif

      break;
    }
  }

#ifdef QDEBUG  
  if (!found_next_boundary_node) {
    qDebug() << "OOPS! Didn't find the next boundrary node!" << endl;
  }
#endif

  return next_boundary_node;
}


void Mesh::CleanUpWalls(void) {
  for (list<Wall *>::iterator w=walls.begin(); w!=walls.end(); w++) {
    if ((*w)->DeadP()) {
      delete *w;
      (*w)=0;      
    }
  }
  walls.remove(0);
}

void Mesh::Rotate(double angle, Vector center) {

  // Rotate the mesh over the angle "angle", relative to center point "center".

  Matrix rotmat;

  rotmat.Rot2D(angle);

  for (vector<Node *>::iterator n=nodes.begin(); n!=nodes.end(); n++) {
    (*n)->setPos ( rotmat * ( *(*n) - center ) + center );  
  }
}


void Mesh::PrintWallList( void ) {

  transform ( walls.begin(), walls.end(), ostream_iterator<Wall>(cerr, "\n"), deref_ptr<Wall> );
}

#include <QString>
//#include "forwardeuler.h"
#include "rungekutta.h"

//class SolveMesh : public ForwardEuler{
class SolveMesh : public RungeKutta{
private:
  SolveMesh(void);

public:
  SolveMesh(Mesh *m_) {

    m = m_;

    kmax=0;
    kount=0;
    xp=0; yp=0; dxsav=0;


  }

protected:
  virtual void derivs(double x, double *y, double *dydx) {

    // set mesh with new values given by ODESolver
    // (we must do this, because only mesh knows the connections
    // between the variables)

    m->setValues(x,y);
    m->Derivatives(dydx);

    //cerr << "Calculated derivatives at " << x << "\n";    
  }

private:
  Mesh *m;
  int kmax,kount;
  double *xp,**yp,dxsav;
  bool monitor_window;
};



void Mesh::ReactDiffuse(double delta_t) {

  // Set Lengths of Walls
  for_each ( walls.begin(), walls.end(), 
         mem_fn( &Wall::SetLength ) );

  static SolveMesh *solver = new SolveMesh(this);

  int nok, nbad, nvar;
  double *ystart = getValues(&nvar);

  solver->odeint(ystart, nvar, getTime(), getTime() + delta_t, 
		 par.ode_accuracy, par.dt, 1e-10, &nok, &nbad);

  setTime(getTime()+delta_t);
  setValues(getTime(),ystart);
}


Vector Mesh::FirstConcMoment(int chem) {

  Vector moment;
  for (vector<Cell *>::const_iterator c=cells.begin(); c!=cells.end(); c++) {
    moment += (*c)->Chemical(chem) * (*c)->Centroid();
  }
  return moment / (double)cells.size();
}

/*! This member function deletes all walls connected to two dead cells from the mesh.
  It should be called before the Cells are actually removed.
  If the cell is connect to one dead cell only, that reference is substituted for a reference 
  to the boundary polygon.
*/
void Mesh::DeleteLooseWalls(void) {

  list<Wall *>::iterator w=walls.begin();

  while (w!=walls.end()) {

    // if both cells of the wall are dead, remove the wall
    if ((*w)->C1()->DeadP() || (*w)->C2()->DeadP()) {
      if ((*w)->C1()->DeadP() && (*w)->C2()->DeadP()) {
	delete *w;
	w=walls.erase(w);
      } else {
	if ((*w)->C1()->DeadP())
	  (*w)->c1 = boundary_polygon;
	else
	  (*w)->c2 = boundary_polygon;
	w++;
      }
    } else {
      w++;
    }

  }
}

/*void Mesh::FitLeafToCanvas(double width, double height) {

  Vector bbll,bbur;
  BoundingBox(bbll,bbur);

  double scale_x = width/(bbur.x-bbll.x);
  double scale_y = height/(bbur.y-bbll.y);

  double factor = scale_x<scale_y ? scale_x:scale_y;

  Cell::SetMagnification(factor); // smallest of scale_x and scale_y

  double offset_x = (width/Cell::Magnification()-(bbur.x-bbll.x))/2.;  
  double offset_y = (height/Cell::Magnification()-(bbur.y-bbll.y))/2.;

  Cell::setOffset(offset_x, offset_y);

  }*/



void Mesh::CleanChemicals(const vector<double> &clean_chem) {

  if (clean_chem.size()!=(unsigned)Cell::NChem()) {
    throw "Run time error in Mesh::CleanChemicals: size of clean_chem should be equal to Cell::NChem()";
  }
  for (vector<Cell *>::iterator c=cells.begin(); c!=cells.end(); c++) {
    for (int i=0;i<Cell::NChem();i++) {
      (*c)->SetChemical(i,clean_chem[i]);
    }
    (*c)->SetNewChemToChem();
  }
}


void Mesh::CleanTransporters(const vector<double> &clean_transporters) {

  if (clean_transporters.size()!=(unsigned)Cell::NChem()) {
    throw "Run time error in Mesh::CleanTransporters: size ofclean_transporters should be equal to Cell::NChem()";
  }


  // clean transporters
  for (list<Wall *>::iterator w=walls.begin(); w!=walls.end(); w++) {
    for (int i=0;i<Cell::NChem();i++) {
      (*w)->setTransporters1(i,clean_transporters[i]); (*w)->setNewTransporters1(i,clean_transporters[i]);
      (*w)->setTransporters2(i,clean_transporters[i]); (*w)->setNewTransporters2(i,clean_transporters[i]);
    }
  }
}


void Mesh::RandomizeChemicals(const vector<double> &max_chem, const vector<double> &max_transporters) {

  if (max_chem.size()!=(unsigned)Cell::NChem() || max_transporters.size()!=(unsigned)Cell::NChem()) {
    throw "Run time error in Mesh::CleanChemicals: size of max_chem and max_transporters should be equal to Cell::NChem()";
  }

  for (vector<Cell *>::iterator c=cells.begin(); c!=cells.end(); c++) {
    for (int i=0;i<Cell::NChem();i++) {
      (*c)->SetChemical(i,max_chem[i]*RANDOM());
     //(*c)->SetChemical(i,RANDOM()*max_chem[i]*0.1+0.9*max_chem[i]);
        //(*c)->SetChemical(i,RANDOM()*max_chem[i]*(par -> range)+(1 - par -> range)*max_chem[i]); // must be a number between 0 and 0.99
    }
    (*c)->SetNewChemToChem();
  }

  // randomize transporters
  for (list<Wall *>::iterator w=walls.begin(); w!=walls.end(); w++) {
    for (int i=0;i<Cell::NChem();i++) {
      (*w)->setTransporters1(i,max_transporters[i] * RANDOM()); (*w)->setNewTransporters1(i, (*w)->Transporters1(i) );
      (*w)->setTransporters2(i,max_transporters[i] * RANDOM()); (*w)->setNewTransporters2(i, (*w)->Transporters1(i) );
    }
  }
}

//!\brief Calculates a vector with derivatives of all variables, which
// we can pass to an ODESolver. 
void Mesh::Derivatives(double *derivs) {

  int nwalls = walls.size();
  int ncells = cells.size();
  int nchems = Cell::NChem();

  // two eqs per chemical for each walls, and one eq per chemical for each cell
  // This is for generality. For a specific model you may optimize
  // this by removing superfluous (empty) equations.
  int neqs = 2 * nwalls * nchems + ncells * nchems;

  //static double *derivs = 0; 
  // derivs is allocated by RungeKutta class.

  for (int i=0;i<neqs;i++) {
    derivs[i]=0.;
  }

  // Layout of derivatives: cells [ chem1 ... chem n]  walls [ [ w1(chem 1) ... w1(chem n) ] [ w2(chem 1) ... w2(chem n) ] ]

  int i=0;

  for (vector<Cell *>::iterator c=cells.begin(); c!=cells.end(); c++) {
    plugin->CellDynamics(*c, &(derivs[i]));
    i+=nchems;
  }

  for (list<Wall *>::iterator w=walls.begin(); w!=walls.end(); w++) {
    // (*wr)(*w, &(derivs[i]), &(derivs[i+nchems]));
    plugin->WallDynamics(*w,  &(derivs[i]), &(derivs[i+nchems]));
    // Transport function adds to derivatives of cell chemicals
    double *dchem_c1 = &(derivs[(*w)->c1->Index() * nchems]);
    double *dchem_c2 = &(derivs[(*w)->c2->Index() * nchems]);
    //plugin->CelltoCellTransport(*w, &(derivs[(*w)->c1->Index() * nchems]),
    //	  &(derivs[(*w)->c2->Index() * nchems]));
    // quick fix: dummy values to prevent end user from writing into outer space and causing a crash :-)
    // start here if you want to implement chemical input/output into environment over boundaries
    double dummy1, dummy2;
    if ((*w)->c1->Index()<0) { // tests if c1 is the boundary pol
      dchem_c1 = &dummy1;
    }
    if ((*w)->c2->Index()<0) {
      dchem_c2 = &dummy2;
    }
    plugin->CelltoCellTransport(*w, dchem_c1, dchem_c2); 

    //(*tf)(*w, &(derivs[(*w)->c1->Index() * nchems]),
    //&(derivs[(*w)->c2->Index() * nchems] ) );
    i+=2*nchems;
  }
}

void Mesh::setValues(double x, double *y) {

  //int nwalls = walls.size();
  //int ncells = cells.size();
  int nchems = Cell::NChem();

  // two eqs per chemical for each walls, and one eq per chemical for each cell
  // This is for generality. For a specific model you may optimize
  // this by removing superfluous (empty) equations.
  //int neqs = 2 * nwalls * nchems + ncells * nchems;

  // Layout of derivatives: cells [ chem1 ... chem n]  walls [ [ w1(chem 1) ... w1(chem n) ] [ w2(chem 1) ... w2(chem n) ] ]

  int i=0;
  static int emit_count=0;
  const int stride = 100;
  for (vector<Cell *>::iterator c=cells.begin(); c!=cells.end(); c++) {
    for (int ch=0;ch<nchems;ch++) {
      (*c)->SetChemical(ch, y[i+ch]);
    }
    if ( !(emit_count%stride)) {
      (*c)->EmitValues(x);
    }
    i+=nchems;
  }

  for (list<Wall *>::iterator w=walls.begin(); w!=walls.end(); w++) {
    for (int ch=0;ch<nchems;ch++) {
      (*w)->setTransporters1(ch,y[i+ch]);
    }
    i+=nchems;

    for (int ch=0;ch<nchems;ch++) {
      (*w)->setTransporters2(ch,y[i+ch]);
    }
    i+=nchems;
  }
  emit_count++;
}

double *Mesh::getValues(int *neqs) {

  int nwalls = walls.size();
  int ncells = cells.size();
  int nchems = Cell::NChem();

  // two eqs per chemical for each wall, and one eq per chemical for each cell
  // This is for generality. For a specific model you may optimize
  // this by removing superfluous (empty) equations.
  (*neqs) = 2 * nwalls * nchems + ncells * nchems;

  // Layout of derivatives: cells [ chem1 ... chem n]  walls [ [ w1(chem 1) ... w1(chem n) ] [ w2(chem 1) ... w2(chem n) ] ]

  static double *values = 0;
  if (values!=0) { delete[] values; }

  values = new double[*neqs];

  int i=0;
  for (vector<Cell *>::iterator c=cells.begin(); c!=cells.end(); c++) {
    for (int ch=0;ch<nchems;ch++) {
      values[i+ch]=(*c)->Chemical(ch);
    }
    i+=nchems;
  }

  for (list<Wall *>::iterator w=walls.begin(); w!=walls.end(); w++) {
    for (int ch=0;ch<nchems;ch++) {
      values[i+ch]=(*w)->Transporters1(ch);
    }
    i+=nchems;

    for (int ch=0;ch<nchems;ch++) {
      values[i+ch]=(*w)->Transporters2(ch);
    }
    i+=nchems;
  }
  return values;
}

void Mesh::DrawNodes(QGraphicsScene *c) const {

  for (vector<Node *>::const_iterator n=nodes.begin(); n!=nodes.end(); n++) {
    Node *i=*n;

    NodeItem *item = new NodeItem ( &(*i), c );
    item->setColor();

    item->setZValue(5);
    item->show();
    item ->setPos(((Cell::offset[0]+i->x)*Cell::factor),
          ((Cell::offset[1]+i->y)*Cell::factor) );
  }
}

/*! Returns the sum of protein "ch" of a cycling protein in cells and walls */
double Mesh::CalcProtCellsWalls(int ch) const {


  double sum_prot=0.;

  // At membranes
  for (list<Wall *>::const_iterator w=walls.begin(); w!=walls.end(); w++) {
    sum_prot += (*w)->Transporters1(ch);
    sum_prot += (*w)->Transporters2(ch);
  }

  // At cells
  for (vector<Cell *>::const_iterator c=cells.begin(); c!=cells.end(); c++) {
    sum_prot += (*c)->Chemical(ch);
  }
  return sum_prot;
}

void Mesh::SettoInitVals(void) {

  vector<double> clean_chem(Cell::NChem());
  vector<double> clean_transporters(Cell::NChem());

  for (int i=0;i<Cell::NChem();i++) {
    clean_transporters[i]=0.;
    clean_chem[i]=par.initval[i];
  }

  CleanChemicals(clean_chem);
  CleanTransporters(clean_transporters);
}

string Mesh::getTimeHours(void) const {
  int hours = (int)(time / 3600);
  int mins = (int)((time - hours * 3600)/60);
  int secs = (int)((time - hours * 3600 - mins * 60));
  ostringstream tstr;
  tstr << hours << " h " << mins << " m " << secs << " s";
  return tstr.str();
}

QVector<qreal> Mesh::VertexAngles(void) {
  QVector<qreal> angles;
  for (vector<Node *>::const_iterator n=nodes.begin(); n!=nodes.end(); n++) {
    if ((*n)->Value()>2 && !(*n)->BoundaryP() ) {
      angles+=(*n)->NeighbourAngles();
    }
  }
  return angles;
}

QVector< QPair<qreal,int> > Mesh::VertexAnglesValues(void) {

  QVector< QPair<qreal,int> > anglesvalues;
  for (vector<Node *>::const_iterator n=nodes.begin(); n!=nodes.end(); n++) {
    if ((*n)->Value()>2 && !(*n)->BoundaryP() ) {
      QVector<qreal> angles = (*n)->NeighbourAngles();
      int value_vertex = angles.size();
      for (QVector<qreal>::ConstIterator i=angles.begin(); i!=angles.end(); i++) {
	anglesvalues += QPair< qreal, int > (*i, value_vertex);
      }
    }
  }
  return anglesvalues;
}

void Mesh::Clean(void) {
#ifdef QDEBUG
  qDebug() << "Freeing nodes" << endl;
#endif
  for (vector<Node *>::iterator i=nodes.begin(); i!=nodes.end(); i++) {
    delete *i;
  }
  nodes.clear();
  Node::nnodes=0;

#ifdef QDEBUG
  qDebug() << "Freeing node sets" << endl;
#endif
  for (vector<NodeSet *>::iterator i=node_sets.begin(); i!=node_sets.end(); i++) {
    delete *i;
  }
  node_sets.clear();


#ifdef QDEBUG
  qDebug() << "Freeing cells" << endl;
#endif	
  //CellsStaticDatamembers *old_static_data_mem = Cell::GetStaticDataMemberPointer();
  for (vector<Cell *>::iterator i=cells.begin(); i!=cells.end(); i++) {
    delete *i;
  }
  //Cell::static_data_members = old_static_data_mem;

  cells.clear();
  Cell::NCells()=0;

  if (boundary_polygon) {
    delete boundary_polygon; // (already deleted during cleaning of cells?)
    boundary_polygon=0;
  }
#ifdef QDEBUG
  qDebug() << "Freeing walls" << endl;
#endif
  for (list<Wall *>::iterator i=walls.begin(); i!=walls.end(); i++) {
    delete *i;
  }
  walls.clear();
  Wall::nwalls=0;

  node_insertion_queue.clear();
  shuffled_nodes.clear();
  shuffled_cells.clear();
  time = 0.0;
}

void Mesh::StandardInit(void) {

  boundary_polygon = new BoundaryPolygon();
  Cell &circle=CircularCell(0,0,10,10);

  circle.SetTargetArea(circle.CalcArea());
  circle.SetTargetLength(par.target_length);
  circle.SetLambdaLength(par.lambda_celllength);
  SetBaseArea();
  // clean up chemicals 
  for (int c=0; c<Cell::NChem(); c++) {
    circle.SetChemical(c, 0.);
  }
}

#include "hull.h"


double Mesh::Compactness(double *res_compactness, double *res_area, double *res_cell_area, double *res_circumference) {
  
  // Calculate compactness using the convex hull of the cells
  // We use Andrew's Monotone Chain Algorithm (see hull.cpp)

  // Step 1. Prepare data for 2D hull code - get boundary polygon
  int pc=0;
  Point *p=new Point[boundary_polygon->nodes.size()+1];
  for (list<Node *>::const_iterator i = boundary_polygon->nodes.begin(); 
       i!=boundary_polygon->nodes.end(); i++) {
    p[pc++]=Point((*i)->x,(*i)->y);
  }
  
  // chainHull algorithm requires sorted points
  sort( p, p+pc );

 
  // Step 2: call 2D Hull code
  int np=boundary_polygon->nodes.size();
  Point *hull=new Point[np+1];
  int nph=chainHull_2D(p,np,hull);
  
  
  // Step 3: calculate area and circumference of convex hull
  double hull_area=0.;
  double hull_circumference=0.;

  for (int i=0;i<nph-1;i++) {
    hull_area+=hull[i].x * hull[i+1].y - hull[i+1].x * hull[i].y;
    double s_dx=(hull[i+1].x-hull[i].x);
    double s_dy=(hull[i+1].y-hull[i].y);
    double l=sqrt(s_dx*s_dx+s_dy*s_dy);
    //    f << hull[i].x << " " << hull[i].y << " " << hull[i+1].x << " " << hull[i+1].y << " " << l << endl;
    hull_circumference+=l;
      
  }
  hull_area/=2.;

  // Step 4: get area of bounary polygon
  double boundary_pol_area = boundary_polygon->CalcArea();
  

  /*  ofstream datastr("hull.dat");
  for (int i=0;i<nph<i++) {
    datastr << hull.x << " " << hull.y << endl;
  }
  ofstream polstr("pol.dat");
  for (int i=0;i<np;h*/
  delete[] p;
  delete[] hull;


  // put intermediate results into optional pointers
  if (res_compactness) {
    *res_compactness = boundary_pol_area/hull_area;
  }
  if (res_area) {
    *res_area = hull_area;
  }
  if (res_cell_area) {
    *res_cell_area = boundary_pol_area;
  }
  if (res_circumference) {
    *res_circumference = hull_circumference;
  }
  
  // return compactness
  return boundary_pol_area/hull_area;

}

// DataExport
void Mesh::CSVExportCellData(QTextStream &csv_stream) const {

  csv_stream << "\"Cell Index\",\"Center of mass (x)\",\"Center of mass (y)\",\"Cell area\",\"Cell length\"";
  
  for (int c=0;c<Cell::NChem(); c++) {
    csv_stream << ",\"Chemical " << c << "\"";
  }
  csv_stream << Qt::endl;
  for (vector<Cell *>::const_iterator i=cells.begin();
       i!=cells.end();
       i++) {
    Vector centroid = (*i)->Centroid();
    csv_stream << (*i)->Index() << ", "
	       << centroid.x << ", "
	       << centroid.y << ", " 
	       <<  (*i)->Area() << ", "
	       <<(*i)->Length();
    for (int c=0;c<Cell::NChem(); c++) {
      csv_stream << ", " << (*i)->Chemical(c);
    }
    csv_stream << Qt::endl;
  }

  
}


// DataExport
void Mesh::CSVExportWallData(QTextStream &csv_stream) const {

  csv_stream << "\"Wall Index\",\"Cell A\",\"Cell B\",\"Length\"";
  
  for (int c=0;c<Cell::NChem(); c++) {
    csv_stream << ",\"Transporter A:" << c << "\"";
  }
  for (int c=0;c<Cell::NChem(); c++) {
    csv_stream << ",\"Transporter B:" << c << "\"";
  }
  csv_stream << Qt::endl;
  for (list<Wall *>::const_iterator i=walls.begin();
       i!=walls.end();
       i++) {
    csv_stream << (*i)->Index() << ","
	       << (*i)->C1()->Index() << ","
	       << (*i)->C2()->Index() << ","
	       << (*i)->Length();
	      
    for (int c=0;c<Cell::NChem(); c++) {
      csv_stream << "," << (*i)->Transporters1(c);
    }
    for (int c=0;c<Cell::NChem(); c++) {
      csv_stream << "," << (*i)->Transporters2(c);
    }

    csv_stream << Qt::endl;
  }

  
}


void Mesh::CSVExportMeshData(QTextStream &csv_stream) { 
  
  csv_stream << "\"Morph area\",\"Number of cells\",\"Number of nodes\",\"Compactness\",\"Hull area\",\"Morph circumference\",\"Hull circumference\"" << Qt::endl;
  
  double res_compactness, res_area, res_cell_area, hull_circumference;
  Compactness(&res_compactness, &res_area, &res_cell_area, &hull_circumference);
  double morph_circumference = boundary_polygon->ExactCircumference();
  csv_stream << Area() << ", " << NCells() << ", " << NNodes() << ", " << res_compactness << ", " << res_area << ", " << morph_circumference << ", " << hull_circumference << Qt::endl;
  
}
/* finis */
