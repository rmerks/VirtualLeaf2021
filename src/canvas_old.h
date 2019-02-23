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

#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <q3popupmenu.h>
#include <q3mainwindow.h>
#include <q3intdict.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>
#include <QDir>

#include <string>
#include <sstream>

//Added by qt3to4:
#include <QMouseEvent>
#include <vector>
#include "simitembase.h"
#include "mainbase.h"
#include "cellitem.h"
#include "infobar.h"

#include "nodeitem.h"
#include "cell.h"

#ifdef HAVE_QWT
#include "data_plot.h"
#endif

#include <libxml/xpath.h>
#include <libxml/xmlreader.h>

#if defined(Q_OS_MAC)
#define PREFIX "cmd"
#else
#define PREFIX "crtl"
#endif


class QFile;
class QDir;
class ModelCatalogue;
class InfoBar;

class FigureEditor : public QGraphicsView {
  Q_OBJECT

    friend class Main;
 public:
  FigureEditor(QGraphicsScene&, Mesh&, QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
  void clear();
  void Save(const char *fname, const char *format, int sizex=640, int sizey=480);
  void FullRedraw(void);

 protected:
  void mousePressEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void wheelEvent(QWheelEvent *event);
  void scaleView(qreal scaleFactor);

  vector <CellItem *> getIntersectedCells(void);
  void insertNode(QPointF p);
  NodeItem *selectedNodeItem(QList<QGraphicsItem *> l) const;  

 signals:
  void status(const QString&);
  void MousePressed(void);
  void MouseReleased(void);
 protected:
  Mesh &mesh;

 private:
  //NodeItem* moving;
  SimItemBase *moving;
  QPointF moving_start;

  QGraphicsLineItem *intersection_line;
  bool rotation_mode;
  bool insert_mode;
  double rot_angle;
};

class Main : public Q3MainWindow, public MainBase {
  Q_OBJECT
    friend class ModelCatalogue; // needs to populate menu item models
 public:
  Main(QGraphicsScene&, Mesh&, QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
  ~Main();
  virtual bool ShowCentersP(void) {return view->isItemChecked(com_id);}
  virtual bool ShowMeshP(void) {return view->isItemChecked(mesh_id);}
  virtual bool ShowBorderCellsP(void) {return view->isItemChecked(border_id);}
  virtual bool PausedP(void) {return run->isItemChecked(paused_id);}
  virtual bool ShowNodeNumbersP(void) {return view->isItemChecked(node_number_id);}
  virtual bool ShowCellNumbersP(void) {return view->isItemChecked(cell_number_id);}
  virtual bool ShowCellAxesP(void) {return view->isItemChecked(cell_axes_id);}
  virtual bool ShowCellStrainP(void) {return view->isItemChecked(cell_strain_id);}
  virtual bool MovieFramesP(void) {return view->isItemChecked(movie_frames_id);}
  virtual bool ShowBoundaryOnlyP(void) {return view->isItemChecked(only_boundary_id);}
  virtual bool ShowWallsP(void) {return view->isItemChecked(cell_walls_id);}
 // virtual bool ShowApoplastsP(void) { return view->isItemChecked(apoplasts_id);}
  virtual bool ShowFluxesP(void) { return view->isItemChecked(fluxes_id); }
  virtual bool DynamicCellsP(void) { return options->isItemChecked(dyn_cells_id); }
  virtual bool RotationModeP(void) { return options->isItemChecked(rotation_mode_id); }
  virtual bool InsertModeP(void) { return options->isItemChecked(insert_mode_id); }
  virtual bool ShowToolTipsP(void) { return helpmenu->isItemChecked(tooltips_id); }
  virtual bool HideCellsP(void) { return view->isItemChecked(hide_cells_id); }
  void scale(double factor); 
  virtual double getFluxArrowsize(void)
  {
    return flux_arrow_size;
  }

  void FitCanvasToWindow();
  void FitLeafToCanvas(void);

  public slots:

  void about();
  void gpl();
  void TimeStepWrap();
  void togglePaused();
  void setFluxArrowSize(int size);
  void RestartSim(void);
  void toggleShowCellCenters(void);
  void toggleShowNodes(void);
  void toggleShowBorderCells(void);
  void toggleShowFluxes(void);
  void toggleNodeNumbers(void);
  void toggleCellNumbers(void);
  void toggleCellAxes(void);
  void toggleCellStrain(void);
  void toggleShowWalls(void);
  void toggleShowApoplasts(void);
  void toggleDynCells(void);
  void toggleMovieFrames(void);
  void toggleLeafBoundary(void);
  void toggleHideCells(void);
  void print();
  void startSimulation(void);
  void stopSimulation(void);
  void RefreshInfoBar(void);
  int readStateXML(const char *filename, bool geometry = true, bool pars=true, bool simtime = true);
  
  void EnterRotationMode(void)
  {

    UserMessage("Rotation mode. Click mouse to exit.");
    if (editor) {
      editor->rot_angle = 0. ; 

      // Exit rotation mode if mouse is clicked
      connect(editor, SIGNAL(MousePressed()), this, SLOT(ExitRotationMode()));
      
      editor->setMouseTracking(true);

    }

  }
  void ExitRotationMode(void)
  { 
    UserMessage("Exited rotation mode.",2000);

    options->setItemChecked(rotation_mode_id, false); 
    if (editor)
      disconnect(editor, SIGNAL(MousePressed()), this, SLOT(ExitRotationMode()));
    editor->setMouseTracking(false);
 
  }

  virtual void UserMessage(QString message, int timeout=0);
  void Refresh(void) { Plot(); }
  void PauseIfRunning(void);
  void ContIfRunning(void);
  virtual void XMLReadSettings(xmlNode *settings);

  private slots:
  void aboutQt();
  void newView();
  void EditParameters();
  QDir GetLeafDir(void);
  void readStateXML();
  void readNextStateXML();
  void readPrevStateXML();
  void readFirstStateXML();
  void readLastStateXML();
  void exportCellData();
  void exportCellData(QString);
  void saveStateXML();
  void snapshot();
  void savePars();
  void readPars();
  void clear();
  void init();
  virtual void CutSAM() { MainBase::CutSAM(); Refresh();}

  void enlarge();
  void shrink();
  void zoomIn();
  void zoomOut();

  void CleanMesh();
  void CleanMeshChemicals(void);
  void CleanMeshTransporters(void);

  void RandomizeMesh();

 signals:
  void SimulationDone(void);
  void ParsChanged(void);

 protected:
  Mesh &mesh;

 private:
  NodeSet *node_set;
  FigureEditor *editor;
  Q3PopupMenu* options;
  Q3PopupMenu *view;
  Q3PopupMenu *run;
  QMenu *modelmenu;
  Q3PopupMenu *helpmenu;

  QPrinter* printer;
  const QDir *working_dir;
  QString currentFile;
  //  toggle item states 
  int dbf_id; // options->Double Buffer
  int com_id; // view->Show centers
  int mesh_id; // view->Show mesh
  int node_number_id; // view->Show Node numbers
  int cell_number_id; // view->Show Cell numbers
  int border_id; // view->Show border cells
  int paused_id; // run->Simulation paused
  int cell_axes_id; // view->Show cell axes
  int cell_strain_id; // view->Show cell strain
  int only_boundary_id; // view ->Show only leaf boundary
  int cell_walls_id; // view -> Show transporters
  //int apoplasts_id; // view -> Show apoplasts
  int tooltips_id; // help -> Show Cell Info
  int hide_cells_id; // view->Hide Cells
  double flux_arrow_size;
  int movie_frames_id;
  int fluxes_id;
  int dyn_cells_id;
  int rotation_mode_id;
  int insert_mode_id;
  QTimer *timer;
  QFile *gifanim;
  bool running;
  virtual xmlNode *XMLSettingsTree(void);
  static const QString caption;
  static const QString caption_with_file;
  InfoBar *infobar;
};

#endif

/* finis*/
