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


#ifndef _TRANSPORTERDIALOG_H_
#define _TRANSPORTERDIALOG_H_

#include <QDialog>
#include <QVector>
#include <QLineEdit>

class QLabel;
class QLineEdit;

class Wall;
class CellBase;

class TransporterDialog : public QDialog
{
  Q_OBJECT

 public:
  TransporterDialog(Wall *w, CellBase *c, int wn, QWidget *parent = 0);

  private slots:
  void setTransporterValues();

 private:
  Wall *wall;
  CellBase *cell;
  int wall_num;
  QLabel *label;
  int ntransporters;
  QVector <QLineEdit*> editors;
};

#endif

/* finis */
