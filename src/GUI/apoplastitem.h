/*
 *
 *  $Id$
 *
 *  This file is part of the Virtual Leaf.
 *
 *  The Virtual Leaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The Virtual Leaf is distributed in the hope that it will be useful,
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

#ifndef _APOPLASTITEM_H_
#define _APOPLASTITEM_H_

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QPainter>
#include <qpainter.h>
#include <QMouseEvent>
#include "simitembase.h"
#include "wall.h"

//! Shows transporter concentrations at one side of the wall

class ApoplastItem : public QGraphicsLineItem, public SimItemBase
{
 public:
  ApoplastItem( Wall *n, QGraphicsScene *canvas );
  virtual ~ApoplastItem() {}
  Wall &getWall(void) const { return *class_cast<Wall*>(obj); }
  void OnClick(QMouseEvent *e);  
  void setColor(void);
 private:
  int wn;
};

#endif

/* finis*/
