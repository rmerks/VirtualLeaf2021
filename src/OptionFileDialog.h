/*
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

#ifndef _OPTIONFILEDIALOG_H_
#define _OPTIONFILEDIALOG_H_

#include <QFileDialog>
#include <QCheckBox>

class OptionFileDialog : public QFileDialog {
  Q_OBJECT
 public:
  OptionFileDialog(QWidget *parent = 0, const char *name = 0, bool modal = false);
  OptionFileDialog ( const QString & dirName, const QString & filter = QString(), 
		     QWidget * parent = 0, const char * name = 0, bool modal = false );

  bool readGeometryP(void) const { return geometrycheck->checkState()==Qt::Checked; }
  bool readParametersP(void) const { return parcheck->checkState()==Qt::Checked; }

 private:
  QCheckBox *geometrycheck;	
  QCheckBox *parcheck;
};

#endif

/* finis */
