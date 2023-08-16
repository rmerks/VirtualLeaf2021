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

#include "Neighbor.h"
#include "cell.h"
#include <string>

static const std::string _module_id("$Id$");

Neighbor::Neighbor(void):
  cell(0), nb1(0), nb2(0){}

Neighbor::Neighbor(Cell *c, Node *n1, Node *n2):
  cell(c), nb1(n1), nb2(n2){}

Neighbor::Neighbor(const Neighbor &src):
  cell(src.cell), nb1(src.nb1), nb2(src.nb2){} // copy constructor

bool Neighbor::CellEquals(int i) const { return cell->Index() == i; } 
bool Neighbor::Cmp(Neighbor &c) const { return cell->Index() < c.cell->Index(); } // Compare cell indices not pointers.
bool Neighbor::Eq(Neighbor &c) const { return cell->Index() == c.cell->Index(); }
Cell* Neighbor::getCell(void) const { return cell; } 
void Neighbor::setPrevious(Node * nb) {nb1=nb;}
void Neighbor::setNext(Node * nb) {nb2=nb;}


bool neighbor_cell_eq(const Neighbor &n1, const Neighbor &n2) {
  return (n1.getCell())->Index() == (n2.getCell())->Index(); // Compare cell indices not pointers.
}

/* finis */

