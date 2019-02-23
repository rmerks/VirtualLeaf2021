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

#include <cmath>
#include <iostream> 
#include "forwardeuler.h"
#include "warning.h"
#include "maxmin.h"
#include <string>

using namespace std;

static const string _module_id("$Id$");

// The value Errcon equals (5/Safety) raised to the power (1/PGrow), see use below.

const double ForwardEuler::Safety  = 0.9;
const double ForwardEuler::PGrow = -0.2;
const double ForwardEuler::Pshrnk = -0.25;
const double ForwardEuler::Errcon = 1.89e-4;
const double ForwardEuler::Maxstp = 10000000;
const double ForwardEuler::Tiny = 1.0e-30;



/* User storage for intermediate results. Preset kmax and dxsav in the calling program. If kmax =
   0 results are stored at approximate intervals dxsav in the arrays xp[1..kount], yp[1..nvar]
   [1..kount], where kount is output by odeint. Defining declarations for these variables, with
   memoryallo cations xp[1..kmax] and yp[1..nvar][1..kmax] for the arrays, should be in
   the calling program.*/

void ForwardEuler::odeint(double *ystart, int nvar, double x1, double x2, double eps, double h1, double hmin, int *nok, int *nbad)
/* Runge-Kutta driver with adaptive stepsize control. Integrate starting values ystart[1..nvar]
   from x1 to x2 with accuracy eps, storing intermediate results in global variables. h1 should
   be set as a guessed first stepsize, hmin as the minimum allowed stepsize (can be zero). On
   output nok and nbad are the number of good and bad (but retried and fixed) steps taken, and
   ystart is replaced byv alues at the end of the integration interval. derivs is the user-supplied
   routine for calculating the right-hand side derivative, while rkqs is the name of the stepper
   routine to be used. */
{
  static bool warning_issued = false;

  if (!warning_issued) {
    cerr << "Using inaccurate method ForwardEuler\n";
    warning_issued=true;
    //MyWarning::warning("Using inaccurate method ForwardEuler");
  }
  // N.B. Not for serious use and not fully usage compatible with RungeKutta
  // simply for testing API of integrator.

  double *y,*dydx;
  y=new double[nvar];
  dydx=new double[nvar];
  double x=x1;

  for (int i=0;i<nvar;i++) y[i]=ystart[i];

  //if (kmax > 0) xsav=x-dxsav*2.0; //Assures storage of first step.

  dydx=new double[nvar];

  for (int nstp=0;nstp<Maxstp;nstp++) {

    derivs(x,y,dydx);

    if (kmax > 0 && kount < kmax-1) {
      xp[kount]=x; //Store intermediate results.
      for (int i=0;i<nvar;i++) yp[i][kount]=y[i];
      kount++;
      //xsav=x;
    }

    for (int i=0;i<nvar;i++) {
      y[i]=y[i] + h1 * dydx[i];
    }

    x = x + h1;

    if ((x-x2)*(x2-x1) >= 0.0) { //Are we done?
      goto done;
    }
  }

 done:
  for (int i=0;i<nvar;i++) ystart[i]=y[i];
  if (kmax) {
    xp[kount]=x; //Save final step.
    for (int i=0;i<nvar;i++) yp[i][kount]=y[i];

  }

  delete[] dydx;
  delete[] y;
  return; //Normal exit.
}

/* finis */
