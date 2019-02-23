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


#ifndef _MATRIX_H_
#define _MATRIX_H_

// three by three matrix (mostly for changes of basis of the vector object)
class Vector;

class Matrix {

 public:
  Matrix(const Vector &v1,const Vector &v2, const Vector &v3); // constructor
  Matrix(void);
  ~Matrix(); // destructor

  void print(ostream *os);
  Matrix(const Matrix &source); // copy constructor
  void operator=(const Matrix &source); // assignment operator

  Vector operator*(const Vector &v) const; // matrix * vector
  bool operator==(Matrix &v) const; // comparison
  double Det(void) const; // gives the "determinant" (| m |) of m
  Matrix Inverse(void) const; // gives the inverse of m
  void Rot2D(double theta); // make a matrix doing a 2D rotation over theta
  // data members 
  double **mat;

 private:

  void Alloc(void);
};

ostream &operator<<(ostream &os, Matrix &v);

#endif

/* finis */
