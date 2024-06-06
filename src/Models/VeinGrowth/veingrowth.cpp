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
#include <vector>

#include "parameter.h"

#include "wallbase.h"
#include "cellbase.h"
#include "veingrowth.h"

#include "far_mem_5.h"

static const std::string _module_id("$Id$");

/*class blabla {
    Vector growth_direction;
    bool is_growth_direction;
};*/


// To be executed after cell division
void VeinGrowthPlugin::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2)
{
    // After division, the growth direction of the parent cell (if it exists) is inherited by the daugther cells
    if (daughter2->CellType() == 1 && daughter2->isGrowthDirection()) {
        Vector growth_direction = daughter2->GetGrowthDirection();
        daughter1->SetGrowthDirection(growth_direction.x, growth_direction.y);
    }

    // Auxin distributes between parent and daughter according to area
    double area1 = daughter1->Area(), area2 = daughter2->Area();
    double tot_area = area1 + area2;

    daughter1->SetChemical(0,daughter1->Chemical(0)*(area1/tot_area));
    daughter2->SetChemical(0,daughter2->Chemical(0)*(area2/tot_area));

    // After division, the growth direction of the parent cell (if it exists) is inherited by the daugther cells
    // The growth direction is saved in the chemicals: Chemical[2]= x-direction, Chemical[3] = y-direction

    // After divisions, parent and daughter cells get a standard stock of PINs.
    //daughter1->SetChemical(1, par->initval[1]);
    //cout << daughter1->Chemical(1) << endl;
    //daughter2->SetChemical(1, par->initval[1]);

    // Only cell types 2 get standard stock of PINs
    if (daughter1->CellType() == 2) {
        daughter1->SetChemical(1, par->initval[1]);
    } /*else if (daughter1->CellType() == 1 || daughter1->CellType() == 3) {
        daughter1->SetChemical(1, par->initval[1] / 4);
    }*/

    if (daughter2->CellType() == 2) {
        daughter2->SetChemical(1, par->initval[1]);
    }/* else if (daughter2->CellType() == 1 || daughter1->CellType() == 3) {
        daughter1->SetChemical(1, par->initval[1] / 4);
    }*/


    // Reset transporter values of parent and daughter
    QList<WallBase *> walls;
    foreach(WallBase *w, walls) {
        w->setTransporter(daughter1, 1, 0.);
    }
}

void VeinGrowthPlugin::SetCellColor(CellBase *c, QColor *color)
{

    // Red: PIN1
    // Green: Auxin
    color->setRgb(c->Chemical(1)/(1+c->Chemical(1)) * 255.,(c->Chemical(0)/(1+c->Chemical(0)) * 255.), 0);
}



