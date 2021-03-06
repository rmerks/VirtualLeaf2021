#!/usr/bin/perl 

#
# $Id$
#
#  This file is part of the Virtual Leaf.
#
#  The Virtual Leaf is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  The Virtual Leaf is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with the Virtual Leaf.  If not, see <http://www.gnu.org/licenses/>.
#
#  Copyright 2010 Roeland Merks.
#

# input: parameter file + types
# format: par_name = default_value/type

# output: C++ source code of class Parameter
# and sample parameter file

%funname = (
    "double" => "toDouble",
    "int" => "toInt",
    );

%typetrans = (
    "double" => "double",
    "float" => "double",
    "int" => "int",
    "bool" => "bool",
    "char *" => "char *",
    "string" => "char *",
    "directory" => "char *",
    "doublelist" => "double *",
    "label" => "label",
    "title" => "title",
    );

$basename = "pardialog";

open parfile,"<$ARGV[0]";
open cppfile,">$basename.cpp";

$i=0;
while (<parfile>) {
    if (/^#/) {
	next;
    }
    @line=split(/=/);
#ignore empty lines
    if ($#line<1) {
	next;
    }
    $param[$i]=$line[0];
    $value_type=$line[1];
    
    @typel=split(/ \/ /,$value_type);
    $value[$i] = $typel[0];
    $type[$i] = $typel[1];

    #get rid of spaces
    $type[$i] =~ s/ //g;
    $type[$i] =~s/\n//g;
    $convtype[$i]=$typetrans{$type[$i]};
    $param[$i] =~ s/ //g;
    if ($convtype[$i] ne "label" && $convtype[$i] ne "title") {
	#get rid of spaces
	$value[$i] =~ s/ //g;
    }
    if ($convtype[$i] eq "char *") {
	$value[$i] = "\"$value[$i]\"";
    }
    #print cppfile "param = $param, value = $value, type = $type\n";

    $i++;
}

$lines=$i;


print cppfile <<END_HEADER;
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

// WARNING: This file is automatically generated by make_parameter_source.pl. Do not edit.
// Do not edit. All edits will be discarded.

#include "$basename.h"
#include "parameter.h"
#include <cstring>
#include <QDialog>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>

static const std::string _module_id("\$Id\$");

