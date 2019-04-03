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
#include <QFile>

#include "parameter.h"
#include "vleafmodel.h"
#include "wallbase.h"
#include "cellbase.h"
#include "Sprinzak.h"
#include "pi.h"
#include "random.h"


static const std::string _module_id("$Id$");

QString Sprinzak::ModelID(void) {
  // specify the name of your model here
  return QString( "Sprinzak" );
}

// return the number of chemicals your model uses
//int Sprinzak::NChem(void) { return 3; }

// To be executed after cell division
void Sprinzak::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
  // rules to be executed after cell division go here
  // (e.g., cell differentiation rules)
}


void Sprinzak::SetCellColor(CellBase *c, QColor *color) {
  // add cell coloring rules here
    // Red: Delta
    // Green: Notch
  //  if (*c < 255){
  //   *c = 255;
  //  }

    color->setRgb(c->Chemical(0)/(1+c->Chemical(0)) * 255.,(c->Chemical(1)/(1+c->Chemical(1)) * 255.), 0);
   // color->setRgbF(c->Chemical(1)/(1+c->Chemical(1)),(c->Chemical(0)/(1+c->Chemical(0))), 0);
    }


void Sprinzak::CellHouseKeeping(CellBase *c) {
  // add cell behavioral rules here

}

void Sprinzak::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
  // add biochemical transport rules here
    // No flux boundaries for all chemicals, except activator: boundary is sink
  //  if (w->C1()->BoundaryPolP() || w->C2()->BoundaryPolP()) {
   // return;
   // }
    // Passive fluxes (Fick's law)
   // for (int c=0;c<NChem();c++) {
    //double phi = w->Length() * ( par->D[c] ) * ( w->C2()->Chemical(c) - w->C1()->Chemical(c) );
    //dchem_c1[c] += phi;
    //dchem_c2[c] -= phi;
    //}

}

 double Sprinzak::NOTCHflux(CellBase *this_cell, CellBase *adjacent_cell, Wall *w) {

    // calculate PIN translocation rate from cell to membrane
//  double  num_cell = list<CellBase *> neighbors;
//  double num_cell_adj = CellBase::neighbors.size();
   double Notch_adj;
           if (adjacent_cell -> BoundaryPolP()){
         Notch_adj = this_cell->Chemical(0);
     }
            else {
       Notch_adj =  adjacent_cell->Chemical(0);
   }
        return Notch_adj;



}

double Sprinzak::DELTAflux(CellBase *this_cell, CellBase *adjacent_cell, Wall *w) {

       // calculate PIN translocation rate from cell to membrane
      double Delta_adj;
              if (adjacent_cell -> BoundaryPolP()){
            Delta_adj = this_cell->Chemical(1);
        }
               else {
          Delta_adj =  adjacent_cell->Chemical(1);
      }
           return Delta_adj;
}

void Sprinzak::WallDynamics(Wall *w, double *dw1, double *dw2) {
for (int c = 0;c<NChem();c++) {
dw1[c] = 0.; dw2[c] = 0.;
}
}

void Sprinzak::CellDynamics(CellBase *c, double *dchem) {
double N = c->Chemical(0);
double D = c->Chemical(1);
double R = c->Chemical(2);
double N_adj = SumFluxFromWalls( c, Sprinzak::NOTCHflux)/6;
double D_adj = SumFluxFromWalls( c, Sprinzak::DELTAflux)/6;
static double krs2 = pow(par -> krs, par -> nu);


//dchem[0] = ( par->betaN - N - (N * D_adj) - N*D/(par ->kc) );
//dchem[1] = ( par->betaD*(1/(1+R))-D- (N_adj * D) - N*D/par->kc );
//dchem[2] = ( par-> betaR * (N*D_adj)/(par -> krs - N*D_adj) - R);
dchem[0] = ( par->betaN - N - (N * D_adj) - N*D/(par ->kc) );
dchem[1] = ( par->betaD*(1/(1+pow((R), par->mu)))-D- (N_adj * D) - N*D/(par->kc) );
dchem[2] = ( par-> betaR * (pow((N*D_adj), par->nu))/(krs2 - pow(N*D_adj, par->nu)) - R);
dchem[3]=0;

static QFile *of=0;
if (!of) { of = new QFile("outputfile.dat");
of->open (QIODevice::WriteOnly) ;

}

QTextStream output(of);
output << N << " " << D << " "  << dchem[0] << " " << dchem[1] << "\n";
}


//Q_EXPORT_PLUGIN2(Sprinzak, Sprinzak)
