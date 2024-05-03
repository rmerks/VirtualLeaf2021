/*
 *  $Id$
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

#ifndef _AUXINGROWTHPLUGIN_H_
#define _AUXINGROWTHPLUGIN_H_

#include <QObject>
#include <QtGui>
#include <QString>
#include "simplugin.h"


class AuxinGrowthPlugin : public QObject, SimPluginInterface {
  Q_OBJECT
  Q_INTERFACES(SimPluginInterface);
    Q_PLUGIN_METADATA(IID "org.virtualleaf.auxingrowthplugin")


 public:
  virtual QString ModelID(void) { return QString( "Vein growth" ); }
	
  // Executed after the cellular mechanics steps have equillibrized
  virtual void CellHouseKeeping (CellBase *c);
  // Differential equations describing transport of chemicals from cell to cell
  virtual void CelltoCellTransport(Wall *w, double *dchem_c1, double *dchem_c2);
    
  // Differential equations describing chemical reactions taking place at or near the cell walls
  // (e.g. PIN accumulation)
  virtual void WallDynamics(Wall *w, double *dw1, double *dw2);
	
  // Differential equations describing chemical reactions inside the cells
  virtual void CellDynamics(CellBase *c, double *dchem);
	
  // to be executed after a cell division
  virtual void OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2);
	
  // to be executed for coloring a cell
  virtual void SetCellColor(CellBase *c, QColor *color);	
  // return number of chemicals
  virtual int NChem(void) { return 2; }

  virtual QString DefaultLeafML(void) { return QString("vein_growth.xml"); }
 
 private:
  double complex_PijAj(CellBase *here, CellBase *nb, Wall *w);
};

#endif

/* finis */
