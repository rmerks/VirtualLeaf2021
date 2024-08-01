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
#include <math.h>
#include <iostream>
#include "rungekutta.h"
#include "warning.h"
#include "maxmin.h"

static const std::string _module_id("$Id$");

// The value Errcon equals (5/Safety) raised to the power (1/PGrow), see use below.

/*
  static float maxarg1,maxarg2;
  #define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
  (maxarg1) : (maxarg2))
  static float minarg1,minarg2;
  #define FMIN(a,b) (minarg1=(a),minarg2=(b),(minarg1) < (minarg2) ?\
  (minarg1) : (minarg2))
  #define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
*/

const double RungeKutta::Safety  = 0.9;
const double RungeKutta::PGrow = -0.2;
const double RungeKutta::Pshrnk = -0.25;
const double RungeKutta::Errcon = 1.89e-4;
const double RungeKutta::Maxstp = 10000;
const double RungeKutta::Tiny = 1.0e-30;

void RungeKutta::rkqs(double *y, double *dydx, int n, double *x, double htry, double eps,
		      double *yscal, double *hdid, double *hnext)
/* Fifth-order Runge-Kutta step with monitoring of local truncation error to ensure accuracy and
   adjust stepsize. Input are the dependent variable vector y[1..n] and its derivative dydx[1..n]
   at the starting value of the independent variable x. Also input are the stepsize to be attempted
   htry, the required accuracy eps, and the vector yscal[1..n] against which the error is
   scaled. On output, y and x are replaced by their new values, hdid is the stepsize that was
   actuallyac complished, and hnext is the estimated next stepsize. derivs is the user-supplied
   routine that computes the right-hand side derivatives. */
{
  int i;
  double errmax,h,htemp,xnew,*yerr,*ytemp;
  yerr=new double[n];
  ytemp=new double[n];

  h=htry; // Set stepsize to the initial trial value.
  for (;;) {
    rkck(y,dydx,n,*x,h,ytemp,yerr); // Take a step.
    errmax=0.0; //Evaluate accuracy.
    for (i=0;i<n;i++) errmax=FMAX(errmax,fabs(yerr[i]/yscal[i]));
    errmax /= eps; // Scale relative to required tolerance.
    if (errmax <= 1.0) break; //Step succeeded. Compute size of next step.
    htemp=Safety*h*pow(errmax,Pshrnk);
    //Truncation error too large, reduce stepsize.
    h=(h >= 0.0 ? FMAX(htemp,0.1*h) : FMIN(htemp,0.1*h));
    //No more than a factor of 10.
    xnew=(*x)+h;
    if (xnew == *x) MyWarning::error("stepsize underflow in rkqs, with h = %f and htry = %f",h, htry);
  }
  if (errmax > Errcon) {
    *hnext=Safety*h*pow(errmax,PGrow);
    //std::cerr << "hnext = " << *hnext << std::endl;
  }
  else *hnext=5.0*h; //No more than a factor of 5 increase.
  *x += (*hdid=h);
  for (i=0;i<n;i++) {
	  y[i]=ytemp[i];
	  if (std::isnan(y[i])) {
		  y[i]=0.;
	  }
  }
  delete[] ytemp;
  delete[] yerr;
}


