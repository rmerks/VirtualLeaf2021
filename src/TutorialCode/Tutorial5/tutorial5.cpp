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

#include "vleafmodel.h"
#include "tutorial5.h"
		   
static const std::string _module_id("$Id$");

QString Tutorial5::ModelID(void) {
  // specify the name of your model here
  return QString( "5: Auxin-driven growth" );
}

// return the number of chemicals your model uses
int Tutorial5::NChem(void) { return 2; }

// To be executed after cell division
void Tutorial5::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {

	// After divisions, parent and daughter cells get a standard stock of PINs.
	daughter1->SetChemical(1, par->initval[1]);
	daughter2->SetChemical(1, par->initval[1]);
	
	daughter1->SetTransporters(1, 0.);
	daughter2->SetTransporters(1, 0.);

	
}

void Tutorial5::SetCellColor(CellBase *c, QColor *color) { 
  // add cell coloring rules here
	// Red: PIN1
	// Green: Auxin
	color->setRgb(c->Chemical(1)/(1+c->Chemical(1)) * 255.,(c->Chemical(0)/(1+c->Chemical(0)) * 255.), 0);
	
}

void Tutorial5::CellHouseKeeping(CellBase *c) {
  // add cell behavioral rules here
	
	c->EnlargeTargetArea(c->Chemical(0)/(1.+c->Chemical(0))*par->cell_expansion_rate);
	
	if (c->Area() > par->rel_cell_div_threshold * c->BaseArea()) {
		c->Divide();
	}
}

void Tutorial5::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
  // add biochemical transport rules here
	double phi = w->Length() * par->D[0] * ( w->C2()->Chemical(0) - w->C1()->Chemical(0) );
	dchem_c1[0]+=phi;
	dchem_c2[0]-=phi;
	
	// Active fluxes (PIN1 mediated transport)
	
    // (Transporters measured in moles, here)
    // efflux from cell 1 to cell 2
    double trans12 = ( par->transport * w->Transporters1(1) * w->C1()->Chemical(0) / (par->ka + w->C1()->Chemical(0)) );
	
    // efflux from cell 2 to cell 1
    double trans21 = ( par->transport * w->Transporters2(1) * w->C2()->Chemical(0) / (par->ka + w->C2()->Chemical(0)) );
    
    dchem_c1[0] += trans21 - trans12;
    dchem_c2[0] += trans12 - trans21;
	
	// Influx at leaf "AuxinSource" (as specified in initial condition)
	if (w->AuxinSource()) {
		double aux_flux = par->leaf_tip_source * w->Length(); 
		dchem_c1[0] += aux_flux;
		dchem_c2[0] += aux_flux;
	}
}

double Tutorial5::PINflux(CellBase *this_cell, CellBase *adjacent_cell, Wall *w) {
	// PIN1 localization at wall
    // Note: chemical 0 is Auxin (intracellular storage only)
    //  PIN1 is Chemical 1 (both in walls and intracellular storage)
    //! \f$ \frac{d Pij/dt}{dt} = k_1 A_j \frac{P_i}{L_ij} - k_2 P_{ij} \f$
    // Note that Pij is measured in term of concentration (mol/L)
    // Pi in terms of quantity (mol)
	
	// Equations as in Merks et al., Trends in Plant Science 2007
    
    // calculate PIN translocation rate from cell to membrane
	double adj_auxin =  adjacent_cell->Chemical(0);
    double receptor_level = adj_auxin * par->r / (par->kr + adj_auxin);
	double pin_atwall; // pick the correct side of the Wall
	
	if (w->C1() == this_cell) pin_atwall = w->Transporters1(1); 
	else pin_atwall=w->Transporters2(1);
	
	// note: pin_flux is net flux from endosome to wall
	double pin_flux = par->k1 * this_cell->Chemical(1) * receptor_level / ( par->km + this_cell->Chemical(1) ) - par->k2 * pin_atwall;
	return pin_flux;


}


void Tutorial5::WallDynamics(Wall *w, double *dw1, double *dw2) {
  // add biochemical networks for reactions occuring at walls here
    dw1[0] = 0.; dw2[0] = 0.; // chemical 0 unused in walls
	dw1[1] = PINflux(w->C1(),w->C2(),w);
    dw2[1] = PINflux(w->C2(),w->C1(),w);
		
}

void Tutorial5::CellDynamics(CellBase *c, double *dchem) { 
	// add biochemical networks for intracellular reactions here
	
	// sum all incoming fluxes of PINs
	dchem[1] =  - SumFluxFromWalls( c, Tutorial5::PINflux );
	
	// auxin degradation
	dchem[0] = - par->aux_breakdown * c->Chemical(0); 
	
	
}


Q_EXPORT_PLUGIN2(tutorial5, Tutorial5)
