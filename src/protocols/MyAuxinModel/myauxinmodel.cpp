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
#include <QDebug>
#include "simplugin.h"
#include "parameter.h"
#include "wallbase.h"
#include "cellbase.h"
#include "myauxinmodel.h"
#include "flux_function.h"
#include "math.h"
#include "random.h"

QString MyAuxinModel::ModelID(void) {
  // specify the name of your model here
  return QString( "PIN1 recycling" );
}

// return the number of chemicals your model uses
int MyAuxinModel::NChem(void) { return 2; }

// To be executed after cell division
void MyAuxinModel::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
  // rules to be executed after cell division go here
  // (e.g., cell differentiation rules)
}

void MyAuxinModel::SetCellColor(CellBase *c, QColor *color) { 
  // add cell coloring rules here

  double colour;

  colour = c->Chemical(1) / (1.0 + c->Chemical(1));
  double red = (colour < 0.0) ? 0.0 : (colour > 1.0) ? 1.0 : colour; 

  colour = c->Chemical(0) / (1.0 + c->Chemical(0));
  double green = (colour < 0.0) ? 0.0 : (colour > 1.0) ? 1.0 : colour; 

  colour = c->Chemical(3) / (1.0 + c->Chemical(3));
  double blue = (colour < 0.0) ? 0.0 : (colour > 1.0) ? 1.0 : colour;

  if ((red < 0 || red > 1) || (green < 0 || green > 1) || (blue < 0 || blue > 1))
    qDebug() << "COLORS OUT OF RANGE: red: " << red << ", green: " << green << ", blue: " << blue  << endl;

  color->setRgbF(red, green, blue);
}

void MyAuxinModel::CellHouseKeeping(CellBase *c) {
  // add cell behavioral rules here
}

void MyAuxinModel::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
  for (int c=0; c<NChem(); c++) {
    
    // diffusive transport
    double phi = w->Length() * ( par->D[c] ) * 
      ( w->C2()->Chemical(c) - w->C1()->Chemical(c) );
    dchem_c1[c] += phi;
    dchem_c2[c] -= phi;

  }

  // active transport

  // efflux from cell 1 to cell 2
  double trans12 = ( par->transport * w->Transporters1(1) * 
		     w->C1()->Chemical(0) / (par->ka + w->C1()->Chemical(0)) );

  // efflux from cell 2 to cell 1
  double trans21 = ( par->transport * w->Transporters2(1) * 
		     w->C2()->Chemical(0) / (par->ka + w->C2()->Chemical(0)) );

  dchem_c1[0] += trans21 - trans12;
  dchem_c2[0] += trans12 - trans21;

  // Influx at leaf "AuxinSource" (as specified in initial condition)
  if (w->AuxinSource()) { // test if wall is auxin source
    double aux_flux = par->leaf_tip_source * w->Length();
    dchem_c1[0] += aux_flux;
    dchem_c2[0] += aux_flux;
  }
}


void MyAuxinModel::WallDynamics(Wall *w, double *dw1, double *dw2){
  // add biochemical networks for reactions occurring at 
  // walls here
  dw1[0] = 0.; dw2[0] = 0.; // chemical 0 unused in walls
  dw1[1] = PINflux(w->C1(),w->C2(), w);
  dw2[1] = PINflux(w->C2(),w->C1(), w);
}


void MyAuxinModel::CellDynamics(CellBase *c, double *dchem) { 
  // add biochemical networks for intracellular reactions here
  // sum all incoming fluxes of PINs
  dchem[1] = - SumFluxFromWalls( c, MyAuxinModel::PINflux ) 
    //  Auxin-dependent production of PINs
    + par->pin_prod * c->Chemical(0) 
    // Breakdown of PIN
    - par->pin_breakdown * c->Chemical(1);
}


double MyAuxinModel::PINflux(CellBase *this_cell, CellBase *adjacent_cell, Wall *w) {
  // calculate PIN translocation rate from cell to membrane
  double adj_auxin =  adjacent_cell->Chemical(0);
  double receptor_level = adj_auxin * par->r / (par->kr + adj_auxin);
  double pin_atwall; // pick the correct side of the Wall
  if (w->C1() == this_cell){
    pin_atwall = w->Transporters1(1);
  }
  else{
    pin_atwall=w->Transporters2(1);
  }
  double pin_flux = par->k1 * this_cell->Chemical(1) *  
    receptor_level / ( par->km + this_cell->Chemical(1) ) - par->k2 * w->Transporters1(1);
  return pin_flux;
}

Q_EXPORT_PLUGIN2(myauxinmodel, MyAuxinModel)

// finis
