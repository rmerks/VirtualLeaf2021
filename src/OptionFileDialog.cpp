/*
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

#include <string>
#include "OptionFileDialog.h"
#include <QCheckBox>
#include <iostream>
using namespace  std; 

static const std::string _module_id("$Id$");

OptionFileDialog::OptionFileDialog(QWidget *parent, const char *name, bool modal) : Q3FileDialog(parent, name, modal)
{
  //cerr << "This is an OptionFileDialog\n";
  geometrycheck = new QCheckBox("geometry",this);
  geometrycheck -> setCheckState(Qt::Checked);

  parcheck = new QCheckBox("parameters", this);
  parcheck -> setCheckState(Qt::Checked);

  addToolButton(geometrycheck);
  addToolButton(parcheck);
};

OptionFileDialog::OptionFileDialog ( const QString & dirName, const QString & filter , 
				     QWidget * parent, const char * name , bool modal  ) :
  Q3FileDialog(dirName, filter, parent, name, modal) {

  cerr << "This is an OptionFileDialog\n";
};

/* finis */
