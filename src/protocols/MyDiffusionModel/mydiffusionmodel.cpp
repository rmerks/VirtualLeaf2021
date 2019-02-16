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
#include "mydiffusionmodel.h"

QString Mydiffusionmodel::ModelID(void) {
  // specify the name of your model here
  return QString( "Prevent vascular cells from expanding" );
}

// return the number of chemicals your model uses
int Mydiffusionmodel::NChem(void) { return 4; }

// To be executed after cell division
void Mydiffusionmodel::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
  // rules to be executed after cell division go here
  // (e.g., cell differentiation rules)
}

void Mydiffusionmodel::SetCellColor(CellBase *c, QColor *color) { 
  // add cell coloring rules here
  double red=c->Chemical(1)/(1.+c->Chemical(1));
  double green=c->Chemical(0)/(1.+c->Chemical(0));
  double blue=c->Chemical(3)/(1.+c->Chemical(3));
  color->setRgbF(red,green,blue);
}

void Mydiffusionmodel::CellHouseKeeping(CellBase *c) {
  // add cell behavioral rules here

  if (c->Chemical(0) < 0.5){
    c->EnlargeTargetArea(par->cell_expansion_rate);
  }

  
  if(c->Area() > 2*c->BaseArea()){
    c->Divide();
  }
}

void Mydiffusionmodel::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
  // add biochemical transport rules here

  // Passive fluxes (Fick's law)
  for (int c=0;c<NChem();c++) {
    if (w->C1()->BoundaryPolP() || w->C2()->BoundaryPolP()) return;
    double phi = w->Length() * ( par->D[c] ) * ( w->C2()->Chemical(c) - w->C1()->Chemical(c) );
    dchem_c1[c] += phi;
    dchem_c2[c] -= phi;
  }
}

void Mydiffusionmodel::WallDynamics(Wall *w, double *dw1, double *dw2) {
  // add biochemical networks for reactions occuring at walls here
}

void Mydiffusionmodel::CellDynamics(CellBase *c, double *dchem) {
  // add biochemical networks for intracellular reactions here
  double Y = c->Chemical(0), 
         A = c->Chemical(1), 
         H = c->Chemical(2), 
         S = c->Chemical(3);

  dchem[0] = ( par->d * A - par->e * Y + Y*Y/(1 + par->f * Y*Y ) );
  dchem[1] = ( par->c * A*A*S/H - par->mu * A + par->rho0*Y );
  dchem[2] = ( par->c * A*A*S - par->nu*H + par->rho1*Y );
  dchem[3] = ( par->c0 - par->gamma*S - par->eps * Y * S );
}


Q_EXPORT_PLUGIN2(mydiffusionmodel, Mydiffusionmodel)

// finis
