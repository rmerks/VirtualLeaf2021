/*
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

#include <QObject>
#include <QtGui>


#include "parameter.h"
#include "node.h"
#include "wallbase.h"
#include "cellbase.h"
#include "tutorial1C.h"

#include "CellOrientation.h"

static const std::string _module_id("$Id$");

QString Tutorial1C::ModelID(void) {
  // specify the name of your model here
  return QString( "1C: Vertical cell division" );
}

// return the number of chemicals your model uses
int Tutorial1C::NChem(void) { return 0; }

// To be executed after cell division
void Tutorial1C::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
  // rules to be executed after cell division go here
  // (e.g., cell differentiation rules)
}

void Tutorial1C::SetCellColor(CellBase *c, QColor *color) { 
  // add cell coloring rules here
}

void Tutorial1C::CellHouseKeeping(CellBase *c) {
    // add cell behavioral rules here
	if (c->FixedP()||c->CellType() == 0||c->CellType() == 1||c->CellType() == 4) {
		return;
	}
//	if (c->TargetArea() < 1.) {
//		c->SetTargetArea(c->Area());
//	}
	CellOrientation orientation =  c->calculateOrientation();
	if (orientation.initialized && false) {
		QList<WallBase *> list = c->getWalls();
		double dStart = min(//
				sqrt((orientation.divide25Start-orientation.minimaStart).SqrNorm()),//
				sqrt((orientation.divide25End-orientation.minimaStart).SqrNorm()));
		double dEnd = min(//
				sqrt((orientation.divide75Start-orientation.minimaEnd).SqrNorm()),//
				sqrt((orientation.divide75End-orientation.minimaEnd).SqrNorm()));
		for (QList<WallBase*>::iterator w = list.begin(); w != list.end();w++) {
			Vector middle = ((*(*w)->N1())+(*(*w)->N2()))/2.;
			double d1 = sqrt((middle-orientation.minimaStart).SqrNorm());
			double d2 = sqrt((middle-orientation.minimaEnd).SqrNorm());
			double weightFactor = 1.;
			if (d1 < dStart) {
				weightFactor = d1/dStart;
			}else if (d2 < dEnd) {
				weightFactor = d2/dEnd;
			}
			(*w)->SetWeightFactor(weightFactor,c);
		}
	}
	c->EnlargeTargetArea(par->cell_expansion_rate);
	if (false && c->Area() > par->rel_cell_div_threshold * c->BaseArea()) {
		if (orientation.initialized) {
			Vector divideVector = orientation.divide50End-orientation.divide50Start;
			Vector center = (orientation.divide50End+orientation.divide50Start)/2.0;
			c->DivideOverAxis(divideVector,center);
		} else {
			c->Divide();
		}
	}
}

void Tutorial1C::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
  // add biochemical transport rules here
}
void Tutorial1C::WallDynamics(Wall *w, double *dw1, double *dw2) {
  // add biochemical networks for reactions occuring at walls here
}
void Tutorial1C::CellDynamics(CellBase *c, double *dchem) { 
  // add biochemical networks for intracellular reactions here
}


//Q_EXPORT_PLUGIN2(tutorial1C, Tutorial1C)
