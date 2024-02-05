/*
 *
 *  $Id$
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



#ifndef _WALLBASE_H_
#define _WALLBASE_H_

#include <list>
#include <iostream>
#include "vector.h"

class Node;
class NodeBase;
class CellBase;

using namespace std;

// warning, touches original sequence
template<class In, class Out> Out duplicates_copy(In first, In last, Out res) {

  In i = adjacent_find(first, last);

  while (i!=last) {
    res++ = *i;
    i = adjacent_find(++i, last);
  }

  return res;
}

/*! \class WallBase. A cell wall, which runs between cell "corner points",
  and consists of wall elements. */
class WallBase {

 protected:
  friend class CellBase;
  friend class Cell;
  friend class Mesh;
  //! Cells to which the wall belongs
  CellBase *c1, *c2;

  //! A list of transporter protein concentrations associated with the wall
  double *transporters1, *transporters2;
  double *new_transporters1, *new_transporters2;

  bool IllegalP(void) { return c1 == c2; }

  //! The chemicals in the apoplast at this position
  //double *apoplast;

  //! Pointers to the wall's corner nodes 
  Node *n1, *n2;

  double length;

  double viz_flux;

  //  bool aux_source;

  bool dead;
  // disallow usage of empty constructor
  WallBase(void) {}


  enum WallType {Normal, AuxSource, AuxSink};
  static int nwalls;
 protected:
  int wall_index;
  WallType wall_type;
  double c1WallStiffness, c2WallStiffness;

 public:
  WallBase(Node *sn1, Node *sn2, CellBase *sc1, CellBase *sc2); 

  // shallow copy
  WallBase(const WallBase &src) {
    c1 = src.c1;
    c2 = src.c2;
    c1WallStiffness = src.c1WallStiffness;
    c2WallStiffness = src.c2WallStiffness;
    transporters1 = src.transporters1;
    transporters2 = src.transporters2;
    new_transporters1 = src.new_transporters1;
    new_transporters2 = src.new_transporters2;
    //apoplast = src.apoplast;
    n1 = src.n1;
    n2 = src.n2;
    length = src.length;
    viz_flux = src.viz_flux;
    dead = src.dead;
    wall_index = src.wall_index;
  }

  inline int Index(void) const { return wall_index;}
  inline bool DeadP(void) const { return dead; }
  inline void Kill(void) { dead = true; }
  // deep copying of chemicals and transporters
  void CopyWallContents(const WallBase &src);
  void SwapWallContents(WallBase *src);
  bool is_wall_of_cell_p ( const CellBase *c ) {
    return (c1==c || c2==c);
  }

  inline CellBase *C1(void) const { return c1; }
  inline CellBase *C2(void) const { return c2; }
  inline Node *N1(void) const { return n1; }
  inline Node *N2(void) const { return n2; }
  inline void setTransporters1(int ch, double val) { transporters1[ch]=val; }
  inline void setTransporters2(int ch, double val) { transporters2[ch]=val; }
  inline void setNewTransporters1(int ch, double val) { new_transporters1[ch]=val; }
  inline void setNewTransporters2(int ch, double val) { new_transporters2[ch]=val; }
  inline double Transporters1(int ch) { return transporters1[ch]; }
  inline double Transporters2(int ch) { return transporters2[ch]; }
  void calculateDirectWallStiffNess(Node* nb, double* stiffness, int* count_p);
  void replaceNode(NodeBase* oldN, NodeBase* newN);
  //! Return true if the WallBase adheres to the SAM (shoot apical meristem)
  bool SAM_P(void);
  // NB. Not checked. If cell is not found, it returns transporters2[ch]!!
  //inline double getTransporter(int ch, Cell *c) { return c1 == c ? transporters1[ch] : transporters2[ch]; }

  //! Return true if the WallBase is a source of auxin
  inline bool AuxinSource(void) const { return wall_type == AuxSource; }
  inline bool AuxinSink(void) const { return wall_type == AuxSink; }

  inline void cycleWallType(void) { 

    if (wall_type == Normal)  
      wall_type = AuxSource;
    else
      if (wall_type == AuxSource) 
	wall_type = AuxSink;
      else
	if (wall_type == AuxSink) {
	  wall_type = Normal;
	}
  }
  // checked version. Use during debugging stage.
  inline double getTransporter(CellBase *c, int ch) const
  { 
    return c1 == c ? transporters1[ch] : ( c2 == c ? transporters2[ch] : throw "WallBase::getTransporter called with wrong cell") ; } 

  inline void setTransporter(CellBase *c, int ch, double val) { 
    if ( c1 == c ) {
      transporters1[ch]=val;
    } else 

      if (c2 == c ) {
	transporters2[ch]=val;
      } else {
	throw "WallBase::setTransporter called with wrong cell"; 
      }
  }
  //inline double getApoplast(int ch) const { return apoplast[ch]; }
  //inline void setApoplast(int ch, double val) { apoplast[ch] = val; }
  inline CellBase *getOtherCell(CellBase *c) { return c1 == c ? c2 : c1; }
  Vector getInfluxVector(CellBase *c);
  Vector getWallVector(CellBase *c);
  void CorrectTransporters(double orig_length);

  inline double Length(void) { return length; }
  //double Length(void);

  ostream &print(ostream &os) const; 

  void SetLength(void);
  void Transport(void);

  inline void setVizFlux( double value ) { viz_flux = value; } 

  /*! Return vector containing the directional flux through the wall.
    as defined by the value "viz_flux" which is supplied by the end-user
    in the TransportFunction.
  */
  Vector VizFlux(void);
  bool IntersectsWithDivisionPlaneP(const Vector &p1, const Vector &p2);
  void SetTransToNewTrans( void );
  bool isHasStartOrEnd(Node * node);

 private:
};

ostream &operator<<(ostream &os, const WallBase &w);
#endif

/* finis */
