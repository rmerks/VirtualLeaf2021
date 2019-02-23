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
#include<QGraphicsScene>
#include "cellitem.h"

static const std::string _module_id("$Id$");

CellItem::CellItem( Cell *c, QGraphicsScene *canvas )
  : QGraphicsPolygonItem( 0 ), SimItemBase( c, canvas){
}

void CellItem::userMove(double dx, double dy)
{
  QGraphicsPolygonItem::moveBy( dx, dy );

  // also move the cell itself
  class_cast<Cell *>(obj)->x += (dx/Cell::Magnification());
  class_cast<Cell *>(obj)->y += (dy/Cell::Magnification());

  class_cast<Cell*>(obj)->Move( (dx/Cell::Magnification()), (dy/Cell::Magnification()) );
}

QPainterPath CellItem::shape() const { return QGraphicsPolygonItem::shape(); }
QRectF CellItem::boundingRect() const { return QGraphicsPolygonItem::boundingRect(); }


/* finis */
