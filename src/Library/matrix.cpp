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
#include <ostream>
#include <cmath>
#include "vector.h"
#include "matrix.h"
#include "tiny.h"

static const std::string _module_id("$Id$");

Matrix::Matrix(const Vector &c1, const Vector &c2, const Vector &c3) {

  Alloc();

  mat[0][0]=c1.x; mat[0][1]=c2.x; mat[0][2]=c3.x;
  mat[1][0]=c1.y; mat[1][1]=c2.y; mat[1][2]=c3.y;
  mat[2][0]=c1.z; mat[2][1]=c2.z; mat[2][2]=c3.z;
}

void Matrix::Alloc(void)
{
  // constructor
  mat = new double*[3];
  mat[0] = new double[9];
  for (int i=1;i<3;i++)
    mat[i]=mat[i-1]+3;
}

Matrix::~Matrix()
{
  // destructor
  delete[] mat[0];
  delete[] mat;
}

Matrix::Matrix(void)
{
  // constructor
  Alloc();

  // clear matrix
  for (int i=0;i<9;i++) {
    mat[0][i]=0.;
  }
}

Matrix::Matrix(const Matrix &source)
{
  // copy constructor
  Alloc();

  for (int i=0;i<9;i++) {
    mat[0][i]=source.mat[0][i];
  }
}


void Matrix::operator=(const Matrix &source)
{
  // assignment

  // don't assign to self
  if (this==&source) return;

  // copy 
  for (int i=0;i<9;i++)
    mat[0][i]=source.mat[0][i];
}


void Matrix::print(ostream *os)
{
  *os << "{ { " << mat[0][0] << "," << mat[0][1] << "," << mat[0][2] 
      << "},{" << mat[1][0] << "," << mat[1][1] << "," << mat[1][2] 
      << "},{" << mat[2][0] << "," << mat[2][1] << "," << mat[2][2] << "} }";
}

ostream &operator<<(ostream &os, Matrix &v) {
  v.print(&os);
  return os;
}


Vector Matrix::operator*(const Vector &v) const
{
  // matrix * vector
  Vector result;

  result.x = mat[0][0]*v.x+mat[0][1]*v.y+mat[0][2]*v.z;
  result.y = mat[1][0]*v.x+mat[1][1]*v.y+mat[1][2]*v.z;
  result.z = mat[2][0]*v.x+mat[2][1]*v.y+mat[2][2]*v.z;

  return result;
}


bool Matrix::operator==(Matrix &m) const
{
  for (int i=0;i<9;i++) {
    if ((mat[0][i]-m.mat[0][i])>TINY)
      return false;
  }
  return true;
}

double Matrix::Det(void) const
{
  return 
    - mat[0][2]*mat[0][4]*mat[0][6]
    + mat[0][1]*mat[0][5]*mat[0][6] 
    + mat[0][2]*mat[0][3]*mat[0][7]
    - mat[0][0]*mat[0][5]*mat[0][7]
    - mat[0][1]*mat[0][3]*mat[0][8]
    + mat[0][0]*mat[0][4]*mat[0][8];
}

Matrix Matrix::Inverse(void) const
{

  // return the Inverse of this matrix
  double rd=1./Det(); // Reciproce Det;
  Matrix inverse;
  inverse.mat[0][0]=rd*(-mat[0][5]*mat[0][7]+mat[0][4]*mat[0][8]);
  inverse.mat[0][1]=rd*( mat[0][2]*mat[0][7]-mat[0][1]*mat[0][8]);
  inverse.mat[0][2]=rd*(-mat[0][2]*mat[0][4]+mat[0][1]*mat[0][5]);
  inverse.mat[0][3]=rd*( mat[0][5]*mat[0][6]-mat[0][3]*mat[0][8]);
  inverse.mat[0][4]=rd*(-mat[0][2]*mat[0][6]+mat[0][0]*mat[0][8]);
  inverse.mat[0][5]=rd*( mat[0][2]*mat[0][3]-mat[0][0]*mat[0][5]);
  inverse.mat[0][6]=rd*(-mat[0][4]*mat[0][6]+mat[0][3]*mat[0][7]);
  inverse.mat[0][7]=rd*( mat[0][1]*mat[0][6]-mat[0][0]*mat[0][7]);
  inverse.mat[0][8]=rd*(-mat[0][1]*mat[0][3]+mat[0][0]*mat[0][4]);

  return inverse;
}

void Matrix::Rot2D(double theta)
{ 
  // make this matrix a rotation matrix over theta
  // see http://mathworld.wolfram.com/RotationMatrix.html

  mat[0][0] = cos(theta); mat[0][1]=sin(theta);
  mat[1][0] = -sin(theta); mat[1][1]=cos(theta);
  mat[0][2] = mat[1][2] = mat[2][0] = mat[2][1] = mat[2][2] = 0.;
}

/* finis */
