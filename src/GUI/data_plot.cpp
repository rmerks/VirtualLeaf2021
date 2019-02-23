/*
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

#include <string>
#include <stdlib.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include <QDialog>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <iostream>
#include "data_plot.h"

using namespace std;

static const string _module_id("$Id$");

//
//  Initialize main window
//
DataPlot::DataPlot(QWidget *parent, const QString title, const QStringList curvenames ):
  QwtPlot(parent),
  d_interval(0),
  d_timerId(-1)
{

  // Number of curves is number of names given
  ncurves = curvenames.size();

  // allocate data and curves
  d_t = new double[PLOT_SIZE];
  d_x = new double *[ncurves];
  d_x[0] = new double[ncurves*PLOT_SIZE];
  for (int i=1;i<ncurves;i++) {
    d_x[i]=d_x[i-1]+PLOT_SIZE;
  }

  // Disable polygon clipping
  QwtPainter::setDeviceClipping(false);

  alignScales();

  //  Initialize data
  for (int i = 0; i< PLOT_SIZE; i++) {
    d_t[i] = 0.;     // time axis
  }

  for (int i=0;i<ncurves * PLOT_SIZE;i++) {
    d_x[0][i]=0.;
  }

  // Assign a title
  setTitle(title);
  insertLegend(new QwtLegend(), QwtPlot::BottomLegend);

  // Insert curves
  curves = new QwtPlotCurve[ncurves];
  for (int i=0;i<ncurves;i++) {

    curves[i].setTitle(curvenames[i]);
    curves[i].attach(this);
    curves[i].setPen(QPen(curvecolors[i]));
    QString col(curvecolors[i]);

#ifdef QDEBUG
    qDebug() << "Curvecolor " << col.toStdString() << endl;
#endif

    curves[i].setRawData(d_t, d_x[i], PLOT_SIZE);
  }

  // Axis 
  setAxisTitle(QwtPlot::xBottom, "Time");
  setAxisTitle(QwtPlot::yLeft, "Level");
  setAxisScale(QwtPlot::yLeft, 0, 10);
  data_pos = 0;
}

DataPlot::~DataPlot(void)
{
  delete[] d_t ;
  delete[] d_x[0];
  delete[] d_x;
  delete[] curves;
}

//
//  Set a plain canvas frame and align the scales to it
//
void DataPlot::alignScales()
{
  // The code below shows how to align the scales to
  // the canvas frame, but is also a good example demonstrating
  // why the spreaded API needs polishing.

  canvas()->setFrameStyle(QFrame::Box | QFrame::Plain );
  canvas()->setLineWidth(1);

  for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
      QwtScaleWidget *scaleWidget = (QwtScaleWidget *)axisWidget(i);
      if ( scaleWidget )
	scaleWidget->setMargin(0);

      QwtScaleDraw *scaleDraw = (QwtScaleDraw *)axisScaleDraw(i);
      if ( scaleDraw )
	scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }
}

//  Generate new values 
void DataPlot::AddValue(double t,double *x)
{

  //  std::cerr << "AddValue receives: " << t << ", " << y << ", " << z << std::endl;

  // Plot slowly fills up, then shifts to the left
  if ( data_pos >= PLOT_SIZE ) {

    for ( int j = 0; j < PLOT_SIZE - 1; j++ )
      d_t[j] = d_t[j+1];

    for ( int i=0;i<ncurves;i++) {
      for ( int j = 0; j < PLOT_SIZE - 1; j++ )
	d_x[i][j] = d_x[i][j+1];
    }
    data_pos = PLOT_SIZE - 1;
  } 

  d_t[data_pos] = t;

  for ( int i=0;i<ncurves;i++) {
    curves[i].setRawData(d_t, d_x[i], data_pos);
    d_x[i][data_pos] = x[i];
  }

  setAxisScale(QwtPlot::xBottom, d_t[0], t);
  data_pos++;

  // update the display
  replot();
}


PlotDialog::PlotDialog(QWidget *parent, const QString title, const QStringList curvenames):
  QDialog(parent) 
{
  plot = new DataPlot(this,title,curvenames);
  plot->resize(400,300);
  show();
}

PlotDialog::~PlotDialog(void)
{
  delete plot;
}

/* finis */
