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

#include "wall.h"
#include "wallbase.h"
#include "node.h"
#include "mesh.h"
#include "parameter.h"
#include <sstream>
#include <string>
#include "warning.h"

#ifdef QTGRAPHICS
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include "wallitem.h"
//#include "apoplastitem.h"
#endif

static const std::string _module_id("$Id$");

int WallBase::nwalls=0;

ostream &WallBase::print(ostream &os) const {
  os << "{ " << n1->Index() << "->" << n2->Index() 
     << ", " << c1->Index() << " | " << c2->Index() << "} ";
  return os;
}

ostream &operator<<(ostream &os, const WallBase &w) { 
  w.print(os); 
  return os;
}


WallBase::WallBase(Node *sn1, Node *sn2, CellBase *sc1, CellBase *sc2)
{

#ifdef QDEBUG
  if (sc1==sc2) { 
    qDebug() << "Attempting to build a wall between identical cells: " << sc1->Index() << endl; 
  }
#endif

  c1 = sc1;
  c2 = sc2;

  n1 = sn1;
  n2 = sn2;

  transporters1 = new double[CellBase::NChem()];
  transporters2 = new double[CellBase::NChem()];
  new_transporters1 = new double[CellBase::NChem()];
  new_transporters2 = new double[CellBase::NChem()];

  for (int i=0;i<CellBase::NChem();i++) {
    transporters1[i] = transporters2[i] = new_transporters1[i] = new_transporters2[i] = 0.;
  }

//  apoplast = new double[CellBase::NChem()]; // not yet in use.

  SetLength();

  // to visualize flux through WallBase
  viz_flux=0;
  dead = false;
  wall_type = Normal;
  wall_index = nwalls++;
  weigthFactor1=1.0;
  weigthFactor2=1.0;
}

void WallBase::CopyWallContents(const WallBase &src)
{

  for (int i=0; i<CellBase::NChem(); i++) {
    if (transporters1) {
      transporters1[i]=src.transporters1[i];
    }
    if (transporters2) {
      transporters2[i]=src.transporters2[i];
    }
    if (new_transporters1) {
      new_transporters1[i]=src.new_transporters1[i];
    }
    if (new_transporters2) {
      new_transporters2[i]=src.new_transporters2[i];
    }

  /*  if (apoplast) {
      apoplast[i]=src.apoplast[i];
    }*/
  }
  dead = src.dead;
  wall_type = src.wall_type;
  weigthFactor1 = src.weigthFactor1;
  weigthFactor2 = src.weigthFactor2;
}

void WallBase::SwapWallContents(WallBase *src)
{

  for (int i=0; i<CellBase::NChem(); i++) {
    if (transporters1) {
      double tmp;

      tmp=src->transporters1[i];
      src->transporters1[i]=transporters1[i];
      transporters1[i]=tmp;

    }
    if (transporters2) {
      double tmp;
      tmp=src->transporters2[i];
      src->transporters2[i]=transporters2[i];
      transporters2[i]=tmp;
    }
    if (new_transporters1) {
      double tmp;
      tmp=src->new_transporters1[i];
      src->new_transporters1[i]=new_transporters1[i];
      new_transporters1[i]=tmp;
    }
    if (new_transporters2) {
      double tmp;
      tmp=src->new_transporters2[i];
      src->new_transporters2[i]=new_transporters2[i];
      new_transporters2[i]=tmp;			
    }

  /*  if (apoplast) {
      double tmp;
      tmp=src->apoplast[i];
      src->apoplast[i]=apoplast[i];
      apoplast[i]=tmp;		
    }*/
  }
  bool tmp_bool;
  tmp_bool = src->dead;
  src->dead=dead;
  dead = tmp_bool;

  WallType tmp_wall_type;
  tmp_wall_type = src->wall_type;
  src->wall_type = wall_type;
  wall_type = tmp_wall_type;

  bool tmp_double;
  tmp_double = src->weigthFactor1;
  src->weigthFactor1=weigthFactor1;
  weigthFactor1 = tmp_double;

  tmp_double = src->weigthFactor2;
  src->weigthFactor2=weigthFactor2;
  weigthFactor2 = tmp_double;
}

