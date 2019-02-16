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


#ifndef _WALLITEM_H_
#define _WALLITEM_H_

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QPainter>
#include <qpainter.h>
#include <QMouseEvent>
#include "simitembase.h"
#include "wall.h"

//! Shows transporter concentrations at one side of the wall

class WallItem : public QGraphicsLineItem, public SimItemBase
{
 public:
  WallItem( Wall *n, int wallnumber, QGraphicsScene *canvas );
  virtual ~WallItem() {}
  Wall &getWall(void) const { return *class_cast<Wall*>(obj); }
  void OnClick(QMouseEvent *e);  
  void setColor(void);
 private:
  int wn;
};

#endif

/* finis */
