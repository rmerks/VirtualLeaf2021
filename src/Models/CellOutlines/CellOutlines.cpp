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
#include "CellOutlines.h"
#include "random.h"

static const std::string _module_id("$Id$");

QString CellOutlines::ModelID(void) {
  // specify the name of your model here
  return QString( "Cell outlines" );
}

// return the number of chemicals your model uses
int CellOutlines::NChem(void) { return 0; }

// To be executed after cell division
void CellOutlines::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
  // rules to be executed after cell division go here
  // (e.g., cell differentiation rules)
}

void CellOutlines::SetCellColor(CellBase *c, QColor *color) {
  // add cell coloring rules here
    
    //set cell colour to white
    color->setRgb(255,255,255,255);
}

void CellOutlines::CellHouseKeeping(CellBase *c) {
  // add cell behavioral rules here

}

void CellOutlines::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
  // add biochemical transport rules here
}
void CellOutlines::WallDynamics(Wall *w, double *dw1, double *dw2) {
  // add biochemical networks for reactions occuring at walls here
}
void CellOutlines::CellDynamics(CellBase *c, double *dchem) {
  // add biochemical networks for intracellular reactions here
}


//Q_EXPORT_PLUGIN2(MyModel, MyModel)
