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


#include <qmainwindow.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QList>
#include <QDir>
#include <QtXml>

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
  FigureEditor(QGraphicsScene&, Mesh&, QWidget* parent=0, const char* name=0, Qt::WindowFlags f=Qt::WindowFlags());
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

class Main : public QMainWindow, public MainBase {
  Q_OBJECT
    friend class ModelCatalogue; // needs to populate menu item models
 public:
  Main(QGraphicsScene&, Mesh&, QWidget* parent=0, const char* name=0, Qt::WindowFlags f=Qt::WindowFlags());
  ~Main();
  virtual bool ShowCentersP(void) {return com_act->isChecked();}
  virtual bool ShowMeshP(void) {return mesh_act->isChecked();}
  virtual bool ShowBorderCellsP(void) {return border_act->isChecked();}
  virtual bool PausedP(void) {return paused_act->isChecked();}
  virtual bool ShowNodeNumbersP(void) {return node_number_act->isChecked();}
  virtual bool ShowCellNumbersP(void) {return cell_number_act->isChecked();}
  virtual bool ShowCellAxesP(void) {return cell_axes_act->isChecked();}
  virtual bool ShowCellStrainP(void) {return cell_strain_act->isChecked();}
  virtual bool MovieFramesP(void) {return movie_frames_act->isChecked();}
  virtual bool ShowBoundaryOnlyP(void) {return only_boundary_act->isChecked();}
  virtual bool ShowWallsP(void) {return cell_walls_act->isChecked();}
 // virtual bool ShowApoplastsP(void) { return apoplasts_act->isChecked();}
  virtual bool ShowFluxesP(void) { return fluxes_act->isChecked(); }
  virtual bool DynamicCellsP(void) { return dyn_cells_act->isChecked(); }
  virtual bool RotationModeP(void) { return rotation_mode_act->isChecked(); }
  virtual bool InsertModeP(void) { return insert_mode_act->isChecked(); }
  virtual bool ShowToolTipsP(void) { return tooltips_act->isChecked(); }
    virtual bool HideCellsP(void) { return hide_cells_act->isChecked(); }
  void scale(double factor); 
  virtual double getFluxArrowsize(void)
  {
    return flux_arrow_size;
  }

  void FitCanvasToWindow();
  void FitLeafToCanvas(void);
   virtual QDomElement XMLSettingsTree();
  void setCheckedShowFluxes(bool checked) {
      fluxes_act->setChecked(checked);
  }
  void setCheckedShowTransporters(bool checked) {
      cell_walls_act->setChecked(checked);
  }
  public slots:
    void setAbout(int state);
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

      rotation_mode_act->setChecked(false);
    if (editor)
      disconnect(editor, SIGNAL(MousePressed()), this, SLOT(ExitRotationMode()));
    editor->setMouseTracking(false);
 
  }

  virtual void UserMessage(QString message, int timeout=0);
  void Refresh(void) { Plot(); }
  void PauseIfRunning(void);
  void ContIfRunning(void);
  virtual void XMLReadSettings(QDomElement &settings);

  private slots:
  void aboutQt();
  //void newView();
  void EditParameters();
  QDir GetLeafDir(void);
  void readStateXML();
  void readNextStateXML();
  void readPrevStateXML();
  void readFirstStateXML();
  void readLastStateXML();
  void exportCellData();
  void exportCellData(QString);
  void saveStateXML(void);
  void GenerateNewTissue();
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
  QMenu *file;
    QMenu *edit;
  QMenu* options;
  QMenu* output;
  QMenu *view;
  QMenu *run;
  QMenu *modelmenu;
  QMenu *helpmenu;

  QPrinter* printer;
  const QDir *working_dir;
  QString currentFile;
  //  toggle item states 
  QAction *dbf_act; // options->Double Buffer
  QAction *com_act; // view->Show centers
  QAction *mesh_act; // view->Show mesh
  QAction *node_number_act; // view->Show Node numbers
  QAction *cell_number_act; // view->Show Cell numbers
  QAction *border_act; // view->Show border cells
  QAction *paused_act; // run->Simulation paused
  QAction *cell_axes_act; // view->Show cell axes
  QAction *cell_strain_act; // view->Show cell strain
  QAction *only_boundary_act; // view ->Show only leaf boundary
  QAction *cell_walls_act; // view -> Show transporters
  QAction *walls_middle_act; // view -> Show middle lamella
  QAction *walls_stiff_act; // view -> Show wall stiffness

  //QAction *apoplasts_act; // view -> Show apoplasts
  QAction *tooltips_act; // help -> Show Cell Info
  QAction *hide_cells_act; // view->Hide Cells
  double flux_arrow_size;
  QAction *movie_frames_act;
  QAction *fluxes_act;
  QAction *dyn_cells_act;
  QAction *rotation_mode_act;
  QAction *insert_mode_act;
  QTimer *timer;
  QFile *gifanim;
  bool running;

  static const QString caption;
  static const QString caption_with_file;
  InfoBar *infobar;
};

#endif

/* finis*/
