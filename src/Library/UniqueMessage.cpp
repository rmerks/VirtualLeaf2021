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
#include <QMessageBox>
#include <QCheckBox>
#include <QPushButton>
#include <QBoxLayout>
#include "UniqueMessage.h"

#include <iostream>

static const std::string _module_id("$Id$");

using namespace std;
UniqueMessageBox::UniqueMessageBox ( /* Icon icon,*/ 
				    const QString & title, 
				    const QString & text, 
				    QWidget * parent ,
				    Qt::WindowFlags f ) : 
  QDialog(parent, f) {

  label = new QLabel(text);
  boxtext = text;

  show_again = new QCheckBox(tr("Do not show this message again"));
  okButton = new QPushButton(tr("Ok"));

  if (issued_messages.contains(boxtext) ) {

    cerr << "Saw message before\n";
    display = false;
  } else {
    display=true;
  }

  connect(okButton, SIGNAL(clicked()), this, SLOT(close()) );
  QHBoxLayout *hlayout = new QHBoxLayout;
  hlayout->addWidget(label);
  hlayout->addWidget(okButton);
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addLayout(hlayout);
  layout->addWidget(show_again);
  setLayout(layout);
  setWindowTitle(title);
};

UniqueMessageBox::~UniqueMessageBox(void)  {

  if (show_again->checkState() == Qt::Checked ) {
    cerr << "Message won't be shown again\n";
    issued_messages << boxtext;
  }
}
int UniqueMessageBox::exec(void)  {

  if (display)
    return QDialog::exec();
  else {
    return 1;
  }
}

QStringList UniqueMessageBox::issued_messages;

/* finis */
