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
#include "CellTypes.h"
#include "random.h"

static const std::string _module_id("$Id$");

QString CellTypes::ModelID(void) {
  // specify the name of your model here
  return QString( "Cell types" );
}

// return the number of chemicals your model uses
int CellTypes::NChem(void) { return 0; }

// To be executed after cell division
void CellTypes::OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) {
  // rules to be executed after cell division go here
  // (e.g., cell differentiation rules)
}

void CellTypes::SetCellColor(CellBase *c, QColor *color) {
  // add cell coloring rules here
    
    //set cell colour according to cell type (1-10)
  if(c->CellType()==0){
        color->setRgb(255,0,0,255);
  }

  if(c->CellType()==1){
        color->setRgb(0,255,0,255);
  }
  if(c->CellType()==2){
        color->setRgb(0,0,255,255);
  }
  if(c->CellType()==3){
        color->setRgb(255,255,0,255);
  }
  if(c->CellType()==4){
        color->setRgb(255,0,255,255);
  }
  if(c->CellType()==5){
        color->setRgb(0,255,255,255);
  }
  if(c->CellType()==6){
        color->setRgb(255,100,0,255);
  }
  if(c->CellType()==7){
        color->setRgb(0,255,100,255);
  }
  if(c->CellType()==8){
        color->setRgb(100,255,0,255);
  }
  if(c->CellType()==9){
        color->setRgb(100,0,255,255);
  }
}

void CellTypes::CellHouseKeeping(CellBase *c) {
  // add cell behavioral rules here

}

void CellTypes::CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2) {
  // add biochemical transport rules here
}
void CellTypes::WallDynamics(Wall *w, double *dw1, double *dw2) {
  // add biochemical networks for reactions occuring at walls here
}
void CellTypes::CellDynamics(CellBase *c, double *dchem) {
  // add biochemical networks for intracellular reactions here
}


//Q_EXPORT_PLUGIN2(MyModel, MyModel)
