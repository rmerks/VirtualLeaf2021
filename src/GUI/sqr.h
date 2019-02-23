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
 *  From "Numerical recipes in C"
 *
 *  Copyright 2010 Roeland Merks.
 *
 */

#ifndef _SQR_H_
#define _SQR_H_


// redefine these macros as inline functions, to prevent scary
// behavior.  (According to compiler warnings, the above macros from
// Numerical Recipes in C are officially undefined...)
//
// However, they seem to work, but it seems safer to redefine them.
// Inline functions will behave like macros anyhow.
inline double DSQR( double a ) {

  if (a == 0.0) {
    return 0.0;
  } else {
    return a*a;
  }
}

inline float SQR( float a ) {
  if (a == 0.0) {
    return 0.0;
  } else {
    return a*a;
  }
}

#endif

/* finis */
