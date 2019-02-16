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


#ifndef _NEIGHBOR_H_
#define _NEIGHBOR_H_

#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace std;

class Cell;
class Node;

class Neighbor {

 protected:
  // Proper accessors and mutators should obviate these friend classes!
  friend class Cell;
  friend class Mesh;
  friend class Node;
  friend class FigureEditor;

  Cell *cell;
  Node *nb1, *nb2;

 public:
  Neighbor(void);
  Neighbor(Cell*, Node*, Node*);
  Neighbor(const Neighbor&);

  bool CellEquals(int) const;
  bool Cmp(Neighbor&) const;
  bool Eq(Neighbor&) const;
  Cell* getCell(void) const;
  void XMLAdd(xmlNodePtr) const;

  ostream &print(ostream &os) const;
};

// Overload the << operator 
ostream &operator<<(ostream &os, const Neighbor &n);

// Realize that we're equating like pointers with object equivalence!
bool neighbor_cell_eq(const Neighbor &n1, const Neighbor &n2);

#endif

/* finis */