void VeinGrowthPlugin::CellHouseKeeping(CellBase *c)
{
    if (c->Boundary()==CellBase::None) {
        //if (c->div)
        if (c->Chemical(0) > 1.9 && c->CellType() != 1) {
             c->SetCellType(1);

            // inherit growth direction
            double x_value = 0;
            double y_value = 0;
            c->LoopNeighbors([&x_value, &y_value](auto neighbor){
                if (neighbor->CellType() == 1) {
                    if (neighbor->isGrowthDirection()) {
                        Vector neighbor_growth_direction = neighbor->GetGrowthDirection();
                        x_value += neighbor_growth_direction.x;
                        y_value += neighbor_growth_direction.y;
                    }

                    /*if (!(std::isnan(neighbor->Chemical(2)))) {
                        x_value += neighbor_growth_direction.x;
                    }

                    if (!(std::isnan(neighbor->Chemical(3)))) {
                        y_value += neighbor_growth_direction.y;
                    }*/
                }
            });

            // reduce values -> capacity?
            c->SetGrowthDirection(x_value, y_value);
            /*c->SetChemical(2, x_value / ((x_value + y_value) / 2));
            c->SetChemical(3, y_value/* / ((x_value + y_value) / 2));*/
        }

        if (c->CellType() == 1) {
            // set growth direction if available, if not then compute it
            Vector growth_direction = Vector(0 ,1 ,0);
            if (c->CountNeighbors() > 1 ) {
                //if (std::isnan(c->Chemical(2)) && std::isnan(c->Chemical(3))) {
                //if (c->Chemical(2) == 0 && c->Chemical(3) == 0) {
                if (!c->isGrowthDirection()) {
                    double highest_auxin = 0;
                    CellBase *target_cell = NULL;
                    c->LoopNeighbors([&highest_auxin, &target_cell](auto neighbor){
                        if (highest_auxin < neighbor->Chemical(0) && neighbor->CellType() != 1) {
                            highest_auxin = neighbor->Chemical(0);
                            target_cell = neighbor;
                        }
                    });
                    if (target_cell != NULL) {
                        growth_direction = c->Centroid() - target_cell->Centroid();
                        //growth_direction = Vector(target_cell->x - c->x, target_cell->y - c->y, 0);
                        c->SetGrowthDirection(growth_direction.x, growth_direction.y);
                        //c->SetChemical(2, growth_direction.x);
                        //c->SetChemical(3, growth_direction.y);
                    } else {
                        growth_direction = Vector(0, 1, 0);
                        cout << "Error when evaluation neighbours in household!" << endl;
                    }

                    //cout << c->NChem() << endl;
                    //growth_direction = new Vector(0, 1, 0);
                } else {
                    growth_direction = c->GetGrowthDirection();
                }
            }/* else {
            growth_direction = Vector(0 ,1 ,0);
        }*/

            // wall stiffness manipulation
            c->LoopWallElements([&growth_direction](auto wallElementInfo){
                if(wallElementInfo->hasWallElement()){
                    //Vector* y = new Vector(0, 1, 0);
                    Vector* we_direction = new Vector(wallElementInfo->getTo()->x - wallElementInfo->getFrom()->x, wallElementInfo->getTo()->y - wallElementInfo->getFrom()->y, 0);
                    double we_angle = (we_direction->Angle(growth_direction))*180/3.1415926536;
                    /*if (direction->x == 0) {
                    angle = 90;
                } else {
                    angle = atan(direction->y/direction->x)*180/3.1415926536;
                }*/
                    if (we_angle <= 30 || we_angle >= 150) {
                        WallElement* we = wallElementInfo->getWallElement();
                        we->setStiffness(4);
                    } /*else if (we_angle >= 75 || we_angle <=105){
                    WallElement* we = wallElementInfo->getWallElement();
                    we->setStiffness(1);
                }*/
                }
            });
        }

        /*        c->LoopWallElements([](auto wallElementInfo){
            if(wallElementInfo->hasWallElement()){
                Vector direction = new Vector(wallElementInfo->getTo()->x - wallElementInfo->getFrom()->x, )
                if (std::fabs(wallElementInfo->getTo()->x - wallElementInfo->getFrom()->x) < 5) {
                    WallElement* we = wallElementInfo->getWallElement();
                    we->setStiffness(4);
                } else {
                    WallElement* we = wallElementInfo->getWallElement();
                    we->setStiffness(1);
                }
            }
        });*/


        if (c->Area() > par->rel_cell_div_threshold * c->BaseArea() ) {
            //c->SetChemical(0, 0);
            c->Divide();
            //Vector* v = new Vector(1,0,0);
            //c->DivideOverAxis(*v);
        }

        //WallElementInfo info;
        //info.getFrom()->x;
        //info.getTo()->x;

        //cout << we->getBaseLength() << endl;

        // expand according to auxin concentration
        c->EnlargeTargetArea(par->auxin_dependent_growth?(c->Chemical(0)/(1.+c->Chemical(0)))*par->cell_expansion_rate:par->cell_expansion_rate);
    }
}

void VeinGrowthPlugin::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2)
{

    // leaf edge is const source of auxin
    // (Neumann boundary condition: we specify the influx)
    if (w->C2()->BoundaryPolP()) {
        if (w->AuxinSource()) {
            double aux_flux = par->leaf_tip_source * w->Length();
            dchem_c1[0]+= aux_flux;
            return;
        } else {
            return;
        }
    }

    if (w->C1()->BoundaryPolP()) {

        if (w->AuxinSource()) {
            double aux_flux = par->leaf_tip_source * w->Length();
            dchem_c2[0] += aux_flux;
            return;
        } else {

            if (w->AuxinSink()) {

                // efflux into Shoot Apical meristem
                // we assume all PINs are directed towards shoot apical meristem
                dchem_c2[0] -= par->sam_efflux * w->C2()->Chemical(0) / (par->ka + w->C2()->Chemical(0));

                return;
            } else
                return;
        }
    }


    // Passive fluxes (Fick's law)
    // only auxin flux now
    // flux depends on edge length and concentration difference
    int c=0;
    double phi = w->Length() * ( par->D[c] ) * ( w->C2()->Chemical(c) - w->C1()->Chemical(c) );
    dchem_c1[c] += phi;
    dchem_c2[c] -= phi;

    // Active fluxes (PIN1 mediated transport)

    // (Transporters measured in moles, here)
    // efflux from cell 1 to cell 2
    double trans12 = ( par->transport * w->Transporters1(1) * w->C1()->Chemical(0) / (par->ka + w->C1()->Chemical(0)) );

    // efflux from cell 2 to cell 1
    double trans21 = ( par->transport * w->Transporters2(1) * w->C2()->Chemical(0) / (par->ka + w->C2()->Chemical(0)) );

    dchem_c1[0] += trans21 - trans12;
    dchem_c2[0] += trans12 - trans21;

}

