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

// CellBase derives from Vector, where Vector is simply used as a Vertex

#ifndef _CELLWALLCURVE_H_
#define _CELLWALLCURVE_H_

#include <cmath>
#include <list>
#include <vector>
#include <iostream>
#include <QString>
#include <QDebug>

#include "nodebase.h"
#include "wallbase.h"
#include "cellbase.h"
#include "warning.h"
#include "assert.h"

class CellWallCurve {
	friend class NodeBase;
	friend class CellBase;

	bool borderCase;

	CellBase * cell;

	NodeBase * from=NULL;
	NodeBase * over=NULL;
	NodeBase * to=NULL;


	WallBase* findWallBetweenEndingAt(CellBase *&c1, CellBase *&c2, NodeBase *&c);

	WallBase* otherWallEndingAt(CellBase *c3, NodeBase *&c, WallBase *&wallc2c3);

public:
	CellWallCurve(double potential_slide_angle) {
		reset();
	}
	bool isBorderCase() {return borderCase;};
	void attachToCell();
	void set(CellWallCurve * other);
	void shift(NodeBase * node) ;
	bool checkBudEnd(NodeBase * node) ;
	void reset() ;
	int Index() ;
	void setCell(CellBase * aCell) ;
	void setTo(NodeBase * node) ;
	NodeBase* getFrom()  ;
	NodeBase* getOver() ;
	NodeBase* getTo() ;
	NodeBase* longerWall() ;
	CellBase* cellBehindLongerWall();
	CellBase* cellBehindShorterWall() ;
	CellBase* cellBehindWallToNode(NodeBase * spikeEnd);
	/**
	 * Now we collapse the spike by letting the longer wall end at the end of the shorter wall.
	 * Assume a sharp spike in a wall from node a to c (a-c) to b (c-b). were a-c is the longer
	 * wall. We remove this spike if the angle between a-c and b-c gets to small. We do this
	 * by replacing a-c and with a new wall a-b.
	 */
	bool removeSpike() ;
	bool isWallBetweenEndingAt(WallBase * wall, CellBase* cell1, CellBase* cell2, NodeBase * node);
};

inline bool cmpCellWallCurve( CellWallCurve &a,  CellWallCurve &b) {
	return b.Index() - a.Index();
}

inline bool eqCellWallCurve( CellWallCurve &a,  CellWallCurve &b) {
	return b.Index() == a.Index();
}

#endif