bool WallBase::SAM_P(void)
{ 
  return N1()->sam || N2()->sam; 
}  

#include <fstream>

void WallBase::SetLength(void)
{

  // Step 1: find the path of nodes leading along the WallBase.
  // A WallBase often represents a curved cell wall: we want the total
  // length _along_ the wall here...

  // Locate first and second nodes of the edge in Cell's list of nodes
  list<Node *>::const_iterator first_node_edge = find(c1->nodes.begin(), c1->nodes.end(), n1);
  list<Node *>::const_iterator second_node_edge_plus_1 = ++find(c1->nodes.begin(), c1->nodes.end(), n2);

  // wrap around
  if (second_node_edge_plus_1 == c1->nodes.end()) {
    second_node_edge_plus_1 = c1->nodes.begin();
  }


  length = 0.;

  // Now, walk to the second node of the edge in the list of nodes
  stringstream deb_str;

  for (list<Node *>::const_iterator n=
	 (++first_node_edge==c1->nodes.end()?c1->nodes.begin():first_node_edge);
       n!=second_node_edge_plus_1;
       (++n == c1->nodes.end()) ? (n=c1->nodes.begin()):n  ) {

    list<Node *>::const_iterator prev_n = n; 
    if (prev_n==c1->nodes.begin()) prev_n=c1->nodes.end();
    --prev_n;

    //cerr << "Node: " << (Vector)(**n) << endl;

    // Note that Node derives from a Vector, so we can do vector calculus as defined in vector.h 

    deb_str << "[ " << (*prev_n)->index << " to " << (*n)->index << "]";
    length += (*(*prev_n) - *(*n)).Norm(); 
  }
}

void WallBase::CorrectTransporters(double orig_length)
{

  double length_factor = length / orig_length;

  for (int ch=0;ch<CellBase::NChem();ch++) {
    transporters1[ch] *= length_factor;
    transporters2[ch] *= length_factor;
    new_transporters1[ch] *= length_factor;
    new_transporters2[ch] *= length_factor;
  }
}


Vector WallBase::VizFlux(void)
{
  return viz_flux * ( (*n2) - (*n1) ).Normalised().Perp2D();    
}


void WallBase::SetTransToNewTrans( void ){ 
  for (int i=0;i<CellBase::NChem();i++) { 
    transporters1[i] = new_transporters1[i]; 
    transporters2[i] = new_transporters2[i]; 
  } 
}


Vector WallBase::getWallVector(CellBase *c)
{
  if ( c == c1 ) {
    return ( Vector(*n2) - Vector(*n1) );
  } else {
    return ( Vector(*n1) - Vector(*n2) );
  }
}
Vector WallBase::getInfluxVector(CellBase *c)
{
  if ( c == c1 ) {
    return ( Vector(*n2) - Vector(*n1) ).Normalised().Perp2D();
  } else {
    return ( Vector(*n1) - Vector(*n2) ).Normalised().Perp2D();
  }
}

//! \brief Test if this wall intersects with division plane p1 -> p2 
bool WallBase::IntersectsWithDivisionPlaneP(const Vector &p1, const Vector &p2)
{

  // Algorithm of http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
  double x1 = n1->x, y1 = n1->y;
  double x2 = n2->x, y2 = n2->y;
  double x3 = p1.x, y3 = p1.y;
  double x4 = p2.x, y4 = p2.y;

  double ua = ( (x4 - x3) * (y1-y3) - (y4 - y3)*(x1-x3) ) / ( (y4 -y3) * (x2 -x1) - (x4-x3)*(y2-y1));

  // If ua is between 0 and 1, line p1 intersects the line segment
  if ( ua >=0. && ua <=1.) return true;
  else return false;
}

/* finis */