ParameterDialog::ParameterDialog(QWidget *parent, Qt::WindowFlags f) : QDialog(parent,f) {
    extern Parameter par;
END_HEADER

for ($i=0;$i<$lines;$i++) {
    if ($convtype[$i] eq "label" || $convtype[$i] eq "title") {
	next;
    }
    if ($convtype[$i] eq "double *") {
	print cppfile  "  QString $param[$i]_string(\"";
	@paramlist = split(/,/,$value[$i]);
	for ($j=1;$j<=$#paramlist;$j++) {
	    print cppfile "%$j,";
	}
	print cppfile "%$j\");\n";
	print cppfile "  $param[$i]_string = $param[$i]_string";
	for ($j=0;$j<=$#paramlist;$j++) {
	    print cppfile ".arg(par.$param[$i]\[$j\])";
	}
	print cppfile ";\n";
	print cppfile "  $param[$i]_edit = new QLineEdit( $param[$i]_string, this );\n";
    } else {
	if ($convtype[$i] eq "bool") {
	    print cppfile "  $param[$i]_edit = new QLineEdit( QString(\"%1\").arg(sbool(par.$param[$i])), this );\n";
	} else {
	    print cppfile "  $param[$i]_edit = new QLineEdit( QString(\"%1\").arg(par.$param[$i]), this );\n";
	}
    }
}


print cppfile <<END_HEADER3;
// make a 1x1 grid; it will auto-expand
QGridLayout *grid = new QGridLayout( this);
    
// add the first four widgets with (row, column) addressing
END_HEADER3

$numrows = 25;
$c = 0;
for ($i=0;$i<$lines;$i++) {
    $col = 2*int($c/($numrows-3));
    $colplus1 = $col+1;
    $row = $c % ($numrows-3) + $ntitles * 3;
    if ($convtype[$i] eq "label") {
	print cppfile "  grid->addWidget( new QLabel( \"$value[$i]\", this), $row, $col, 1, 2 );\n";
	$c++;
    } else {
	if ($convtype[$i] eq "title") {
	    if ($ntitles) {
		print stderr "Sorry, only one title allowed. Aborting source construction...\n";
		exit(1);
	    }
	    print cppfile "  setWindowTitle( QString( \"$value[$i]\") );\n";
	    print cppfile "  grid->addWidget( new QLabel( \"<h3>$value[$i]</h3>\",this), $row, $col, 1, -1, Qt::AlignCenter);\n";
	    print cppfile "  grid->addWidget( new QLabel( \"\", this), $row+1, $col, 1, -1);\n";
	    $ntitles++;
	} else {
	    print cppfile "  grid->addWidget( new QLabel( \"$param[$i]\", this ),$row, $col );\n";
	    print cppfile "  grid->addWidget( $param[$i]_edit, $row, $colplus1 );\n";
	    $c++;
    }
    }
    if (int(($c-1)%($numrows-3))==0) {
        print cppfile "  grid->setColumnMinimumWidth($colplus1, 80);\n";
    }
}

$row = $numrows+1;
$col = 2*int($i/$numrows);

print cppfile <<ANOTHER_LABEL;
QPushButton *pb = new QPushButton( \"&Write\", this );
grid->addWidget(pb, $row, $col );
connect( pb, SIGNAL( clicked() ), this, SLOT( write() ) );
QPushButton *pb2 = new QPushButton( \"&Close\", this );
grid->addWidget(pb2,$row, $col+1 );
connect( pb2, SIGNAL( clicked() ), this, SLOT( close() ) );
QPushButton *pb3 = new QPushButton( \"&Reset\", this );
grid->addWidget(pb3, $row, $col+2 );
connect( pb3, SIGNAL( clicked() ), this, SLOT( Reset() ) );
show();
};

ParameterDialog::~ParameterDialog(void) {
ANOTHER_LABEL

for ($i=0;$i<$lines;$i++) {
    if ($convtype[$i] ne "label" && $convtype[$i] ne "title") {
	print cppfile "delete $param[$i]_edit;\n";
    }
}
print cppfile <<END_HEADER4;
}

void ParameterDialog::write(void) {
    
extern Parameter par;
QString tmpval;
END_HEADER4

    for ($i=0;$i<$lines;$i++) {
	if ($convtype[$i] eq "label" || $convtype[$i] eq "title") {
	    next;
	}
	if ($convtype[$i] eq "double *") {
	    @paramlist = split(/,/,$value[$i]);
	    for ($j=0;$j<=$#paramlist;$j++) {
		print cppfile "  tmpval = $param[$i]_edit->text().section(',', $j, $j);\n";
		print cppfile "  par.$param[$i]\[$j\] = tmpval.toDouble();\n";
	    }
	} else {
	    if ($convtype[$i] eq "bool") {
		print cppfile "  tmpval = $param[$i]_edit->text().trimmed();\n";
		print cppfile "  if (tmpval == \"true\" || tmpval == \"yes\" ) par.$param[$i] = true;\n";
		print cppfile "  else if (tmpval == \"false\" || tmpval == \"no\") par.$param[$i] = false;\n";
		print cppfile "  else {\n";
		print cppfile "    if (QMessageBox::question(this, \"Syntax error\", tr(\"Value %1 of parameter %2 is not recognized as Boolean.\\nDo you mean TRUE or FALSE?\").arg(tmpval).arg(\"$param[$i]\"),\"True\",\"False\", QString(), 0, 1)==0) par.$param[$i]=true;\n";
		print cppfile "      else par.$param[$i]=false;\n";
		print cppfile "  }\n";
	    } else {
		if ($convtype[$i] eq "char *") {
		    print cppfile "  par.$param[$i] = strdup($param[$i]_edit->text().toLatin1());\n";
		} else {
		    print cppfile "  par.$param[$i] = $param[$i]_edit->text().$funname{$convtype[$i]}();\n";
		}
	    }
	}
}



print cppfile <<END_MIDPART;
Reset();

}
END_MIDPART

print cppfile "void ParameterDialog::Reset(void) {\n";
print cppfile "  extern Parameter par;\n";

for ($i=0;$i<$lines;$i++) {
    if ($convtype[$i] eq "label" || $convtype[$i] eq "title") {
	next;
    }
    if ($convtype[$i] eq "double *") {
	print cppfile  "  QString $param[$i]_string(\"";
	@paramlist = split(/,/,$value[$i]);
	for ($j=1;$j<=$#paramlist;$j++) {
	    print cppfile "%$j,";
	}
	print cppfile "%$j\");\n";
	print cppfile "  $param[$i]_string = $param[$i]_string";
	for ($j=0;$j<=$#paramlist;$j++) {
	    print cppfile ".arg(par.$param[$i]\[$j\])";
	}
	print cppfile ";\n";
	print cppfile "  $param[$i]_edit->setText( $param[$i]_string );\n";
    } else {
	if ($convtype[$i] eq "bool") {
	    print cppfile "  $param[$i]_edit->setText( QString(\"%1\").arg(sbool(par.$param[$i])));\n";
	} else {
	    print cppfile "  $param[$i]_edit->setText( QString(\"%1\").arg(par.$param[$i]) );\n";
	}
    }
}

print cppfile "}\n\n";

# qparameter.h

open hfile, ">$basename.h";
print hfile <<END_HEADER2;
/*
 *
 *  \$Id\$
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

// WARNING: This file is automatically generated by make_parameter_source.pl. Do not edit.
// Do not edit. All edits will be discarded.

#ifndef PARAMETER_DIALOG_H
#define PARAMETER_DIALOG_H
#include <QDialog>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <iostream>

class ParameterDialog : public QDialog {
    Q_OBJECT
	
      public:
	ParameterDialog(QWidget *parent=0, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~ParameterDialog(void);
    public slots:
    void Reset(void);

    private slots:
    void write(void);

  private:
END_HEADER2

for ($i=0;$i<$lines;$i++) {
    if ($convtype[$i] ne "label" && $convtype[$i] ne "title") {
	print hfile "  QLineEdit *$param[$i]_edit;\n";
    }
}

print hfile <<END_TRAILER2;
};
#endif
END_TRAILER2

# finis
