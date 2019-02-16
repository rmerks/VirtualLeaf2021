/*
 *
 *  $Id$
 *
 *  This file is part of the Virtual Leaf.
 *
 *  VirtualLeaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VirtualLeaf is distributed in the hope that it will be useful,
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


#ifndef _SIMPLUGIN_H_
#define _SIMPLUGIN_H_

#include <QtPlugin>
#include <QMetaType>
#include "cellbase.h"
#include "wallbase.h"

class Parameter;

#include <QColor>
#include <QString>


class SimPluginInterface {

 public:
  virtual QString ModelID(void) = 0; 

  virtual ~SimPluginInterface() { }

  // Executed after the cellular mechanics steps have equillibrized
  virtual void CellHouseKeeping(CellBase *c) = 0;

  // Differential equations describing transport of chemicals from cell to cell
  virtual void CelltoCellTransport(Wall *, double *dchem_c1, double *dchem_c2) = 0;

  // Differential equations describing chemical reactions taking place at or near the cell walls
  // (e.g. PIN accumulation)
  virtual void WallDynamics(Wall *w, double *dw1, double *dw)  = 0;

  // Differential equations describing chemical reactions inside the cells
  virtual void CellDynamics(CellBase *c, double *dchem) = 0;

  // to be executed after a cell division
  virtual void OnDivide(ParentInfo *parent_info, CellBase *daughter1, CellBase *daughter2) = 0;

  // to be executed for coloring a cell
  virtual void SetCellColor(CellBase *c, QColor *color) = 0;

  // Number of chemicals
  virtual int NChem(void) = 0;

  // Default LeafML-file to be read after model startup
  virtual QString DefaultLeafML(void);
  
  // For internal use; not to be redefined by end users
  virtual void SetParameters(Parameter *pass_pars);// { par = pass_pars; }
  virtual void SetCellsStaticDatamembers (CellsStaticDatamembers *cells_static_data_members_of_main);

 protected:
  class Parameter *par;
};

Q_DECLARE_INTERFACE(SimPluginInterface, "nl.cwi.VirtualLeaf.SimPluginInterface/1.3") 
Q_DECLARE_METATYPE(SimPluginInterface *)

#endif

/* finis */
