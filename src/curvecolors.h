/*
 *
 *  $Id$
 *
 *  This file is part of the Virtual Leaf.
 *
 *  The Virtual Leaf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The Virtual Leaf is distributed in the hope that it will be useful,
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


#ifndef _CURVECOLORS_H_
#define _CURVECOLORS_H_

#include <QStringList>

class CurveColors {

 public:
  CurveColors(void) {
    colors += QString("red");
    colors += QString("darkgreen");
    colors += QString("blue");
    colors += QString("purple");
    colors += QString("powderblue");
    colors += QString("darkgreen");
    colors += QString("darkcyan");
    colors += QString("burlywood");
    colors += QString("deeppink");
    colors += QString("deepskyblue");
    colors += QString("fuchsia");
    colors += QString("lawngreen");
    colors += QString("lavenderblush");
    colors += QString("pink");
    colors += QString("purple");
    colors += QString("navy");
    colors += QString("greenyellow");
    colors += QString("mintcream");
    colors += QString("saddlebrown");
    colors += QString("salmon");
    colors += QString("cornsilk");
    colors += QString("darkmagenta");
    colors += QString("chocolate");
    colors += QString("ivory");
    colors += QString("khaki");
    colors += QString("moccasin");
    colors += QString("mediumturquoise");
    colors += QString("mediumorchid");
    colors += QString("darkgray");
    colors += QString("lightseagreen");
    colors += QString("royalblue");
    colors += QString("darkred");
    colors += QString("violet");
    colors += QString("lavender");
    colors += QString("silver");
    colors += QString("slategray");
    colors += QString("ghostwhite");
    colors += QString("forestgreen");
    colors += QString("lightgrey");
    colors += QString("brown");
    colors += QString("lightgoldenrodyellow");
    colors += QString("darkblue");
    colors += QString("lime");
    colors += QString("darkkhaki");
    colors += QString("oldlace");
    colors += QString("springgreen");
    colors += QString("darkseagreen");
    colors += QString("skyblue");
    colors += QString("teal");
    colors += QString("lightsalmon");
    colors += QString("midnightblue");
    colors += QString("mediumslateblue");
    colors += QString("darkslateblue");
    colors += QString("indianred");
    colors += QString("honeydew");
    colors += QString("lemonchiffon");
    colors += QString("peru");
    colors += QString("snow");
    colors += QString("mistyrose");
    colors += QString("darkslategrey");
    colors += QString("gainsboro");
    colors += QString("mediumpurple");
    colors += QString("grey");
    colors += QString("palegoldenrod");
    colors += QString("linen");
    colors += QString("dimgrey");
    colors += QString("firebrick");
    colors += QString("lightcyan");
    colors += QString("steelblue");
    colors += QString("orangered");
    colors += QString("darkgoldenrod");
    colors += QString("turquoise");
    colors += QString("blueviolet");
    colors += QString("rosybrown");
    colors += QString("lightslategrey");
    colors += QString("lightgray");
    colors += QString("orchid");
    colors += QString("darkturquoise");
    colors += QString("darkorange");
    colors += QString("plum");
    colors += QString("dimgray");
    colors += QString("crimson");
    colors += QString("mediumaquamarine");
    colors += QString("chartreuse");
    colors += QString("limegreen");
    colors += QString("cadetblue");
    colors += QString("mediumblue");
    colors += QString("papayawhip");
    colors += QString("tan");
    colors += QString("dodgerblue");
    colors += QString("orange");
    colors += QString("lightblue");
    colors += QString("coral");
    colors += QString("blue");
    colors += QString("sandybrown");
    colors += QString("slategrey");
    colors += QString("lightpink");
    colors += QString("sienna");
    colors += QString("indigo");
    colors += QString("seashell");
    colors += QString("lightskyblue");
    colors += QString("paleturquoise");
    colors += QString("darkslategray");
    colors += QString("darkolivegreen");
    colors += QString("mediumspringgreen");
    colors += QString("floralwhite");
    colors += QString("lightyellow");
    colors += QString("palegreen");
    colors += QString("red");
    colors += QString("lightgreen");
    colors += QString("cyan");
    colors += QString("hotpink");
    colors += QString("cornflowerblue");
    colors += QString("maroon");
    colors += QString("magenta");
    colors += QString("lightsteelblue");
    colors += QString("gray");
    colors += QString("green");
    colors += QString("darkviolet");
    colors += QString("palevioletred");
    colors += QString("darksalmon");
    colors += QString("seagreen");
    colors += QString("tomato");
    colors += QString("olive");
    colors += QString("slateblue");
    colors += QString("peachpuff");
    colors += QString("gold");
    colors += QString("mediumseagreen");
    colors += QString("darkorchid");
    colors += QString("lightslategray");
    colors += QString("lightcoral");
    colors += QString("mediumvioletred");
    colors += QString("thistle");
  }
  QString &operator[](int i) {
    if (i>=colors.size() || i<0) {
      throw("Color number out of range in curvecolors.h");
    } else
      return colors[i];
  }
 private:
  QStringList colors;
};
#endif

/* finis */
