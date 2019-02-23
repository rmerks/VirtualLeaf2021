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


#ifndef _UNIQUEMESSAGE_H_
#define _UNIQUEMESSAGE_H_

#include <QDialog>
#include <QCheckBox>
#include <QStringList>
#include <QLabel>

class UniqueMessageBox : public QDialog {
  Q_OBJECT

 public:
  UniqueMessageBox ( /* Icon icon,*/ 
		    const QString & title, 
		    const QString & text, 
		    /* StandardButtons buttons = Ok, */
		    QWidget * parent = 0 , Qt::WindowFlags f = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint );
  ~UniqueMessageBox();
  int exec(void);

 private:
  static QStringList issued_messages;
  bool display;
  QCheckBox *show_again;
  QPushButton *okButton;
  QLabel *label;
  QString boxtext;
};

#endif

/* finis */
