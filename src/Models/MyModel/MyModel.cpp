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
#include "mymodel.h"
#include "random.h"

static const std::string _module_id("$Id$");

QString MyModel::ModelID(void) {
  // specify the name of your model here
  return QString( "Bubbles" );
}

// return the number of chemicals your model uses
int MyModel::NChem(void) { return 0; }

// To be executed after cell division
void MyModel::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
  // rules to be executed after cell division go here
  // (e.g., cell differentiation rules)
}

void MyModel::SetCellColor(CellBase *c, QColor *color) {
  // add cell coloring rules here
    double redness=c->Area()/(10*c->BaseArea());
    if (redness>1.0) {redness=1.0; }
    //color->setRgbF(redness,0,0);
    color->setHsv(static_cast<int>((1.0-redness)*60.), 222, 222);
}

void MyModel::CellHouseKeeping(CellBase *c) {
  // add cell behavioral rules here

    if (c->Index()==par->i1) {
        c->EnlargeTargetArea(10.);
        if (c->Area()>10*c->BaseArea()) {
            c->SetTargetArea(c->BaseArea());
            // Choose new cell
            int cell=RandomNumber(CellBase::NCells());
            par->i1=cell;
        }
    }
}

void MyModel::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
  // add biochemical transport rules here
}
void MyModel::WallDynamics(Wall *w, double *dw1, double *dw2) {
  // add biochemical networks for reactions occuring at walls here
}
void MyModel::CellDynamics(CellBase *c, double *dchem) {
  // add biochemical networks for intracellular reactions here
}


//Q_EXPORT_PLUGIN2(MyModel, MyModel)
