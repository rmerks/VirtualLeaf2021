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


#include <string>
#include <QGraphicsScene>
#include "canvas.h"
#include "parameter.h"
#include "node.h"
#include "apoplastitem.h"

static const std::string _module_id("$Id$");

ApoplastItem::ApoplastItem( Wall *w, QGraphicsScene *canvas )
  : QGraphicsLineItem( 0, canvas ), SimItemBase( w, canvas){

  setColor();

  // line with "PIN1"is a bit inside the cell wall
  Vector edgevec = (*(w->N2())) - (*(w->N1()));
  Vector perp = edgevec.Normalised().Perp2D();

  Vector offs = Cell::Offset();
  double factor = Cell::Factor();

  Vector from = ( offs + *(w->N1()) ) * factor;
  Vector to = ( offs + *(w->N2()) ) *factor;


  setLine(( from.x ),
	  ( from.y ),
	  ( to.x ),
	  ( to.y ) );
  setZValue(12);
}


void ApoplastItem::setColor(void) {

  QColor diffcolor;
  static const QColor purple("Purple");
  static const QColor blue("blue");

  Wall *w=&getWall();
  double val = w->getApoplast(2);

  diffcolor.setRgb( 0,0,(int)( ( val / (1 + val) )*255.));
  setPen (QPen(diffcolor, 20) );
}

void ApoplastItem::OnClick(QMouseEvent *e) {}

/* finis*/
