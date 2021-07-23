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
#include "wall.h"
#include "cell.h"
#include "wallitem.h"
#include "node.h"
//#include "apoplastitem.h"
#include <algorithm>
#include <QGraphicsScene>

static const std::string _module_id("$Id$");

/*! Check if this Wall still belongs to Cell a, otherwise gives it to Cell b.
  \return true: Wall was corrected
  \return false: Wall was still correct
*/
bool Wall::CorrectWall( void ) {


  // collect all cells to which my nodes are connected on one list
  list<CellBase *> owners;
  transform(n1->owners.begin(),n1->owners.end(), back_inserter(owners), mem_fn(&Neighbor::getCell));
  transform(n2->owners.begin(),n2->owners.end(), back_inserter(owners), mem_fn(&Neighbor::getCell));

  // get the list of duplicates
  list<CellBase *> wall_owners;
  owners.sort();
  //transform(owners.begin(), owners.end(), ostream_iterator<int>(cerr, ", "), mem_fn(&Cell::Index));
  duplicates_copy( owners.begin(), owners.end(), back_inserter(wall_owners) );

  // duplicates are the cells to which the Wall belongs
  // For the first division, wall finds three "owners", including the boundary_polygon.
  // Remove that one from the list.
  //cerr << "wall_owners.size() = " << wall_owners.size() << endl;
  if (wall_owners.size() == 3 && nwalls==1 /* bug-fix 22/10/2007; confine this condition to first division only */) {

#ifdef QDEBUG
    qDebug() << "nwalls = " << nwalls << endl;
#endif
    // special case for first cleavage

    // find boundary polygon in the wall owners list
    list<CellBase *>::iterator bpit = find_if (
					       wall_owners.begin(), wall_owners.end(), 
                           mem_fn( &CellBase::BoundaryPolP )
					       );
    if (bpit!=wall_owners.end()) {

      // add a Wall with the boundary_polygon to each cell
      Wall *bp_wall1 = new Wall( n2, n1, c1, (*bpit) );
      bp_wall1->CopyWallContents(*this);
      ((Cell *)c1)->AddWall( bp_wall1);
      ((Cell *)(*bpit))->AddWall(bp_wall1);

      Wall *bp_wall2 = new Wall( n1, n2, c2, (*bpit) );
      bp_wall2->CopyWallContents(*this);
      ((Cell *)c2)->AddWall( bp_wall2);
      ((Cell *)(*bpit))->AddWall(bp_wall2);

      wall_owners.erase(bpit); 

    }else {

#ifdef QDEBUG
      qDebug() << "Wall::CorrectWall says: Wall has three owners, but none of them is the BoundaryPolygon. I have no clue what to do with this case... Sorry!" << endl;
      cerr << "Wall: " << *this << endl;
      qDebug() << "Owners are: ";
      transform(wall_owners.begin(), wall_owners.end(), ostream_iterator<int>(cerr, "  "), mem_fn (&CellBase::Index) );
      qDebug() << endl;
      qDebug() << "Owners node " << n1->Index() << ": ";

      for (list<Neighbor>::iterator i = n1->owners.begin(); i!=n1->owners.end(); i++) {
	qDebug() << i->getCell()->Index() << " ";
      }

      qDebug() << endl;
      qDebug() << "Owners node " << n2->Index() << ": ";

      for (list<Neighbor>::iterator i = n2->owners.begin(); i!=n2->owners.end(); i++) {
	qDebug() << i->getCell()->Index() << " ";
      }
      qDebug() << endl;
#endif
      std::exit(1);
    }
  }

#ifdef QDEBUG
  if (wall_owners.size() == 1) {
    qDebug() << "Corner point. Special case." << endl;
  }
#endif

  CellBase *cell1 = wall_owners.front();
  CellBase *cell2 = wall_owners.back();

  if ( (c1 == cell1 && c2==cell2) || (c1 == cell2 && c2 == cell1) ) {

    return false;
  }

  if ( c1 == cell1 ) {
    //cerr << "Block 1\n";
    ((Cell *)c2) -> RemoveWall(this);
    c2 = cell2;
    ((Cell *)c2) -> AddWall(this);
  } else {
    if ( c1 == cell2 ) {
      //	cerr << "Block 2\n";
      ((Cell *)c2) -> RemoveWall(this);
      c2 = cell1;
      ((Cell *)c2) -> AddWall(this);
    } else {
      if ( c2 == cell1) {
	((Cell *)c1)->RemoveWall(this);
	c1 = cell2;
	((Cell *)c1) -> AddWall(this);
	//  cerr << "Block 3\n";
      } else {
	if ( c2 == cell2) {
	  ((Cell *)c1)->RemoveWall(this);
	  c1 = cell1;
	  ((Cell *)c1)->AddWall(this);
	  //	  cerr << "Block 3\n";
	} else {
#ifdef QDEBUG
	  qDebug() << "Warning, cell wall was not corrected." << endl;
#endif
	  return false;
	}
      }
    }
  }
  return true;
}

void Wall::Draw(QGraphicsScene *c) {

  WallItem *wi1 = new WallItem(this, 1, c);
  WallItem *wi2 = new WallItem(this, 2, c);
    c->addItem(wi1); c->addItem(wi2);
  wi1->show();
  wi2->show();
}

/* void Wall::DrawApoplast(QGraphicsScene *c) {
  ApoplastItem *apo = new ApoplastItem(this, c);
  apo->show();
}*/

void Wall::ShowStructure(QGraphicsScene *c) {

  Vector offset = Cell::Offset();
  double factor = Cell::Factor();

  Vector startpoint ( ((offset.x+n1->x)*factor),((offset.y+n1->y)*factor)),
    endpoint (((offset.x+n2->x)*factor),((offset.y+n2->y)*factor));

  Vector linevec = endpoint - startpoint;
  Vector midline = startpoint + linevec/2.;
  Vector perpvec = linevec.Normalised().Perp2D();
  Vector textpos1 = midline + 100 * perpvec;
  Vector textpos2 = midline - 100 * perpvec;

  QGraphicsLineItem *line = new QGraphicsLineItem(0);

  line->setPen( QPen(QColor(par.arrowcolor),2) );
  line->setLine(startpoint.x, startpoint.y, endpoint.x, endpoint.y );
  line->setZValue(10);
    c->addItem(line);
  line->show();

  QGraphicsSimpleTextItem *text1 = new QGraphicsSimpleTextItem( QString("%1").arg(c2->Index()),0);
  QGraphicsSimpleTextItem *text2 = new QGraphicsSimpleTextItem( QString("%1").arg(c1->Index()),0);

  text1 -> setPos( textpos1.x, textpos1.y );
  text2 -> setPos( textpos2.x, textpos2.y );
  text1->setZValue(20); text2->setZValue(20);

  text1->setFont( QFont( "Helvetica", par.nodenumsize, QFont::Bold) );
  text2->setFont( QFont( "Helvetica", par.nodenumsize, QFont::Bold) );

    text1->setBrush ( QBrush(QColor(par.textcolor)) ); text1->setPen(Qt::NoPen);
    text2->setBrush ( text1->brush() ); text2->setPen(Qt::NoPen);
    c->addItem(text1); c->addItem(text2);
  text1->show(); text2->show();
}
string Wall::WallTypetoStr(const WallType &wt) const {

  if (wt == Normal) {
    return string("normal");
  }

  if (wt == AuxSource) {
    return string("aux_source");
  }

  if (wt == AuxSink) {
    return string("aux_sink");
  }

  return string("");
}

/* finis */
