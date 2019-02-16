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


TARGET = tutorial4
VLEAFHOME = ../../..

CONFIG += release
CONFIG -= debug
CONFIG += plugin

BINDIR = $${VLEAFHOME}/bin
LIBDIR = $${VLEAFHOME}/lib
INCDIR = $${VLEAFHOME}/src
DEFINES = QTGRAPHICS # VLEAFPLUGIN
DESTDIR = $${BINDIR}/models
HEADERS = $${TARGET}.h 
INCLUDEPATH += $${INCDIR}	

QMAKE_CXXFLAGS += -fexceptions #-I$${INCDIR}
QMAKE_CXXFLAGS += -Wno-write-strings
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS_DEBUG += -g3
QMAKE_CXXFLAGS_DEBUG += -DQDEBUG
QT += qt3support
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
 LIBS += -L$${LIBDIR} -lvleaf
 QMAKE_CXXFLAGS += -DLIBXML_STATIC
 QMAKE_CXXFLAGS += -I$${LIBXML2DIR}\include -I$${LIBICONVDIR}\include -I$${LIBZDIR}\include

}

# finish
