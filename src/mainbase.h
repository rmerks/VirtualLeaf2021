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
#include <q3picture.h>
#include <qpainter.h>
#include <qwidget.h>
#include <iostream>
#include <QGraphicsItem>
#include <QPrinter>
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
    showboundaryonlyp =  false;
    showwallsp =  false;
    showfluxesp = false;
    dynamicscellsp = true;
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
  virtual bool ShowBoundaryOnlyP(void) {return showboundaryonlyp;}
  virtual bool ShowToolTipsP(void) {return showtooltipsp;}
  virtual bool ShowWallsP(void) {return showwallsp;}
 // virtual bool ShowApoplastsP(void) { return showapoplastsp;}
  virtual bool ShowFluxesP(void) { return showfluxesp; }
  virtual bool DynamicCellsP(void) { return dynamicscellsp; }
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
  virtual void XMLReadSettings(xmlNode *settings);
  virtual void XMLReadViewport(xmlNode *viewport);

  virtual double getFluxArrowsize(void) { return 10.;}

  int Save(const char *fname, const char *format, int sizex=640, int sizey=480);
  void CutSAM(void);

  void Plot(int resize_stride=10);

  virtual void UserMessage(QString message, int timeout = 0) {
    cerr << message.toAscii().constData() << endl;
  }

  Mesh &mesh;
  QTransform viewport;


 protected:
  QGraphicsScene &canvas;
  virtual xmlNode *XMLSettingsTree(void);
  virtual xmlNode *XMLViewportTree(QTransform &transform) const;


  
 protected:
  bool showcentersp;
  bool showmeshp;
  bool showbordercellp;
  bool shownodenumbersp;
  bool showcellnumbersp;
  bool showcellsaxesp;
  bool showcellstrainp;
  bool movieframesp;
  bool showboundaryonlyp;
  bool showwallsp;
//  bool showapoplastsp;
  bool showfluxesp;
  bool dynamicscellsp;
  bool showtooltipsp;
  bool hidecellsp;
};

//#include <qapplication.h>
#define TIMESTEP double MainBase::TimeStep(void)
#define INIT void MainBase::Init(const char *leaffile)

#endif

/* finis */
