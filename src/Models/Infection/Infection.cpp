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

#include "wallbase.h"
#include "cellbase.h"
#include "Infection.h"
#include "random.h"

static const std::string _module_id("$Id$");

QString Infection::ModelID(void) {
  // specify the name of your model here
  return QString( "Infection" );
}

// return the number of chemicals your model uses
int Infection::NChem(void) { return 2; }

// To be executed after cell division
void Infection::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
	// rules to be executed after cell division go here
	// (e.g., cell differentiation rules)
}

void Infection::SetCellColor(CellBase *c, QColor *color) {
	// add cell coloring rules here
    // Color code for cell types:
    //xylem
    if(c->CellType()==0){
        color->setRgbF(0,1,0,1);
    }
    // procambium
    if(c->CellType()==1){
        color->setRgbF(0,0,1,1);
    }


    double col_inf = c->Chemical(0) / (0.2);
    double col_inf2 = c->Chemical(0) / (0.4);
    if (col_inf > 1.0) {
    	col_inf = 1.0;
    }
    if (col_inf2 > 0.4) {
        col_inf2 = 0.4;
    }
    color->setRgbF(0 + col_inf - col_inf2, 1 - col_inf2, 0);

    //color->setHsv(static_cast<int>((1.0-redness)*60.), 222, 222);


     // pathogenic fungi, which is always red
     if(c->CellType()==2){
         color->setRgbF(1,0,0,1);
     }



}

void Infection::CellHouseKeeping(CellBase *c) {
    // add cell behavioral rules here
    if(c->CellType()==2){
        c->EnlargeTargetArea(2);
    }

    // initial cell length setup
    double base_element_length = 25;
    c->LoopWallElements([base_element_length](auto wallElementInfo){
        if(std::isnan(wallElementInfo->getWallElement()->getBaseLength())){
        wallElementInfo->getWallElement()->setBaseLength(base_element_length);
        }
    });


    //cell wall weakening happens here
    double patho_chem_level = c->Chemical(0) / (0.5);
    if (patho_chem_level > 1.2) {
        patho_chem_level = 1.2;
    }
    double stiffness_inf = 2.5;
    if(patho_chem_level>0.1 && c->CellType()!=2){
        c->SetCellVeto(false);
        stiffness_inf = 2.5 - (patho_chem_level);
    c->LoopWallElements([stiffness_inf](auto wallElementInfo){
        wallElementInfo->getWallElement()->setStiffness(stiffness_inf);
    });
    }
    else{
        c->LoopWallElements([stiffness_inf](auto wallElementInfo){
        wallElementInfo->getWallElement()->setStiffness(stiffness_inf);
        });
        c->SetCellVeto(true);
    }



}

void getLengthAndStiffness(Wall *w,double *plength,double *pstiffness) {
    w->C1()->LoopWallElementsOfWall(w, [plength,pstiffness](auto wallElementInfo){
    	double thisLength = wallElementInfo->getLength();
    	(*plength)=(*plength)+ thisLength;
        (*pstiffness)=(*pstiffness)+ (wallElementInfo->getWallElement()->getStiffness())*thisLength;
    });
    w->C2()->LoopWallElementsOfWall(w, [plength,pstiffness](auto wallElementInfo){
    	double thisLength = wallElementInfo->getLength();
        (*pstiffness)=(*pstiffness)+ (wallElementInfo->getWallElement()->getStiffness())*thisLength;
    });
    (*pstiffness)=(*pstiffness)/(2.0*(*plength));
}

void Infection::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
	// add biochemical transport rules here
    double sum =  w->C1()->Area() + w->C2()->Area();
    double corr1 = w->C2()->Area() / sum;
    double corr2 = w->C1()->Area() / sum;

    double length = 1.0;
    double stiffness = 1.0;
    getLengthAndStiffness(w,&length,&stiffness);
    double diffusionCoef;
    if(stiffness>0.001){
        diffusionCoef = 0.00001/stiffness;
    }
    else{diffusionCoef=0.00001;}

    double phi = length * diffusionCoef * ( w->C2()->Chemical(0) - w->C1()->Chemical(0) );

    dchem_c1[0] += corr1 * phi;
    dchem_c2[0] -= corr2 * phi;

}

void Infection::WallDynamics(Wall *w, double *dw1, double *dw2) {
	// add biochemical networks for reactions occuring at walls here
}

void Infection::CellDynamics(CellBase *c, double *dchem) {
	// add biochemical networks for intracellular reactions here
    if(c->CellType()==2){
        dchem[0] = 0.1;
    }
    // the fungi is assumed to have a constant chemical level
    else{
    // for all other cells, the chemical is degraded:
        dchem[0] = -0.001 * c->Chemical(0);
    }


}


//Q_EXPORT_PLUGIN2(Infection, Infection)
