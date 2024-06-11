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

#include "lateralRoot.h"

#include <QObject>
#include <QtGui>


#include "parameter.h"

#include "wallbase.h"
#include "cellbase.h"
#include "random.h"

static const std::string _module_id("$Id$");

QString LateralRoot::ModelID(void) {
  // specify the name of your model here
  return QString( "Lateral Root Growth" );
}




// return the number of chemicals your model uses
int LateralRoot::NChem(void) { return 1; }

// To be executed after cell division
void LateralRoot::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
  // rules to be executed after cell division go here
  // (e.g., cell differentiation rules)
//	int count1 = 0;
//	daughter1->LoopNeighbors([&count1](auto neighbor){
//            if (neighbor->CellType() != 3) {
//                count1++;
//            }
//    });
//	int count2 = 0;
//	daughter2->LoopNeighbors([&count2](auto neighbor){
//            if (neighbor->CellType() != 3) {
//                count2++;
//            }
//    });
	if (daughter1->CellType()==3) {
		double condition = daughter1->Centroid().x < daughter2->Centroid().x;
//		Vector horizontal(1,0,0);
//		double size = daughter1->Length(&horizontal, NULL);
//		if (abs(daughter1->Centroid().x - daughter2->Centroid().x) <(size/5)){
//			condition=count2>count1;
//		}
		if (  condition ) {

		daughter1->SetChemical(0, 0.49);
		daughter2->SetChemical(0, 1.);
	}else {
		daughter2->SetChemical(0, 0.49);
		daughter1->SetChemical(0, 1.);
	}
	}
}

void LateralRoot::SetCellColor(CellBase *c, QColor *color) { 
  // add cell coloring rules here
	 if (c->CellType()==3){
		color->setNamedColor("green");
	}
	else {
		color->setNamedColor("blue");
	}

}

void LateralRoot::CellHouseKeeping(CellBase *c) {
  // add cell behavioral rules here
	if (c->CellType()==3) {
		if (c->Chemical(0)>0.5||c->Chemical(0)<0.001){
			c->EnlargeTargetArea(par->cell_expansion_rate);
			if (c->Area() > par->rel_cell_div_threshold * c->BaseArea()) {
				c->Divide();
			}
		}
//		if (c->Chemical(0)>0.4){
//			c->SetChemical(0,c->Chemical(0)-0.01);
//			c->EnlargeTargetArea(par->cell_expansion_rate);
//		}
	}
	}

void LateralRoot::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
  // add biochemical transport rules here
}
void LateralRoot::WallDynamics(Wall *w, double *dw1, double *dw2) {
  // add biochemical networks for reactions occuring at walls here
}
void LateralRoot::CellDynamics(CellBase *c, double *dchem) { 
  // add biochemical networks for intracellular reactions here
}


//Q_EXPORT_PLUGIN2(LateralRoot, LateralRoot)
