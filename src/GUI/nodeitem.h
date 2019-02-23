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


#ifndef _NODEITEM_H_
#define _NODEITEM_H_

#include <QGraphicsScene>
#include <QGraphicsItem>
#include "simitembase.h"
#include "node.h"
#include "cell.h"

class NodeItem: public QGraphicsItem, public SimItemBase
{
 public:
  NodeItem( Node *n, QGraphicsScene *canvas );
  ~NodeItem() {}
  Node &getNode(void) const { return *class_cast<Node*>(obj); }
  virtual void userMove(double dx, double dy);  

  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  QPainterPath shape() const;
  void setBrush( QBrush newbrush) { brush = newbrush; }
  void setColor(void); 
  /*! We use this function internally, to (somewhat) interactively edit init configurations.
    Simply put the property to be change upon clicking a node in this function. */
  void OnClick( const Qt::MouseButton &mb );
  void OnClick(void);
 protected:
  QBrush brush;
  QRectF ellipsesize;

 private:
};

#endif

/* finis */
