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

#ifndef _DATA_PLOT_H_
#define _DATA_PLOT_H_

#include <QDialog>
#include <qwt_plot.h>
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <qwt_plot_curve.h>
#include "curvecolors.h"

const int PLOT_SIZE = 1000; 

class DataPlot : public QwtPlot
{
  Q_OBJECT

 public:
  DataPlot(QWidget *parent, const QString title, const QStringList curvenames);
  virtual ~DataPlot(void);
  public slots:
  void AddValue(double t, double *x);

 private:
  void alignScales();

  double *d_t; 
  double **d_x; 

  int d_interval; // timer in ms
  int d_timerId;

  QwtPlotCurve *curves;
  int data_pos;
  int ncurves;

  CurveColors curvecolors;

 protected:
  // to write contents of DataPlot to a file
  QFile *datfile;
  QTextStream datstream;
};

class PlotDialog : public QDialog {

  Q_OBJECT
    public:
  PlotDialog(QWidget *parent, const QString title, const QStringList curvenames);
  ~PlotDialog(void);
  public slots:
  void AddValue(double t, double *x) {
    //std::cerr << "AddValue receives: " << t << ", " << y << ", " << z << std::endl;
    plot->AddValue(t,x);
  }
 private:
  DataPlot *plot;
};

#endif

/* finis */
