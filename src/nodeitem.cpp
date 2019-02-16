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
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOption>
#include <Qt>
#include "nodeitem.h"
#include "parameter.h"

static const std::string _module_id("$Id$");

extern Parameter par;

NodeItem::NodeItem( Node *n, QGraphicsScene *canvas )
  : QGraphicsItem( 0, canvas ), SimItemBase( n, canvas) {

  brush = Qt::darkGray;

  const double mag = par.node_mag;
  ellipsesize=QRectF(-1*mag, -1*mag, 2*mag, 2*mag);
}

void NodeItem::userMove(double dx, double dy)
{
  QGraphicsItem::moveBy( dx, dy );

  class_cast<Node *>(obj)->x += (dx/Cell::Magnification());
  class_cast<Node *>(obj)->y += (dy/Cell::Magnification());
}



void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
  painter->setBrush(brush);
  painter->setPen(Qt::NoPen);
  painter->drawEllipse(ellipsesize);
}


QPainterPath NodeItem::shape() const
{
  QPainterPath path;
  path.addEllipse(ellipsesize);
  return path;
}

QRectF NodeItem::boundingRect() const
{
  qreal penwidth = 0;// painter->pen()->widthF();
  return QRectF(ellipsesize.x()-penwidth/2.,ellipsesize.y()-penwidth/2.,
		ellipsesize.width()+penwidth, ellipsesize.height()+penwidth);
}

// polymorphic OnClick functions
void NodeItem::OnClick(void)
{
  Node *n = &getNode();
  n->toggleBoundary();
  setColor();
  update();
}

void NodeItem::OnClick(const Qt::MouseButton &mb)
{
  if (mb == Qt::LeftButton) {
    Node *n = &getNode();
    n->toggleBoundary();
    setColor();
    update();
  }
}

void NodeItem::setColor(void)
{
  static QColor indian_red("IndianRed");
  static QColor deep_sky_blue("DeepSkyBlue");
  static QColor purple("Purple");

  Node &n=getNode();

  if (n.SamP()) {
    setBrush( purple );
  } else {
    if (n.BoundaryP()) {
      setBrush( deep_sky_blue );
    } 
    else {
      setBrush( indian_red );
    }
  }
}

/* finis */
