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


#ifndef _QCANVASARROW_H_
#define _QCANVASARROW_H_

#include <QGraphicsScene>
#include <QWidget>
#include <QGraphicsLineItem>
#include <QPainter>
#include <iostream>

class QGraphicsArrowItem : public QGraphicsLineItem {

 public:
 QGraphicsArrowItem(QGraphicsItem *parent) : QGraphicsLineItem(parent) {};

  //void paint ( QPainter *p ) {
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr){
    // construct arrow head
    QPointF start=line().p1();
    QPointF end=line().p2();
    QPointF mid=start + (3./4.)*(end-start);

    double vx=end.x()-start.x();
    double vy=end.y()-start.y();

    double length = sqrt(vx*vx+vy*vy);
    if (length==0) return;

    // perpendicular vector
    double px=-vy/length;
    double py=vx/length;

    // Arrow head lines go from end point
    // to points about 3/4 of the total arrow, extending sideways about 1/4
    // of the arrow length.


    QPointF arwp1 = mid + QPointF( (int)( (length/4.)*px ),
				   (int)( (length/4.)*py ) );
    QPointF arwp2 = mid - QPointF( (int)( (length/4.)*px ),
				   (int)( (length/4.)*py ) );

    painter->setPen(pen());
     //Draw arrow head
    painter->drawLine( end, arwp1 );
    painter->drawLine( end, arwp2 );
     //Draw arrow line
    painter->drawLine( start, end);

  }
};

#endif

/* finis */