void VeinGrowthPlugin::WallDynamics(Wall *w, double *dw1, double *dw2)
{
    // Cells polarize available PIN1 to Shoot Apical Meristem
    if (w->C2()->BoundaryPolP()) {
        if (w->AuxinSink()) {

            dw1[0] = 0.; dw2[0] = 0.;

            // assume high auxin concentration in SAM, to convince PIN1 to polarize to it
            // exocytosis regulated0
            double nb_auxin = par->sam_auxin;
            double receptor_level = nb_auxin * par->r / (par->kr + nb_auxin);

            dw1[1] = par->k1 * w->C1()->Chemical(1) * receptor_level /( par->km + w->C1()->Chemical(1) ) - par->k2 * w->Transporters1(1);

            dw2[1] = 0.;
            return;

        } else {
            dw1[0]=dw2[0]=dw1[1]=dw2[1];
            return;
        }
    }

    if (w->C1()->BoundaryPolP()) {
        if (w->AuxinSink())  {

            dw1[0] = 0.; dw2[0] = 0.;

            // assume high auxin concentration in SAM, to convince PIN1 to polarize to it
            // exocytosis regulated
            double nb_auxin = par->sam_auxin;
            double receptor_level = nb_auxin * par->r / (par->kr + nb_auxin);
            dw2[1] = par->k1 * w->C2()->Chemical(1) * receptor_level /( par->km + w->C2()->Chemical(1) ) - par->k2 * w->Transporters2(1);

            dw1[1] = 0.;
            return;

        }  else {
            dw1[0]=dw2[0]=dw1[1]=dw2[1];
            return;
        }
    }



    // PIN1 localization at wall 1
    // Note: chemical 0 is Auxin (intracellular storage only)
    // Chemical 1 is PIN1 (walls and intracellular storage)
    //! \f$ \frac{d Pij/dt}{dt} = k_1 A_j \frac{P_i}{L_ij} - k_2 P_{ij} \f$
    // Note that Pij is measured in term of concentration (mol/L)
    // Pi in terms of quantity (mol)

    double dPijdt1=0., dPijdt2=0.;

    // normal cell
    double  auxin2 = w->C2()->Chemical(0);
    double receptor_level1 = auxin2 * par->r / (par->kr + auxin2);

    dPijdt1 =
        // exocytosis regulated
        par->k1 * w->C1()->Chemical(1) * receptor_level1 / ( par->km + w->C1()->Chemical(1) ) - par->k2 * w->Transporters1(1);

    double  auxin1 = w->C1()->Chemical(0);
    double receptor_level2 = auxin1 * par->r / (par->kr + auxin1);

    // normal cell
    dPijdt2 =

            // exocytosis regulated
            par->k1 * w->C2()->Chemical(1) * receptor_level2 / ( par->km + w->C2()->Chemical(1) ) - par->k2 * w->Transporters2(1);

    /* PIN1 of neighboring vascular cell inhibits PIN1 endocytosis */

    dw1[0] = 0.; dw2[0] = 0.;
    dw1[1] = dPijdt1;
    dw2[1] = dPijdt2;
}


double VeinGrowthPlugin::complex_PijAj(CellBase *here, CellBase *nb, Wall *w)
{
    // gives the amount of complex "auxinreceptor-Pin1"  at the wall (at QSS)
    //return here.Chemical(1) * nb.Chemical(0) / ( par->km + here.Chemical(1));

    double nb_aux = (nb->BoundaryPolP() && w->AuxinSink()) ? par->sam_auxin : nb->Chemical(0);
    double receptor_level = nb_aux * par->r / (par->kr + nb_aux);

    return here->Chemical(1) * receptor_level / ( par->km + here->Chemical(1));

}


void VeinGrowthPlugin::CellDynamics(CellBase *c, double *dchem)
{
    // Note: Pi and Pij measured in numbers of molecules, not concentrations
    double dPidt = 0.;

    if (std::isnan(c->SumTransporters( 1 ))) {
        double sum_Pij = c->SumTransporters( 1 );
    }

    double sum_Pij = c->SumTransporters( 1 );

    // exocytosis regulated:

    dPidt = -par->k1 * c->ReduceCellAndWalls<double>( far_3_arg_mem_fun( *this, &VeinGrowthPlugin::complex_PijAj ) ) + par->k2 * sum_Pij;

    // production of PIN depends on auxin concentration
    dPidt +=  (c->AtBoundaryP()?par->pin_prod_in_epidermis:par->pin_prod) * c->Chemical(0) - c->Chemical(1) * par->pin_breakdown;

    if (std::isnan(dPidt)) {
        cout << "Break" << endl;
        dPidt = 0.1;
    }

    // no PIN production in SAM
    if (c->Boundary() == CellBase::SAM) {
        dchem[1]=0.;
        dchem[0]= - par->sam_auxin_breakdown * c->Chemical(0);
    } else {
        dchem[1] = dPidt;

        // source of auxin
        dchem[0] = par->aux_cons - par->aux_breakdown * c->Chemical(0);
    }
}
