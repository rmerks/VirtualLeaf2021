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


#ifndef _RUNGEKUTTA_H_
#define _RUNGEKUTTA_H_

class RungeKutta  {

 public:
  RungeKutta(void) {
    kmax=kount=0;
    dxsav=0.;
    xp=0;
    yp=0;
  }
  virtual ~RungeKutta() {}

  void odeint(double ystart[], int nvar, double x1, double x2, double eps, double h1,
	      double hmin, int *nok, int *nbad);


 protected:
  // implement "derivs" in a derived class
  virtual void derivs(double x, double *y, double *dxdy) = 0;
  int kmax,kount;
  double *xp,**yp,dxsav;

 private:
  void rkqs(double *y, double *dydx, int n, double *x, double htry, double eps,
	    double *yscal, double *hdid, double *hnext);

  void rkck(double *y, double *dydx, int n, double x, double h, double yout[],
	    double *yerr);

  static const double Safety;
  static const double PGrow;
  static const double Pshrnk;
  static const double Errcon;
  static const double Maxstp;
  static const double Tiny;
};
#endif

/* finis */
