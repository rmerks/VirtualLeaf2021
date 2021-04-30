#
#  $Id$
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

CONFIG += release
CONFIG -= debug
CONFIG += qt
#CONFIG -= app_bundle
CONFIG+=sdk_no_version_check


VLEAFLIBDIR = ../Library
PERLDIR = $$top_srcdir/Perl

QT += widgets xml
QT += printsupport

INCLUDEPATH += ../Library
INCLUDEPATH += ../../include

#REACTIONS = reactions_auxin_growth.h 
#REACTIONS = reactions_meinhardt.h
#REACTIONS = reactions_pce_growth.h
DEFINES += QTGRAPHICS
DEFINES += REACTIONS_HEADER=$${REACTIONS}
DEFINES += REACTIONS_HEADER_STRING=\"$${REACTIONS}\"
DEFINES += FLEMING

BINDIR = ../../bin
LIBDIR = ../../lib

DESTDIR = $$BINDIR
TARGET = VirtualLeaf
TEMPLATE = app
PARTMPL = $${TARGET}par.tmpl
MAINSRC = $${TARGET}.cpp
QT -= network sql

LIBS += -L$${LIBDIR}
!win32 {
 GRAPHICS = qt #qwt
}

win32 {
 CONFIG -= console
 CONFIG += windows
 GRAPHICS = qt 
 RC_FILE = VirtualLeaf.rc
 QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
 LIBS +=  -lm #-lwsock32
}


macx:release {
 LIBS+= -dead_strip
}

unix {
# CC = /usr/bin/gcc
# # QWTDIR = /ufs/guravage/opt/qwt-5.2.1-svn
# #QMAKE_LIBDIR += $$QWTDIR/lib
QMAKE_CXXFLAGS += -fPIC# -I/usr/include/libxml2
# QMAKE_LFLAGS += -fPIC
 LIBS +=  -lm
}

# Input
# apoplastitem.h
HEADERS += \
 canvas.h \
$${VLEAFLIBDIR}/cellbase.h \
 cell.h \
 cellitem.h \
 forwardeuler.h \
 hull.h \
 infobar.h \
 mainbase.h \
 mainbase.h \
 $${VLEAFLIBDIR}/matrix.h \
 mesh.h \
 modelcatalogue.h \
 Neighbor.h \
 node.h \
 nodeitem.h \
 nodeset.h \
 OptionFileDialog.h \
 $${VLEAFLIBDIR}/output.h \
 $${VLEAFLIBDIR}/parameter.h \
 pardialog.h \
 $${VLEAFLIBDIR}/parse.h \
 pi.h \
 qcanvasarrow.h \
 $${VLEAFLIBDIR}/random.h \
 rungekutta.h \
 simitembase.h \
 $${VLEAFLIBDIR}/simplugin.h \
 sqr.h \
 tiny.h \
 tissuegenerator.h \
 transporterdialog.h \
 $${VLEAFLIBDIR}/UniqueMessage.h \
 $${VLEAFLIBDIR}/vector.h \
 $${VLEAFLIBDIR}/wallbase.h \
 wall.h \
 wallitem.h \
 $${VLEAFLIBDIR}/warning.h \
 xmlwrite.h \
 $${VLEAFLIBDIR}/$${PARTMPL}

# apoplastitem.cpp
SOURCES += \
 canvas.cpp \
 cell.cpp \
$${VLEAFLIBDIR}/cellbase.cpp \
 cellitem.cpp \
 forwardeuler.cpp \
 hull.cpp \
 $${VLEAFLIBDIR}/matrix.cpp \
 mainbase.cpp \
 mesh.cpp \
 modelcatalogue.cpp \
 Neighbor.cpp \
 node.cpp \
 nodeitem.cpp \
 nodeset.cpp \
 $${VLEAFLIBDIR}/output.cpp \
 OptionFileDialog.cpp \
 $${VLEAFLIBDIR}/parameter.cpp \
 pardialog.cpp \
 $${VLEAFLIBDIR}/parse.cpp \
$${VLEAFLIBDIR}/random.cpp \
    qtxmlwrite.cpp \
 rungekutta.cpp \
 simitembase.cpp \
 tissuegenerator.cpp \
 transporterdialog.cpp \
$${VLEAFLIBDIR}/UniqueMessage.cpp \
$${VLEAFLIBDIR}/vector.cpp \
 wall.cpp \
$${VLEAFLIBDIR}/wallbase.cpp \
 wallitem.cpp \
$${VLEAFLIBDIR}/warning.cpp \
 $$MAINSRC

contains( TARGET, leaf_fleming ) {
 DEFINES += FLEMING	
}

# Create pardialog.cpp from *.tmpl file
#TMPL_FILES = $${VLEAFLIBDIR}/$${PARTMPL}
#tmpl_compiler.output = pardialog.cpp
#tmpl_compiler.commands = perl $${PERLDIR}/make_pardialog_source.pl ${QMAKE_FILE_NAME}
#tmpl_compiler.input = TMPL_FILES
#tmpl_compiler.variable_out = SOURCES
#tmpl_compiler.clean =
#QMAKE_EXTRA_COMPILERS += tmpl_compiler

# Create pardialog.h from *.tmpl file
#tmplh_compiler.output = pardialog.h
#tmplh_compiler.commands = perl $${PERLDIR}/make_pardialog_source.pl ${QMAKE_FILE_NAME}
#tmplh_compiler.input = TMPL_FILES
#tmplh_compiler.variable_out = HEADERS
#tmplh_compiler.config += moc_verify
# tmplh_compiler.clean = 
#QMAKE_EXTRA_COMPILERS += tmplh_compiler

# See https://forum.qt.io/topic/64160/qmake_extra_compilers-and-moc/2
#PARDIALOG_H = pardialog.h
#new_moc.commands = moc $(DEFINES) $(INCPATH) pardialog.h -o ${QMAKE_FILE_OUT}
#new_moc.input = "PARDIALOG_H"
#new_moc.output = moc_pardialog.cpp
#new_moc.variable_out = SOURCES
#QMAKE_EXTRA_COMPILERS += new_moc

contains(GRAPHICS, qwt) {
 #macx:LIBS += -L$$QWTDIR/lib -lqwt
 #win32:LIBS += -L$$QWTDIR/lib -lqwt5
 #LIBS += -L$$QWTDIR/lib -lqwt
 INCLUDEPATH += $$QWTDIR/include
 DEFINES += HAVE_QWT
 HEADERS += data_plot.h
 SOURCES += data_plot.cpp
}

contains( GRAPHICS, qt ) {
 message( "Building Qt executable" )
 QMAKE_CXXFLAGS += -DQTGRAPHICS # -fpermissive
}



# MACOSX packaging
macx {
ICON = leaficon.icns
QMAKE_POST_LINK = "cp leaficon.icns $${DESTDIR}/$${TARGET}.app; \ 
      cp ../../doc/gpl3.txt $${DESTDIR}/$${TARGET}.app/Contents/MacOS/.;" 
}


