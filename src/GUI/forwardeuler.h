/*
 *  VirtualLeaf
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

#ifndef _FORWARDEULER_H_
#define _FORWARDEULER_H_

class ForwardEuler  {

 public:
  ForwardEuler(void) {
    kmax=kount=0;
    dxsav=0.;
    xp=0;
    yp=0;
  }

  virtual ~ForwardEuler() {}

  void odeint(double ystart[], int nvar, double x1, double x2, double eps, double h1,
	      double hmin, int *nok, int *nbad);

  // implement this function in a derived class
 protected:
  virtual void derivs(double x, double *y, double *dxdy) = 0;
  int kmax,kount;
  double *xp,**yp,dxsav;

 private:

  static const double Safety;
  static const double PGrow;
  static const double Pshrnk;
  static const double Errcon;
  static const double Maxstp;
  static const double Tiny;
};
#endif

/* finis */
