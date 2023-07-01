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


#include <QObject>
#include <QtGui>
#include <QString>
#include "simplugin.h"

#define FIXED_CELL_TYPE 0
#define FLUID_CELL_TYPE 1
#define MESOPHYLL_CELL_TYPE 2
#define EPIDERMIS_CELL_TYPE 3

class Tutorial1C : public QObject, SimPluginInterface {
	Q_OBJECT
	Q_INTERFACES(SimPluginInterface);
     Q_PLUGIN_METADATA(IID "org.virtualleaf.tutorial1c")

	double initialSharedArea = -1.0;
	double lastSharedArea = -1.0;
 	double sharedArea = -1.0;
 	double sharedAreaGrowthFactor = 1.0;
    int cellIndex = -1;
public:
	virtual QString ModelID(void);
	
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
	virtual int NChem(void);
	virtual QString DefaultLeafML(void) { return QString("tutorial1_init.xml"); }
};




