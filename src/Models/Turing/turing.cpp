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
//#include "warning.h"
#include "wallbase.h"
#include "cellbase.h"
#include "turing.h"

static const std::string _module_id("$Id: meinhardtplugin.cpp,v 59e38e5f05d2 2010/06/23 14:21:32 michael $");

bool batch = false;

// To be executed after cell division
void TuringPlugin::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {}

void TuringPlugin::SetCellColor(CellBase *c, QColor *color) { 

  if (fpclassify(c->Chemical(0))==FP_NAN) {
    // somehow the function isnan doesn't work properly on my system... SuSE Linux
    // 10.0 64-bits (isnan seems not be implemented using fpclassify).
    //MyWarning::warning("Whoops! Numerical instability!!");
    color->setNamedColor("red");
  } else {
    double range_min = 0.;//, range_max = 1.;
    if (c->Chemical(0)<range_min) {
      //MyWarning::warning("Whoops! Numerical instability!!");
      color->setNamedColor("blue");
    } else {
      color->setRgb(c->Chemical(0)/(1+c->Chemical(0)) * 255.,c->Chemical(0)/(1+c->Chemical(0)) * 255.,(c->Chemical(1)/(1+c->Chemical(1)) *255.) );
    }
  }
}



void TuringPlugin::CellHouseKeeping(CellBase *c) {
	
  if (c->Area() > par->rel_cell_div_threshold * c->BaseArea() ) {
    c->Divide();
  }
  c->EnlargeTargetArea(par->cell_expansion_rate);
}

void TuringPlugin::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
	
  // No flux boundaries for all chemicals, except activator: boundary is sink
  if (w->C1()->BoundaryPolP() || w->C2()->BoundaryPolP()) {
		
    /*  if (w->C1()->BoundaryPolP()) {
      dchem_c2[1] -=  w->Length() * ( par->D[1] ) * ( w->C2()->Chemical(1) );
    } else {
      dchem_c1[1] -=  w->Length() * ( par->D[1] ) * ( w->C1()->Chemical(1) );
      } */
    return;
  }
	
  

  // Passive fluxes (Fick's law)
  for (int c=0;c<NChem();c++) {
    double phi = w->Length() * ( par->D[c] ) * ( w->C2()->Chemical(c) - w->C1()->Chemical(c) );
    dchem_c1[c] += phi; 
    dchem_c2[c] -= phi;
  }
}

void TuringPlugin::WallDynamics(Wall *w, double *dw1, double *dw2) {
  for (int c = 0;c<NChem();c++) {
    dw1[c] = 0.; dw2[c] = 0.;
  }
}

void TuringPlugin::CellDynamics(CellBase *c, double *dchem) {

  // double Y = c->Chemical(0);
  double A = c->Chemical(0);
  double H = c->Chemical(1);
  //double S = c->Chemical(3);
  
  dchem[0] = ( par->c * A*A/H - par->mu * A + par->rho0 );
  dchem[1] = ( par->c0 * A*A - par->nu*H + par->rho1 );
}
