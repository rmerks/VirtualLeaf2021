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

#include "simplugin.h"

#include "parameter.h"

#include "wallbase.h"
#include "cellbase.h"
#include "tutorial3.h"

static const std::string _module_id("$Id$");

QString Tutorial3::ModelID(void) {
  // specify the name of your model here
  return QString( "3: Directed phytohormone transport" );
}

// return the number of chemicals your model uses
int Tutorial3::NChem(void) { return 2; }

// To be executed after cell division
void Tutorial3::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
  // rules to be executed after cell division go here
  // (e.g., cell differentiation rules)
	
	// set one cell to source after first division
	if (CellBase::NCells()==2) {
		daughter1->SetCellType(1);
		daughter2->SetCellType(0);
	}
	
	// if a source cells has divided, one of the daughters becomes the new source
	if (daughter1->CellType()==1) {
		
		// if both cells are at the tissue perimeter, choose at random
		if (daughter1->AtBoundaryP() && daughter2->AtBoundaryP()) {
		
			if (qrand()%2){
				daughter1->SetCellType(1);
				daughter2->SetCellType(0);
			} else {
				daughter1->SetCellType(0);
				daughter2->SetCellType(1);
			}
		} else {
			// otherwise choose the one that is still at the perimeter
			if (daughter1->AtBoundaryP()) {
				daughter1->SetCellType(1);
				daughter2->SetCellType(0);
			} else {
				daughter1->SetCellType(0);
				daughter2->SetCellType(1);
			}
		}
	}
	
}

void Tutorial3::SetCellColor(CellBase *c, QColor *color) { 
  // add cell coloring rules here
	
	// white: high concentration of growth hormone, black low concentration
	double val = 1.-c->Chemical(0)/(1.+c->Chemical(0));
	color->setRgbF(val, val, val);
}

void Tutorial3::CellHouseKeeping(CellBase *c) {
  // add cell behavioral rules here
	if (CellBase::NCells()==1)
		// first cell expands unconditionally 
		c->EnlargeTargetArea(par->cell_expansion_rate);
	else 
		c->EnlargeTargetArea(c->Chemical(0)*par->cell_expansion_rate);
	
	if (c->Area() > par->rel_cell_div_threshold * c->BaseArea()) {
		c->Divide();
	}
}

void Tutorial3::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
  // add biochemical transport rules here
	double phi = w->Length() * par->D[0] * ( w->C2()->Chemical(0) - w->C1()->Chemical(0) );
	dchem_c1[0]+=phi;
	dchem_c2[0]-=phi;
	
	// directed transport
	// efflux from cell 1 to cell 2
  	double trans12 = ( par->transport * w->Transporters1(1) * 
					  w->C1()->Chemical(0) / (par->ka + w->C1()->Chemical(0)) );
	
    // efflux from cell 2 to cell 1
    double trans21 = ( par->transport * w->Transporters2(1) * 
					  w->C2()->Chemical(0) / (par->ka + w->C2()->Chemical(0)) );
    
    dchem_c1[0] += trans21 - trans12;
    dchem_c2[0] += trans12 - trans21;
	
}

void Tutorial3::WallDynamics(Wall *w, double *dw1, double *dw2) {
  // add biochemical networks for reactions occuring at walls here
}
void Tutorial3::CellDynamics(CellBase *c, double *dchem) { 
  // add biochemical networks for intracellular reactions here
	if (c->CellType()==1) {
		dchem[0] = par->leaf_tip_source;
	}
}
