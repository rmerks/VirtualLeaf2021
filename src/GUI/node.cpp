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
#include <sstream>
#ifdef QTGRAPHICS
#include <qstring.h>
#include <QGraphicsSimpleTextItem>
#endif
#include "node.h" 
#include "cell.h"
#include "mesh.h"
#include "random.h"
#include "parameter.h"
#include "sqr.h"
#include "pi.h"
#include "warning.h"

static const std::string _module_id("$Id$");

extern Parameter par;

int Node::nnodes=0;
double Node::target_length=1;

ostream &Edge::print(ostream &os) const {
  return os << "{ " << first->Index() << ", " << second->Index() << " }";
}

ostream &Neighbor::print(ostream &os) const {

  os << " {" << cell->Index() << " " << nb1->Index() << " " << nb2->Index() << "}";
  return os;
}

ostream &operator<<(ostream &os, const Neighbor &n) {
  n.print(os);
  return os;
}


Node::Node(void) : NodeBase()
{
  index=(nnodes++);
  node_set =0;
  fixed=false;
  boundary=false;
  sam=false;
  dead=false;
}

Node::Node(int ind) : NodeBase()
{
  node_set =0;
  index=ind;
  fixed=false;
  boundary=false;
  sam=false;
  dead=false;
}

Node::Node(const Vector &src) : NodeBase(src)
{
  node_set = 0;
  index=(nnodes++);
  fixed=false;
  boundary=false;
  sam=false;
  dead = false;
}

Node::Node(double x,double y, double z) : NodeBase (x,y,z)
{
  node_set = 0;
  index=(nnodes++);
  fixed=false;
  boundary=false;
  sam=false;
  dead = false;
}

Node::Node(const Node &src) : NodeBase(src)
{
  node_set=0;
  owners=src.owners;
  m=src.m;
  index=src.index;
  fixed = src.fixed;
  boundary = src.boundary;
  sam=src.sam;
  dead = src.dead;
}

int Node::Index(void) { return index; }

Cell &Node::getCell(const Neighbor &i)
{
  return *i.getCell(); // use accessor!
}


ostream &Node::print(ostream &os) const {

  if (!dead) 
    os << "Node ";
  else
    os << "DEAD NODE ";

  os  << index << "[ {" << x << ", " << y << ", " << z << "}: {";

  os << "Neighbors = { ";

  for (list<Neighbor>::const_iterator i =  owners.begin(); i!=owners.end(); i++) {
    os << " {" << i->cell->Index() << " " << i->nb1->Index() << " " << i->nb2->Index() << "} ";
  }
  os << " } " << endl;

  return os;
}


#ifdef QTGRAPHICS
void Node::DrawIndex(QGraphicsScene *c) const {

  //return DrawOwners(c);
  stringstream text;
  text << index;
  QGraphicsSimpleTextItem *number = new QGraphicsSimpleTextItem ( QString (text.str().c_str()), 0);
  number->setFont( QFont( "Helvetica", par.nodenumsize, QFont::Normal) );
  number->setBrush( QBrush (par.textcolor) );
  number->setZValue(20);
  Vector offs=Cell::Offset();
  number -> setPos(
		   ((offs.x+x)*Cell::Factor()),
		   ((offs.y+y)*Cell::Factor()) );
    c->addItem(number);
    number->show();

}

void Node::DrawOwners(QGraphicsScene *c) const {

  stringstream text;
  text << owners.size();

  QGraphicsSimpleTextItem *number = new QGraphicsSimpleTextItem ( QString (text.str().c_str()), 0);
  number->setFont( QFont( "Helvetica", par.nodenumsize, QFont::Bold) );
  number->setPen( QPen(par.textcolor) );
  number->setZValue(20);
   Vector offs=Cell::Offset();

  number ->setPos(((offs.x+x)*Cell::Factor()),
		  ((offs.y+y)*Cell::Factor()) );
    c->addItem(number);
    number->show();

}


QVector<qreal> Node::NeighbourAngles(void)
{
  QVector<qreal> angles;
  for (list<Neighbor>::iterator i=owners.begin(); i!=owners.end(); i++) {
    Vector v1 = (*this - *i->nb1).Normalised();
    Vector v2 = (*this - *i->nb2).Normalised();	

    double angle = v1.SignedAngle(v2);
    if (angle<0) {
      //cerr << "Changing sign of angle " << angle << endl;
      angle = angle + 2*Pi;
    }
    angles.push_back(angle);

    //cerr << "Cell " << i->cell->Index() << ": " <<  v1 << " and " << v2 
    //     << ": angle = " << angles.back() << ", " << v1.Angle(v2) << endl;

  }

  double sum=0.;
  for (QVector<qreal>::iterator i=angles.begin(); i!=angles.end(); i++) {
    sum+=*i;
  }
  //cerr << "Angle sum = " << sum << endl;
  // Check if the summed angle is different from 2 Pi 
  if (fabs(sum-(2*Pi))>0.0001) {

    MyWarning::warning("sum = %f",sum);
  }
  return angles;
}

#endif

ostream &operator<<(ostream &os, const Node &n) {
  n.print(os);
  return os;
}

void Node::correctNeighbors(int cellIndex, Node* n1, Node* n3) {
	for (list<Neighbor>::iterator it=this->owners.begin(); it!=this->owners.end(); ++it) {
		Neighbor &nb=*it;
		if (nb.CellEquals(cellIndex)) {
			if (nb.nb1!=n1) {
				nb.nb1=n1;
			}
			if (nb.nb2!=n3) {
				nb.nb2=n3;
			}
		}
	}
}
void Node::removeCell(CellBase * cell) {
	owners.erase(
	std::remove_if(owners.begin(), owners.end(),
			[this,cell](const Neighbor & o) { return o.CellEquals(cell->index); }),
			this->owners.end());
}

void Node::addCell(CellBase * cell) {
	owners.push_back( Neighbor( (Cell*)cell, NULL, NULL ) );
}


int Node::countNeighbors(void) {
	return owners.size();
}


void Node::splitWallElements(WallElementInfo *base,double ratioOfBase) {
	this->insertWallElement(base->getCell());
	WallElementInfo sub;
	base->getCell()->fillWallElementInfo(&sub, this, (Node*)base->getTo());
	base->divide(&sub,(1.-ratioOfBase));
}

void Node::splittWallElementsBetween(Node *from, Node *to) {
	double original = ((*to)-(*from)).Norm();
	double fromPart = ((*from)-(*this)).Norm()/original;
	double toPart = ((*to)-(*this)).Norm()/original;

	for (list<Neighbor>::const_iterator owner = this->owners.begin();
			owner != this->owners.end(); owner++) {
		if (from->getWallElement(owner->cell) != NULL) {
			WallElementInfo info;
			owner->cell->fillWallElementInfo(&info, from, to);
			if (info.hasWallElement()) {
				splitWallElements(&info,fromPart);
			}
		}
		if (to->getWallElement(owner->cell) != NULL) {
			WallElementInfo info;
			owner->cell->fillWallElementInfo(&info, to, from);
			if (info.hasWallElement()) {
				splitWallElements(&info,toPart);
			}
		}
	}
}
/* finis */
