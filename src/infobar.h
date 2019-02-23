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


#ifndef _INFOBAR_H_
#define _INFOBAR_H_


#include <qmainwindow.h>
#include <QLabel>
#include <QBoxLayout>
#include <QDockWidget>

class InfoBar : public QDockWidget {

  Q_OBJECT
    public:

  InfoBar(void) : QDockWidget() {

    virtleaf = new QLabel();
    SetText("undefined");

    //setHorizontalStretchable(true);
      //boxLayout()->addStretch();
    //boxLayout()->addWidget(virtleaf);//, Qt::AlignHCenter);
    //boxLayout()->addStretch();
  }

  void SetText(QString text) {
    virtleaf->setText(QString("<h1>VirtualLeaf</h1>\n<center><b>Model:</b> <i>%1</i></center>").arg(text));
  }

 private:
  QLabel *virtleaf;
};

#endif

/* finis */
