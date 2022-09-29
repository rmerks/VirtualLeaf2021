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


#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <iostream>
#include <cmath>
#include "sqr.h"
#ifdef QTGRAPHICS
#include <QPointF>
#endif


// three dimensional vector
using namespace std;

class Vector {

 public:
  inline Vector(const double cx=0,const double cy=0,const double cz=0) {
    x=cx;
    y=cy;
    z=cz;
  }; 
  explicit inline Vector(const double m[3]) {
    x=m[0];
    y=m[1];
    z=m[2];
  }


  inline Vector(const Vector &source) { 

    // copy constructor

    x=source.x;
    y=source.y;
    z=source.z;

  }
  // type conversion from and to QPointF (2D vector class from Qt library; we throw away "z" here!!)
#ifdef QTGRAPHICS
  inline operator QPointF() const {
    return QPointF( x, y );
  }
  inline Vector(const QPointF &p) {
    x = p.x();
    y = p.y();
    z = 0.;
  }
#endif
  virtual ~Vector() {

  };

  virtual ostream& print(ostream &os) const;
  void operator=(const Vector &source); // assignment operator
  void operator=(const double &s) {
    x=s;
    y=s;
    z=s;
  }
  Vector operator+(const Vector &v) const; // addition
  inline Vector operator-(const Vector &v) const { 

    Vector result;

    result.x=x-v.x;
    result.y=y-v.y;
    result.z=z-v.z;

    return result;
  }
  Vector &operator-=(const Vector &v);

  inline Vector &operator+=(const Vector &v) {

    x+=v.x;
    y+=v.y;
    z+=v.z;

    return *this;
  }

  Vector operator/(const double divisor) const; // division by a double
  Vector operator*(const double multiplier) const; // multiply by a scalar (double)
  Vector &operator/=(const double divisor);
  Vector &operator*=(const double multiplier);
  Vector operator*(const Vector &v) const; // cross product
  bool operator==(const Vector &v) const; // comparison
  bool operator< (const Vector &v) const; // order x,y,z
  inline double Norm(void) const {

    return sqrt(DSQR(x)+DSQR(y)+DSQR(z));
  }

  // Quick and dirty Norm (i.e. Manhattan distance)
  // (e.g. useful if we want to see if the vec is (0,0,0);
  inline double ManhattanNorm(void) const {
    return x+y+z;
  }

  double SqrNorm(void) const; // gives the square of |v|
  void Normalise(void); // normalise myself
  Vector Normalised(void) const; // return me normalised
  double Angle(const Vector &angle) const; // gives the angle between me and another vector
  double SignedAngle(const Vector &v) const;
  bool SameDirP(const Vector &v);
  double Max(void); // Find max, resp. min value of vector
  double Min(void);
  Vector Perp2D(void) const {
    return Vector(y,-x,0);
  }

  // data members
  double x,y,z;

  void Dump(ostream &os) const { 
    os << x << " " << y << " " << z << " ";
  }
  void ReadDump(istream &is) {
    is >> x >> y >> z;
  }
 private:
};

ostream &operator<<(ostream &os, const Vector &v);
Vector operator*(const double multiplier, const Vector &v);
double InnerProduct(const Vector &v1, const Vector &v2);

#endif

/* finis */
