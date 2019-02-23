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
#include <QGridLayout>

using namespace  std; 

static const std::string _module_id("$Id$");

OptionFileDialog::OptionFileDialog(QWidget *parent, const char *name, bool modal) : QFileDialog(parent, QString(name), QString("."), QString("*"))
{
    // in order to derive from the QFileDialog, we need the system to use Qt's dialog
    setOption(QFileDialog::DontUseNativeDialog);
   geometrycheck = new QCheckBox("geometry",this);
  geometrycheck -> setCheckState(Qt::Checked);

  parcheck = new QCheckBox("parameters", this);
  parcheck -> setCheckState(Qt::Checked);

  //addToolButton(geometrycheck);
  //addToolButton(parcheck);
    QGridLayout *layout = (QGridLayout*)this->layout();
    if (layout) {
    int row=layout->rowCount();
    
    layout->addWidget(geometrycheck, row,0);
    layout->addWidget(parcheck, row,1);
    }
    };

OptionFileDialog::OptionFileDialog ( const QString & dirName, const QString & filter ,
				     QWidget * parent, const char * name , bool modal  ) :
  QFileDialog(this, name, dirName, filter) {

  
};

/* finis */
