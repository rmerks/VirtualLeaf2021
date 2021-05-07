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
CONFIG += staticlib

QT += widgets xml
QT += core
QT += printsupport
QT += xml
INCLUDEPATH += ../GUI
INCLUDEPATH += ../../include

#QMAKE_CXXFLAGS += -fexceptions
#QMAKE_CXXFLAGS += -Wall
#QMAKE_CXXFLAGS += -Wno-write-strings
#QMAKE_CXXFLAGS += -Wno-unused-parameter
#win32:QMAKE_CXXFLAGS -= O2
#QMAKE_CXXFLAGS_DEBUG += -g3
#QMAKE_CXXFLAGS_DEBUG += -DQDEBUG
#QMAKE_CXXFLAGS_DEBUG -= -finstrument-functions
#QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10.5

DEFINES = QTGRAPHICS VLEAFPLUGIN
LIBDIR = ../../lib
DESTDIR = $${LIBDIR}
PARTMPL = VirtualLeafpar.tmpl
PERLDIR = ../Perl
#QT += qt3support


TARGET = vleaf
TEMPLATE = lib

HEADERS = \
 cellbase.h \
 matrix.h \
 output.h \
 parameter.h \
 parse.h \
 random.h \
 simplugin.h \
 UniqueMessage.h \
 vector.h \
 wallbase.h \
 warning.h \
vleafmodel.h \
flux_function.h

SOURCES = \
 cellbase.cpp \
 matrix.cpp \
 output.cpp \
 parameter.cpp \
 parse.cpp \
 random.cpp \
 simplugin.cpp \
 UniqueMessage.cpp \
 vector.cpp \
 wallbase.cpp \
 warning.cpp

# Create parameter.cpp from *.tmpl file
#TMPL_FILES = $${PARTMPL}
#tmpl_compiler.output = parameter.cpp
#tmpl_compiler.commands = perl $${PERLDIR}/make_parameter_source.pl ${QMAKE_FILE_NAME}
#tmpl_compiler.input = TMPL_FILES
#tmpl_compiler.variable_out = SOURCES
#tmpl_compiler.clean = ${QMAKE_FILE_OUT}
#QMAKE_EXTRA_COMPILERS += tmpl_compiler

# Create parameter.h from *.tmpl file
#tmplh_compiler.output = parameter.h
#tmplh_compiler.commands = perl $${PERLDIR}/make_parameter_source.pl ${QMAKE_FILE_NAME}
#tmplh_compiler.input = TMPL_FILES
#tmplh_compiler.variable_out = HEADERS
#tmplh_compiler.clean = ${QMAKE_FILE_OUT}
#QMAKE_EXTRA_COMPILERS += tmplh_compiler

unix {
 QMAKE_CXXFLAGS += -fPIC 
 QMAKE_LFLAGS += -fPIC
 LIBS +=-lm
}

QMAKE_CXXFLAGS += -Wall -Wextra
QMAKE_CXXFLAGS += -Wno-unused-parameter


win32 {

}

#
