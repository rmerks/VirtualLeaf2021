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


#ifndef _NODE_H_
#define _NODE_H_

#include <list>
//#include <pair>

#ifdef QTGRAPHICS
#include <QGraphicsScene>
#include <qcolor.h>
#include <iostream>
#endif
#include "nodebase.h"
#include "random.h"
#include "parameter.h"
#include "cell.h"
#include "modelelement.h"
#include "wallelement.h"

#include <QVector>

#include "Neighbor.h"

extern Parameter par;

class Edge {

 public:

  Edge(void) {
    first=0;
    second=0;
  }

  Edge(Node *f, Node *s) {
    first=f;
    second=s;
  }

  // Copy Constructor
  Edge(const Edge &src) {
    first=src.first;
    second=src.second;
  }

  // Ambidextrous equivalence 
  inline bool operator==(const Edge &e) {
    return ( (first==e.first && second==e.second) ||
	     (first==e.second && second==e.first) );
  }


  // Output the Edge
  ostream &print(ostream &os) const;

  Node *first, *second;
};


class NodeSet;

// this class is a node in a cell wall
class Node : public NodeBase {

  friend class Mesh;
  friend class CellBase;
  friend class Cell;
  friend class WallBase;
  friend class Wall;
  friend class NodeSet;
  friend class FigureEditor;
  friend class WallElementInfo;

 public:
  Node(void);

  Node(int index); // if want to construct a node, and not increase nnodes

  Node(double x,double y, double z=0);

  Node(const Node &src);

  explicit Node(const Vector &src); // "nonconverting" - explicit constructor syntax required

  virtual ~Node() {}

  inline int Index(void) const { return index; }

  inline bool IndexEquals(int i) { return i == index; }

  inline bool BoundaryP(void) const { return boundary; }

  inline void SetBoundary(void) { boundary = true; }

  inline void UnsetBoundary(void) { boundary = false; }

  inline void SetSAM(void) { sam = true; }

  inline void toggleBoundary(void) {
    boundary = !boundary;
  }


  Cell &getCell(const Neighbor &i);

  ostream &print(ostream &os) const;
  void XMLAdd(QDomDocument &doc, QDomElement &nodes_node) const;

#ifdef QTGRAPHICS
  void Draw(QGraphicsScene &c, QColor color=QColor("black"), int size = 10) const;
  void DrawIndex(QGraphicsScene *c) const;
  void DrawOwners(QGraphicsScene *c) const;
#endif

  // temporary function for easier debugging
  inline int CellsSize(void) const { return owners.size(); }

  inline int Value(void) const { return owners.size(); }

  void Fix(void) { fixed=true; }

  inline bool Fixed(void) const { return fixed; }

  inline void Unfix(void) { fixed=false; }

  inline void MarkDead(void) { dead=true; }

  inline bool DeadP(void) { return dead; }

  inline void Mark(void) { marked=true; }

  inline void Unmark(void) { marked=false; }

  inline bool Marked(void) const { return marked; }

  inline void setPos( Vector p ) { 
    x = p.x;
    y = p.y;
    z = p.z;
  }

  inline bool SamP(void) const { return sam; }

  //!\brief Calculate angles with neighboring vertices
  //! Sum of angles should be 2*Pi
  QVector<qreal> NeighbourAngles(void);
 private:

  // "owners" lists the cells to which this cell belong
  // and the two neighboring nodes relative to each cell
  list< Neighbor > owners;

  Mesh *m;
  int index;
  static int nnodes;
  static double target_length;

  // if the node belongs to a NodeSet, node_set contains the pointer. Otherwise it is 0.
  NodeSet *node_set; 
  // fixed nodes cannot move. E.g. to represent the petiole
  bool fixed;
  bool boundary; // true if node is at the edge of the leaf
  bool sam; // true if node is connected to the shoot
  bool dead;
  bool marked;
};

ostream &operator<<(ostream &os, const Node &n);

inline ostream &operator<<(ostream &os, const Edge &e) {
  e.print(os);
  return os;
}

#endif

/* finis */
