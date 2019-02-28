#
# $Id: plugin_turing.pro,v caa535c40937 2010/06/25 14:50:35 michael $
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
CONFIG += plugin

BINDIR = ../../../bin
LIBDIR = ../../../lib
INCLUDEPATH += ../../Library
INCLUDEPATH += ../../GUI
INCLUDEPATH += ../../../include

DEFINES = QTGRAPHICS # VLEAFPLUGIN
DESTDIR = $${BINDIR}/models
TARGET = turing
HEADERS = ../simplugin.h $${TARGET}.h  
QMAKE_CXXFLAGS += -fexceptions -I..
QMAKE_CXXFLAGS += -Wno-write-strings
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS_DEBUG += -g3
QMAKE_CXXFLAGS_DEBUG += -DQDEBUG

QT += widgets
SOURCES = $${TARGET}.cpp
TEMPLATE = lib 

unix {
 LIBS += -L$${LIBDIR} -lvleaf
 QMAKE_CXXFLAGS += -fPIC -I/usr/include/libxml2
 QMAKE_LFLAGS += -fPIC
}

win32 {
 LIBXML2DIR = $${LIBDIR}\libxml2
 LIBICONVDIR = $${LIBDIR}\libiconv
 LIBZDIR = $${LIBDIR}\libz
 LIBS += -L$${LIBDIR} -Llib -lvleaf
 QMAKE_CXXFLAGS += -DLIBXML_STATIC
 QMAKE_CXXFLAGS += -I$${LIBXML2DIR}\include -I$${LIBICONVDIR}\include -I$${LIBZDIR}\include
}

# finis

