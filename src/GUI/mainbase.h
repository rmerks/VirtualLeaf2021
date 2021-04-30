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


#ifndef _MAINBASE_H_
#define _MAINBASE_H_

#include <QGraphicsScene>
#include <qpixmap.h>
#include <qpainter.h>
#include <qwidget.h>
#include <iostream>
#include <QGraphicsItem>
#include <QPrinter>
#include <QtXml>
#include "mesh.h"
#include "warning.h"

using namespace std;

/*! Implement these functions in your main application */
class MainBase  {

 public:
 MainBase(QGraphicsScene &c, Mesh &m) : mesh(m), canvas(c) {

    // Standard options for batch version
    showcentersp =  false;
    showmeshp =  false;
    showbordercellp =  false;
    shownodenumbersp =  false;
    showcellnumbersp =  false;
    showcellsaxesp = false;
    showcellstrainp =  false;
    movieframesp = true;
    exportdatap = false;
    ignorep = false;
    showboundaryonlyp =  false;
    showwallsp =  false;
    showfluxesp = false;
    MonteCp = true;
    metrop = true;
    dynamicscellsp = true;
    rigidp = false;
    remodp = false;
    vertexp = false;
    duplip = false;
    removep = false;
    showtooltipsp = false;
    hidecellsp = false;
  }
  virtual ~MainBase() {};

  virtual double TimeStep();
  virtual void Init(const char *leaffile=0);

  virtual bool ShowCentersP(void) {return showcentersp;}
  virtual bool ShowMeshP(void) {return showmeshp; }
  virtual bool ShowBorderCellsP(void) {return showbordercellp; }
  virtual bool PausedP(void) {return false; }
  virtual bool ShowNodeNumbersP(void) {return shownodenumbersp; }
  virtual bool ShowCellNumbersP(void) {return showcellnumbersp;}
  virtual bool ShowCellAxesP(void) {return showcellsaxesp;}
  virtual bool ShowCellStrainP(void) {return showcellstrainp;}
  virtual bool MovieFramesP(void) {return movieframesp;}
  virtual bool ExportDataP(void) {return exportdatap;}
  virtual bool IgnoreP(void) {return ignorep;}
  virtual bool ShowBoundaryOnlyP(void) {return showboundaryonlyp;}
  virtual bool ShowToolTipsP(void) {return showtooltipsp;}
  virtual bool ShowWallsP(void) {return showwallsp;}
  // virtual bool ShowApoplastsP(void) { return showapoplastsp;}
  virtual bool ShowFluxesP(void) { return showfluxesp; }
  virtual bool MonteCarloP(void) { return MonteCp; }
  virtual bool MetroP(void) { return metrop; }
  virtual bool DynamicCellsP(void) { return dynamicscellsp; }
  virtual bool RigidityP(void) { return rigidp; }
  virtual bool RemodP(void) { return remodp; }
  virtual bool VertexP(void) { return vertexp; }
  virtual bool DupliP(void) { return duplip; }
  virtual bool RemoveP(void) { return removep; }
  virtual void FitCanvasToWindow() {};
  virtual void FitLeafToCanvas() {};
  virtual bool HideCellsP(void) { return hidecellsp; }
  virtual void clear(void) {
    QList<QGraphicsItem *> list = canvas.items();
    QList<QGraphicsItem *>::Iterator it = list.begin();
    for (; it != list.end(); ++it) {
      if ( *it )
	delete *it;
    }
  };
  virtual void XMLReadSettings(QDomElement &settings);
  virtual void XMLReadViewport(QDomElement &viewport);

  virtual double getFluxArrowsize(void) { return 10.;}

  int Save(const char *fname, const char *format, int sizex=640, int sizey=480);
  void CutSAM(void);

  void Plot(int resize_stride=10);

  virtual void UserMessage(QString message, int timeout = 0) {
    cerr << message.toLatin1().constData() << endl;
  }

  Mesh &mesh;
  QTransform viewport;
  virtual QDomElement XMLSettingsTree();

 protected:
  QGraphicsScene &canvas;

  virtual QDomElement XMLViewportTree( QTransform &transform) const;


  
 protected:
  bool showcentersp;
  bool showmeshp;
  bool showbordercellp;
  bool shownodenumbersp;
  bool showcellnumbersp;
  bool showcellsaxesp;
  bool showcellstrainp;
  bool movieframesp;
  bool exportdatap;
  bool ignorep;
  bool showboundaryonlyp;
  bool showwallsp;
//  bool showapoplastsp;
  bool showfluxesp;
  bool MonteCp;
  bool metrop;
  bool dynamicscellsp;
  bool rigidp;
  bool remodp;
  bool vertexp;
  bool duplip;
  bool removep;
  bool showtooltipsp;
  bool hidecellsp;

};

//#include <qapplication.h>
#define TIMESTEP double MainBase::TimeStep(void)
#define INIT void MainBase::Init(const char *leaffile)

#endif

/* finis */
