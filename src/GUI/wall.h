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


#ifndef _WALL_H_
#define _WALL_H_

#include "wallbase.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

#include<QGraphicsScene>

class Wall : public WallBase {

 public:
 Wall(Node *sn1, Node *sn2, CellBase *sc1, CellBase *sc2) : WallBase(sn1, sn2, sc1, sc2) {}


  void XMLAdd(xmlNodePtr parent_node) const;
  bool CorrectWall(void);


  // Graphics:
  //! Visualize transport protein concentrations
  void Draw(QGraphicsScene *c);

  //! Visualize contents of the apoplast
  //void DrawApoplast(QGraphicsScene *c); 
  /*! \brief Visualize the structure of the wall (Cell ID's etc.). 
    Used for debugging purposes.
  */
  void ShowStructure(QGraphicsScene *c);

 private:
  string WallTypetoStr(const WallType &wt) const;
};

#endif

/* finis */
