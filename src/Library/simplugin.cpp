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

#include <string>
#include <QString>
#include "simplugin.h"

static const std::string _module_id("$Id$");

void SimPluginInterface::SetParameters(Parameter *pass_pars)
{
  par = pass_pars; 
}

void SimPluginInterface::SetCellsStaticDatamembers( CellsStaticDatamembers *cells_static_data_members_of_main)
{
  CellBase::static_data_members = cells_static_data_members_of_main;
}

QString SimPluginInterface::DefaultLeafML(void) { return QString(); }

/* finis */
