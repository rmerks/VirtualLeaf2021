/*
 *  VirtualLeaf
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

#ifndef _CELLITEM_H_
#define _CELLITEM_H_

#include <QGraphicsScene>
#include <QGraphicsPolygonItem>
#include <QPainter>
#include "simitembase.h"
#include "cell.h"


class CellItem : public QGraphicsPolygonItem, public SimItemBase
{
 public:
  CellItem( Cell *n, QGraphicsScene *canvas );
  virtual ~CellItem() {}
  Cell &getCell(void) const { return *class_cast<Cell*>(obj); }
  virtual void userMove(double dx, double dy);  
  QPainterPath shape() const;
  QRectF boundingRect() const;

 private:
};

#endif

/* finis */