void RungeKutta::rkck(double *y, double *dydx, int n, double x, double h, double *yout, double *yerr)
/*Given values for n variables y[1..n] and their derivatives dydx[1..n] known at x, use
  the fifth-order Cash-Karp Runge-Kutta method to advance the solution over an interval h
  and return the incremented variables as yout[1..n]. Also return an estimate of the local
  truncation error in yout using the embedded fourth-order method. The user supplies the routine
  derivs(x,y,dydx), which returns derivatives dydx at x.*/
{
  int i;

  static double a2=0.2,a3=0.3,a4=0.6,a5=1.0,a6=0.875,b21=0.2,
    b31=3.0/40.0,b32=9.0/40.0,b41=0.3,b42 = -0.9,b43=1.2,
    b51 = -11.0/54.0, b52=2.5,b53 = -70.0/27.0,b54=35.0/27.0,
    b61=1631.0/55296.0,b62=175.0/512.0,b63=575.0/13824.0,
    b64=44275.0/110592.0,b65=253.0/4096.0,c1=37.0/378.0,
    c3=250.0/621.0,c4=125.0/594.0,c6=512.0/1771.0,
    dc5 = -277.00/14336.0;
  double dc1=c1-2825.0/27648.0,dc3=c3-18575.0/48384.0,
    dc4=c4-13525.0/55296.0,dc6=c6-0.25;
  double *ak2,*ak3,*ak4,*ak5,*ak6,*ytemp;
  ak2=new double[n];
  ak3=new double[n];
  ak4=new double[n];
  ak5=new double[n];
  ak6=new double[n];
  ytemp=new double[n];
  for (i=0;i<n;i++) //First step.
    ytemp[i]=y[i]+b21*h*dydx[i];
  derivs(x+a2*h,ytemp,ak2);// Second step.
  for (i=0;i<n;i++)
    ytemp[i]=y[i]+h*(b31*dydx[i]+b32*ak2[i]);
  derivs(x+a3*h,ytemp,ak3); //Third step.
  for (i=0;i<n;i++)
    ytemp[i]=y[i]+h*(b41*dydx[i]+b42*ak2[i]+b43*ak3[i]);
  derivs(x+a4*h,ytemp,ak4); //Fourth step.
  for (i=0;i<n;i++)
    ytemp[i]=y[i]+h*(b51*dydx[i]+b52*ak2[i]+b53*ak3[i]+b54*ak4[i]);
  derivs(x+a5*h,ytemp,ak5); //Fifth step.
  for (i=0;i<n;i++)
    ytemp[i]=y[i]+h*(b61*dydx[i]+b62*ak2[i]+b63*ak3[i]+b64*ak4[i]+b65*ak5[i]);
  derivs(x+a6*h,ytemp,ak6); //Sixth step.
  for (i=0;i<n;i++) //Accumulate increments with proper weights.
    yout[i]=y[i]+h*(c1*dydx[i]+c3*ak3[i]+c4*ak4[i]+c6*ak6[i]);
  for (i=0;i<n;i++)
    yerr[i]=h*(dc1*dydx[i]+dc3*ak3[i]+dc4*ak4[i]+dc5*ak5[i]+dc6*ak6[i]);

  //Estimate error as difference between fourth and fifth order methods.
  delete[] ytemp;
  delete[] ak6;
  delete[] ak5;
  delete[] ak4;
  delete[] ak3;
  delete[] ak2;
}



/* User storage for intermediate results. Preset kmax and dxsav in the calling program. If kmax =
   0 results are stored at approximate intervals dxsav in the arrays xp[1..kount], yp[1..nvar]
   [1..kount], where kount is output by odeint. Defining declarations for these variables, with
   memoryallo cations xp[1..kmax] and yp[1..nvar][1..kmax] for the arrays, should be in
   the calling program.*/

void RungeKutta::odeint(double *ystart, int nvar, double x1, double x2, double eps, double h1, double hmin, int *nok, int *nbad)
/* Runge-Kutta driver with adaptive stepsize control. Integrate starting values ystart[1..nvar]
   from x1 to x2 with accuracy eps, storing intermediate results in global variables. h1 should
   be set as a guessed first stepsize, hmin as the minimum allowed stepsize (can be zero). On
   output nok and nbad are the number of good and bad (but retried and fixed) steps taken, and
   ystart is replaced byv alues at the end of the integration interval. derivs is the user-supplied
   routine for calculating the right-hand side derivative, while rkqs is the name of the stepper
   routine to be used. */
{
  int nstp,i;
  double xsav=0,x,hnext,hdid,h;
  double *yscal,*y,*dydx;
  yscal=new double[nvar];
  y=new double[nvar];
  dydx=new double[nvar];
  x=x1;
  h=SIGN(h1,x2-x1);
  *nok = (*nbad) = kount = 0;
  for (i=0;i<nvar;i++) y[i]=ystart[i];
  if (kmax > 0) xsav=x-dxsav*2.0; //Assures storage of first step.
  for (nstp=0;nstp<Maxstp;nstp++) { //Take at most Maxstp steps.
    derivs(x,y,dydx);
    for (i=0;i<nvar;i++)
      /* Scaling used to monitor accuracy. This general-purpose choice can be modified
	 if need be.*/
      yscal[i]=fabs(y[i])+fabs(dydx[i]*h)+Tiny;
    if (kmax > 0 && kount < kmax-1 && fabs(x-xsav) > fabs(dxsav)) {
      xp[kount]=x; //Store intermediate results.
      for (i=0;i<nvar;i++) yp[i][kount]=y[i];
      kount++;
      xsav=x;
    }
    if ((x+h-x2)*(x+h-x1) > 0.0) h=x2-x;// If stepsize can overshoot, decrease.

    rkqs(y,dydx,nvar,&x,h,eps,yscal,&hdid,&hnext);
    if (hdid == h) ++(*nok); else ++(*nbad);
    if ((x-x2)*(x2-x1) >= 0.0) { //Are we done?
      for (i=0;i<nvar;i++) ystart[i]=y[i];
      if (kmax) {
	xp[kount]=x; //Save final step.
	for (i=0;i<nvar;i++) yp[i][kount]=y[i];
      }
      delete[] dydx;
      delete[] y;
      delete[] yscal;
      return; //Normal exit.
    }
    if (fabs(hnext) <= hmin) MyWarning::error("Step size too small in odeint");
    h=hnext;
  }
  MyWarning::error("Too many steps in routine odeint");
}

/* finis */
