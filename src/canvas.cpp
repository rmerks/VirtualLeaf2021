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

#include <time.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <qdatetime.h>
#include <q3mainwindow.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qlabel.h>
#include <qimage.h>
#include <q3progressdialog.h>
#include <qtimer.h>
#include <qslider.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qdir.h>
#include <q3filedialog.h>
#include <QGraphicsItem>
#include <QList>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QImageWriter>

#include <set>

//Added by qt3to4:
#include <Q3ValueList>
#include <Q3PopupMenu>
#include <QMouseEvent>
#include <typeinfo>
#include <cstring>
#include <q3process.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <fstream>
#include <sstream>
#include "pardialog.h"
#include "parameter.h"
#include "canvas.h"
#include "node.h"
#include "nodeset.h"
#include "nodeitem.h"
#include "cellitem.h"
#include "wallitem.h"
#include "mesh.h"
#include "xmlwrite.h"
#include "OptionFileDialog.h"
#include <cstdlib>
#include <cstdio>
#include "modelcatalogue.h"

#include <algorithm>

// Include PSB, CWI and vleaf logos
#include "psb.xpm"
#include "cwi.xpm"
#include "leaficon_small.xpm"

static const std::string _module_id("$Id$");

using namespace std;

// We use a global variable to save memory - all the brushes and pens in
// the mesh are shared.

#define FNAMESIZE 100
#define QUOTE_ME(s) QUOTE_ME_2NDLEV(s)
#define QUOTE_ME_2NDLEV(s) #s

static QColor dark_red("darkRed");


static const int imageRTTI = 984376;
extern Parameter par;
const QString Main::caption("Virtual leaf");
const QString Main::caption_with_file("Virtual leaf: %1");

FigureEditor::FigureEditor(
			   QGraphicsScene& c, Mesh &m, QWidget* parent,
			   const char* name, Qt::WindowFlags f) :
  QGraphicsView(&c,parent), mesh(m)
{
  intersection_line = 0;
  //angle_line = 0;
  setInteractive(true);
  moving = 0;  
  rotation_mode = false;
}


void FigureEditor::clear()
{
  QList<QGraphicsItem *> list = scene()->items();
  QList<QGraphicsItem *>::Iterator it = list.begin();
  for (; it != list.end(); ++it) {
    delete *it;
  }
}

void FigureEditor::wheelEvent(QWheelEvent *event)
{
  scaleView(pow((double)2, -event->delta() / 240.0));
}


void FigureEditor::scaleView (qreal scaleFactor)
{
  qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)). width();
  if (factor < 0.07 || factor > 100) return;
  scale (scaleFactor, scaleFactor);
}

void FigureEditor::Save(const char *fname, const char *format, int sizex, int sizey)
{

  QImage *image = new QImage(sizex, sizey, QImage::Format_RGB32);
  image->fill(QColor(Qt::white).rgb());
  QPainter *painter=new QPainter(image);

  render(painter);

  image->save(QString(fname),format);
  delete painter;
  delete image;
}

void FigureEditor::mousePressEvent(QMouseEvent* e)
{
  static QList<Node*> selected;
  emit MousePressed();

  //QPointF p = matrix().inverted().map(e->pos());
  QPointF p = mapToScene(e->pos());

#ifdef QDEBUG  
  qDebug() << endl << "MousePressEvent location: (" << p.x() << "," << p.y() << ")." << endl;
  qDebug() << "Magnification:  " << Cell::Magnification() << endl;
  qDebug() << "Offsets:  " << Cell::Offset() << endl;
#endif


  QList<QGraphicsItem *> l=scene()->items(p);

#ifdef QDEBUG  
  qDebug() << "MousePressEvents, items: " << l.size() << endl;
  qDebug() << "Mouse button modifier: " << e->modifiers() << endl;
#endif

  if (e->button()==Qt::RightButton || l.size()==0) {

    //cerr << "Drawing an intersection line from " << p.x() << ", " << p.y() << endl;
    intersection_line = new QGraphicsLineItem( 0, scene() );
    intersection_line->setPen( QPen( QColor("red"), 3, Qt::DashLine ) );
    intersection_line->setLine( QLineF(p,p) );
    intersection_line->setZValue( 100 );
    intersection_line->show();
  }

  for (QList<QGraphicsItem *>::Iterator it=l.begin(); it!=l.end(); ++it) {
#ifdef QDEBUG
    qDebug() << typeid(**it).name() << endl;
#endif

    if ( !strcmp(typeid(**it).name(),"8NodeItem")) {

      stringstream data_strstream;
      data_strstream << (dynamic_cast<NodeItem*>(*it))->getNode();
      dynamic_cast<Main *>(parent())->UserMessage(QString(data_strstream.str().c_str()));

      (dynamic_cast<NodeItem*>(*it))->OnClick(e->button());
    }
    else 
      if ( !strcmp(typeid(**it).name(),"8CellItem") ) {

	Cell &c=((dynamic_cast<CellItem *>(*it))->getCell());      
	// OnClick to be defined in end-user code
	c.OnClick(e);
      } else {
	if ( !strcmp(typeid(**it).name(),"8WallItem") ) {
	  (dynamic_cast<WallItem *>(*it))->OnClick(e);
	} 
      }
  }

  FullRedraw();
  moving = 0;
}

void FigureEditor::mouseMoveEvent(QMouseEvent* e)
{

  // User chooses "rotation mode" and we can rotate the object around its center of mass
  if (dynamic_cast<Main *>(parent())->RotationModeP()) {

    QPointF p = mapToScene(e->pos());
    p.setX(p.x() / Cell::Magnification());
    p.setY(p.y() / Cell::Magnification());


    // get object's center of mass
    QPointF rotation_midpoint = mesh.Centroid()*Cell::Factor() - Cell::Offset();


    // calculate rotation angle
    double dy = (rotation_midpoint.y() - p.y());
    double dx = (rotation_midpoint.x() - p.x());
    double new_rot_angle = atan2(dx, dy);
    double d_alpha = new_rot_angle - rot_angle;
    rot_angle = new_rot_angle;

    mesh.Rotate(d_alpha, ( Vector(rotation_midpoint) + Cell::Offset() ) / Cell::Factor() );

    dynamic_cast<Main *>(parent())->Plot(0);
    FullRedraw();
    return;
  }
 
  
  if ( moving ) {

    QPointF p = mapToScene(e->pos());
    moving->userMove(p.x() - moving_start.x(),
		     p.y() - moving_start.y());
    moving_start = p;
    scene()->update();

  }

  //cerr << "event";

  // keep track of intersection line to interactively cut a growing leaf

  if ( intersection_line ) {

    QPointF sp = intersection_line -> line().p1(); // startpoint
    QPointF ep = mapToScene(e->pos()); // endpoint
    intersection_line -> setLine( QLineF(sp, ep) ); 
    scene()->update();
    // Need this for Mac
    FullRedraw();
  }
}

//void FigureEditor::contentsMouseReleaseEvent(QMouseEvent* e)
void FigureEditor::mouseReleaseEvent(QMouseEvent* e)
{

  emit MouseReleased();
  // intersection line for leaf was finished now.

  if (e->button()==Qt::LeftButton) { 
    if (intersection_line ) {
#ifdef QDEBUG
      qDebug() << "Trying to cut leaf" << endl;
#endif
      QPointF sp = intersection_line -> line().p1(); // startpoint
      QPointF ep = mapToScene(e->pos());

      intersection_line -> setLine( QLineF(sp, ep) ); 
      intersection_line -> show();

      vector <CellItem *> intersected_cells = getIntersectedCells();

      // no cells selected, do nothing
      if (intersected_cells.size()==0) {
#ifdef QDEBUG
	qDebug() << "No cells detected :-(" << endl;
#endif
	return;
      }


      Vector startpoint = Vector(sp.x(), sp.y()) / Cell::Factor() - Cell::Offset();
      Vector endpoint = Vector(ep.x(), ep.y()) / Cell::Factor() - Cell::Offset();

      NodeSet *node_set = new NodeSet;

      for (vector<CellItem *>::iterator it = intersected_cells.begin(); it != intersected_cells.end(); it++) {
	(*it)->setBrush(QBrush("purple"));
	
	Cell &c=(*it)->getCell();

	// sometimes the cell hasn't properly divided yet before the
	// next division is called?  so check for it?  let's find a way
	// to do this later. Note that this functionality currently
	// might result in a segmentation fault for users who are
	// quickly dragging and releasing division lines...
	scene()->update();

#ifdef QDEBUG
	qDebug() << "Dividing Cell " << c.Index() << endl;
#endif

	c.DivideOverGivenLine( startpoint, endpoint, true, node_set);
      }

      node_set->CleanUp();
      mesh.AddNodeSet(node_set);

#ifdef QDEBUG
      qDebug() << "Done DivideOverGivenLine" << endl;
#endif

      mesh.TestIllegalWalls();
      // Do the actual cutting and removing
      if (intersected_cells.size()) {
	mesh.CutAwayBelowLine( startpoint, endpoint ); 

	// Correct flags of nodeset
	for (NodeSet::iterator i = node_set->begin(); i != node_set->end(); i++) {
	  (*i)->SetSAM();
	  (*i)->SetBoundary();
	}

	// Make cells attached to nodeset part of the boundary
	// This is a temporary solution for the following:
	//   If a cell attached to a NodeSet divides (with a division plane intersecting the node set), 
	//   we must insert a new node into the node set.
	// For now, we add a layer of "virtual cells" inbetween. 
	list<Cell *> cells_attached_to_nodeset = node_set->getCells();
	for ( list<Cell *>::iterator c = cells_attached_to_nodeset.begin(); c != cells_attached_to_nodeset.end(); c++) {
	  (*c)->SetBoundary(Cell::SAM);
	}



#ifdef QDEBUG
	qDebug() << "Done CutAwayBelowLine" << endl;
#endif
	mesh.TestIllegalWalls();
	mesh.RepairBoundaryPolygon();
#ifdef QDEBUG
	qDebug() << "Done RepairBoundaryPolygon" << endl;
#endif
	mesh.TestIllegalWalls();
	mesh.CleanUpWalls();
#ifdef QDEBUG
	qDebug() << "Done CleanUpWalls" << endl;
#endif
	mesh.TestIllegalWalls();
      }

      dynamic_cast<Main *>(parent())->Plot();

#ifdef QDEBUG
      cerr << "NodeSet of cutting line: " << *node_set << endl;
#endif
    }
  } else 
    if (e->button()==Qt::RightButton) {

      if (intersection_line /* && !angle_line*/) {

	QPointF p = mapToScene(e->pos());
	QPointF sp = intersection_line->line().p1();

	viewport()->setMouseTracking( TRUE );
      } 
    }
}


// returns a vector of pointer to cells colliding with intersection line
vector <CellItem *> FigureEditor::getIntersectedCells(void)
{ 
  vector <CellItem *> colliding_cells;

  QList<QGraphicsItem *> l = intersection_line->collidingItems( );

#ifdef QDEBUG
  qDebug() <<  "l.size() = " << l.size() << endl;
#endif

  for (QList<QGraphicsItem *>::Iterator it=l.begin(); it!=l.end(); ++it) {

#ifdef QDEBUG
    qDebug() << typeid(**it).name() << endl;
#endif

    if ( !strcmp(typeid(**it).name(),"8CellItem") ) {
      colliding_cells.push_back(dynamic_cast<CellItem *>(*it));
    }
  }

  delete intersection_line;
  intersection_line = 0;
  return colliding_cells;
}

void FigureEditor::FullRedraw(void)
{
  QList<QRectF> rl;
  rl << sceneRect();
  updateScene(rl);   
}


NodeItem *FigureEditor::selectedNodeItem(QList<QGraphicsItem *> graphicItems) const
{
  NodeItem *nodeItem = 0;
  // graphicItems is a list of the QgraphicItems under the mouse click event 
  QList<QGraphicsItem *>::Iterator it = graphicItems.begin();
  for (; it != graphicItems.end(); ++it) {
    if ( !strcmp(typeid(**it).name(),"8NodeItem")) {
      // the object under the mouse click is a Nodeitem
      nodeItem = dynamic_cast<NodeItem *>(*it);
      // indicate we've selected it
      nodeItem->setBrush(dark_red);
      break;
    }
  }
  return nodeItem;
}


void FigureEditor::insertNode(QPointF p)
{
  Node *node = new Node(p.x(), p.y(), 0);
  mesh.AddNode(node);
  scene()->clearSelection();
  dynamic_cast<Main *>(parent())->Plot();
  FullRedraw();
#ifdef QDEBUG  
  qDebug() << "Node: " << p << endl;
#endif
}

static uint mainCount = 0;

Main::Main(QGraphicsScene& c, Mesh &m, QWidget* parent, const char* name, Qt::WindowFlags f) :
  Q3MainWindow(parent,name,f),
  MainBase(c,m),
  mesh(m)
{
  editor = new FigureEditor(canvas,mesh, this);

#ifdef QDEBUG
  qDebug() << "Interactive = " << editor->isEnabled();
#endif

  working_dir = 0;
  QObject::connect( editor, SIGNAL(MousePressed()), this, SLOT(PauseIfRunning()));
  QObject::connect( editor, SIGNAL(MouseReleased()), this, SLOT(ContIfRunning()));
  QMenuBar* menu = menuBar();

  Q3PopupMenu* file = new Q3PopupMenu( menu );

  file->insertItem("&Read leaf", this, SLOT(readStateXML()));
  file->insertItem("&Save leaf", this, SLOT(saveStateXML()));
  file->insertItem("Snapshot", this, SLOT(snapshot()), Qt::CTRL+Qt::SHIFT+Qt::Key_S);

  file->insertSeparator();
  file->insertItem("Read next leaf", this, SLOT(readNextStateXML()), Qt::Key_PageDown);
  file->insertItem("Read previous leaf", this, SLOT(readPrevStateXML()), Qt::Key_PageUp);
  file->insertItem("Read last leaf", this, SLOT(readLastStateXML()), Qt::Key_End);
  file->insertItem("Read first leaf", this, SLOT(readFirstStateXML()), Qt::Key_Home);
  file->insertItem("Export cell data", this, SLOT(exportCellData()));

  file->insertSeparator();
  file->insertItem("&Print...", this, SLOT(print()), Qt::CTRL+Qt::Key_P);
  file->insertSeparator();
  file->insertItem("E&xit", qApp, SLOT(quit()), Qt::CTRL+Qt::Key_Q);
  menu->insertItem("&File", file);

  Q3PopupMenu* edit = new Q3PopupMenu( menu );
  edit->insertItem("Reset Chemicals and Transporters", this, SLOT( CleanMesh()), Qt::CTRL+Qt::Key_R );
  edit->insertItem("Reset Chemicals", this, SLOT( CleanMeshChemicals()) );
  edit->insertItem("Reset Transporters", this, SLOT( CleanMeshTransporters()) );
  edit->insertItem("Randomize PIN1 Transporters", this, SLOT( RandomizeMesh()) );
  edit->insertItem("Cut away SAM", this, SLOT( CutSAM() ));
  menu->insertItem("&Edit", edit);

  run = new Q3PopupMenu( menu );
  running = false;
  paused_id = run->insertItem("&Simulation paused", this, SLOT(togglePaused()), Qt::Key_S);
  run->setItemChecked(paused_id, FALSE);

  menu->insertItem("&Run", run);

  view = new Q3PopupMenu( menu );
  view->insertItem("&Zoom in", this, SLOT(zoomIn()), Qt::CTRL+Qt::Key_Equal);
  view->insertItem("Zoom &out", this, SLOT(zoomOut()), Qt::CTRL+Qt::Key_Minus);
  view->insertSeparator();
  com_id = view->insertItem("Show cell &centers", this, SLOT(toggleShowCellCenters()));
  view->setItemChecked(com_id, FALSE);

  mesh_id = view->insertItem("Show &nodes", this, SLOT(toggleShowNodes()), Qt::CTRL+Qt::SHIFT+Qt::Key_N);
  view->setItemChecked(mesh_id, TRUE);
  node_number_id = view->insertItem("Show node numbers", this, SLOT(toggleNodeNumbers()), Qt::CTRL+Qt::SHIFT+Qt::Key_M);
  view->setItemChecked(node_number_id, FALSE);
  cell_number_id = view->insertItem("Show cell numbers", this, SLOT(toggleCellNumbers()));
  view->setItemChecked(cell_number_id, FALSE);
  hide_cells_id = view->insertItem("Hide cells", this, SLOT(toggleHideCells()));
  view->setItemChecked(hide_cells_id, FALSE);
  border_id = view->insertItem("Show &border cells", this, SLOT(toggleShowBorderCells()));
  view->setItemChecked(border_id, FALSE);
  cell_axes_id = view->insertItem("Show cell &axes", this, SLOT(toggleCellAxes()));
  cell_strain_id = view->insertItem("Show cell &strain", this, SLOT(toggleCellStrain()));
  view->setItemChecked(cell_axes_id, FALSE);
  fluxes_id = view->insertItem("Show &fluxes", this, SLOT(toggleShowFluxes()));
  view->setItemChecked(fluxes_id, FALSE);
  cell_walls_id = view->insertItem("Show transporters", this, SLOT(toggleShowWalls()));
  view->setItemChecked(cell_walls_id, FALSE);
 // apoplasts_id = view->insertItem("Show apoplasts", this, SLOT(toggleShowApoplasts()));
 // view->setItemChecked(apoplasts_id, FALSE);
  view->insertSeparator();
  only_boundary_id = view->insertItem("Show only leaf &boundary", this, SLOT(toggleLeafBoundary()));
  view->insertSeparator();
  movie_frames_id = view->insertItem("Start saving movie &frames", this, SLOT(toggleMovieFrames()));
  view->setItemChecked(movie_frames_id, par.movie);

  view->setItemChecked(only_boundary_id, FALSE);
  menu->insertItem("&View", view);


  options = new Q3PopupMenu( menu );
  dyn_cells_id = options->insertItem("Cell growth", this, SLOT(toggleDynCells()));
  options->setItemChecked(dyn_cells_id, true);

  options->insertItem("Edit &parameters", this, SLOT(EditParameters()), Qt::CTRL+Qt::Key_E);

  rotation_mode_id = options->insertItem("Rotate leaf", this, SLOT(EnterRotationMode()), Qt::CTRL + Qt::SHIFT + Qt::Key_R);
  options->setItemChecked(rotation_mode_id, false);

  menu->insertItem("&Options",options);

  // Menu of models
  modelmenu = new QMenu( menu );
  menu->insertItem("&Models", modelmenu);


  menu->insertSeparator();

  helpmenu = new Q3PopupMenu( menu );
  tooltips_id = helpmenu->insertItem("Show Cell&Info", this, SLOT(Refresh()));
  helpmenu->setItemChecked(tooltips_id, true);
  helpmenu->insertSeparator();
    //helpmenu->insertSeparator();
  helpmenu->insertItem("&LICENSE", this, SLOT(gpl()) );
  helpmenu->insertItem("About", this, SLOT(about()) ); //, Key_F1);
	
	menu->insertItem("&Help",helpmenu);
  statusBar();
  setCentralWidget(editor);
  printer = 0;
  init();

  // Start timer which repetitively invokes
  // a simulation time step
  timer = new QTimer( this );
  connect( timer, SIGNAL(timeout()), SLOT(TimeStepWrap()) );

  stopSimulation();
  statusBar()->addWidget(new QLabel("Ready."));
  setCaption(caption);
  gifanim = 0;

  infobar = new InfoBar();
  addDockWindow(infobar);

}

void Main::RefreshInfoBar(void)
{
  infobar->SetText(mesh.ModelID());
}


void Main::UserMessage(QString message, int timeout)
{
  statusBar()->showMessage(message, timeout);
}


void Main::init()
{
  clear();

  static int r=24;
  srand(++r);

  mainCount++;
}

Main::~Main()
{
  delete printer;
  if ( !--mainCount ) {
  }
  //EndGifAnim();
}

void Main::newView()
{
  // Open a new view... have it delete when closed.
  Main *m = new Main(canvas, mesh, 0, 0, Qt::WDestructiveClose);
  qApp->setMainWidget(m);
  m->show();
  qApp->setMainWidget(0);
}


void Main::EditParameters()
{

  ParameterDialog *pardial = new ParameterDialog(this, "stridediag");

  // Make sure the values in the parameter dialog are updated after a file is read 
  // each method changing the parameters (reading XML or PAR files) should
  // emit this signal
  QObject::connect(   this, SIGNAL( ParsChanged() ), pardial, SLOT( Reset() ) );
}

void Main::savePars()
{

  stopSimulation();

  Q3FileDialog *fd = new Q3FileDialog( this, "file dialog", TRUE );
  fd->setMode( Q3FileDialog::AnyFile );
  fd->setFilter( "Parameter files (*.par)");
  fd->setDir(par.datadir);

  QString fileName;
  if ( fd->exec() == QDialog::Accepted ) {
    fileName = fd->selectedFile();
    ofstream parfile((const char *)fileName);
    par.Write(parfile);
  }

  startSimulation();
}

void Main::readPars()
{

  stopSimulation();

  Q3FileDialog *fd = new Q3FileDialog( this, "file dialog", TRUE );
  fd->setMode( Q3FileDialog::ExistingFile );
  fd->setFilter( "Parameter files (*.par)");
  fd->setDir(par.datadir);

  QString fileName;
  if ( fd->exec() == QDialog::Accepted ) {
    fileName = fd->selectedFile();
    par.Read((const char *)fileName);
  }

  emit ParsChanged();
}


void Main::saveStateXML()
{

  stopSimulation();
  Q3FileDialog *fd = new Q3FileDialog( this, "file dialog", TRUE );
  fd->setMode( Q3FileDialog::AnyFile );
  fd->setFilter( "LeafML files (*.xml)");
  fd->setDir(par.datadir);
  QString fileName;

  if ( fd->exec() == QDialog::Accepted ) {
    fileName = fd->selectedFile();

    // extract extension from filename
    QFileInfo fi(fileName);
    QString extension = fi.suffix();
    
    if (extension.isEmpty()) {
      extension = "xml";
      fileName += ".";
      fileName += extension;
    }
    
    
    if ( QFile::exists( fileName ) &&
	 QMessageBox::question(
			       this,
			       tr("Overwrite File? -- Leaf Growth"),
			       tr("A file called %1 already exists."
				  " Do you want to overwrite it?")
			       .arg( fileName ),
			       tr("&Yes"), tr("&No"),
			       QString::null, 1, 1 ) ) {
      return saveStateXML();

    } else {

      mesh.XMLSave((const char *)fileName, XMLSettingsTree());
      QString status_message;
      status_message = QString("Wrote LeafML to %1").arg(fileName);
      cerr << status_message.toStdString().c_str() << endl;
      statusBar()->showMessage(status_message);
    }
  }
}



void Main::snapshot()
{

#ifdef QDEBUG
  qDebug() << "Supported Image Formats: " << QImageWriter::supportedImageFormats() << endl;
#endif

  stopSimulation();
  Q3FileDialog *fd = new Q3FileDialog( this, "Save snapshot", TRUE );
  fd->setDir(par.datadir);
  fd->setMode( Q3FileDialog::AnyFile );

  QString supported_file_formats = " *.pdf";
  foreach (QString format, QImageWriter::supportedImageFormats()){
    supported_file_formats += (" *." + format);
  }

  fd->setFilter("Image files (" + supported_file_formats + " )");

  QString fileName;
  if ( fd->exec() == QDialog::Accepted ) {
    fileName = fd->selectedFile();
    
    // extract extension from filename
    QFileInfo fi(fileName);
    QString extension = fi.suffix();
    
    if (extension.isEmpty()) {
      extension = "png";
      fileName += ".";
      fileName += extension;
    }
    
    
    if ( QFile::exists( fileName ) &&
	 QMessageBox::question(
			       this,
			       tr("Overwrite file? -- Cell data export"),
			       tr("A file called %1 already exists."
				  " Do you want to overwrite it?").arg( fileName ),
			       QMessageBox::Yes, QMessageBox::No 
			       ) == QMessageBox::No
	 ) {
      
      return snapshot();
      
    } else {

      // Save bitmaps at 1024x768
      QString status_message;
      if (Save((const char *)fileName, extension, 1024, 768)==0) {
	status_message = QString("Wrote snapshot to %1").arg(fileName);
      } else {
	status_message = QString("Error writing snapshot to %1").arg(fileName);
      }
      cerr << status_message.toStdString().c_str() << endl;
      statusBar()->showMessage(status_message);

    }
  }
}



void Main::readPrevStateXML()
{

  // if we have already read a file, read the next file
  if (!currentFile.isEmpty() && working_dir) {
    QString next_file;

    QStringList xml_files = working_dir->entryList("*.xml");
    QString currentFile_nopath = currentFile.section( '/', -1 );
    QString currentFile_path = currentFile.section( '/', 0, -2 );

    QList<QString>::iterator f = xml_files.find( currentFile_nopath );

    if (f == xml_files.end()) {
      return;
    }

    if (f==xml_files.begin()) {
      QMessageBox mb( "Read previous leaf",
		      "No more files",
		      QMessageBox::Information,
		      QMessageBox::Ok | QMessageBox::Default,
		      QMessageBox::NoButton,
		      QMessageBox::NoButton);
      mb.exec();
      return;
    }
    next_file = *(--f);
    next_file = currentFile_path+"/"+next_file;

    readStateXML((const char *)next_file);

  }
}

int Main::readStateXML(const char *filename, bool geometry, bool pars, bool simtime)
{

  try {
    xmlNode *settings;
    mesh.XMLRead((const char *)filename, &settings, geometry, pars, simtime);
#ifdef QDEBUG
    qDebug() << "Reading done."<< endl;
#endif
    XMLReadSettings(settings);
    xmlFree(settings);
    Cell::SetMagnification(1);
    Cell::setOffset(0,0);

    FitLeafToCanvas();
    
    currentFile =  QString(filename);

    Plot();
    QString status_message = QString("Successfully read leaf from file %1. Time is %2 h.").arg(currentFile).arg(mesh.getTimeHours().c_str());
    cerr << status_message.toStdString().c_str() << endl;
    setCaption(caption_with_file.arg(filename));
    statusBar()->message(status_message);
    emit ParsChanged();
#ifdef QDEBUG
    qDebug() << "Done. Returning 0." << endl;
#endif
    return 0;
  } catch (const char *error_message) {
    QMessageBox mb( "Read leaf from XML file",
		    QString(error_message),
		    QMessageBox::Critical,
		    QMessageBox::Ok | QMessageBox::Default,
		    Qt::NoButton,
		    Qt::NoButton);
    mb.exec();
    return 1;
  }
}


void Main::readNextStateXML()
{

  // if we have already read a file, read the next file
  if (!currentFile.isEmpty() && working_dir) {
    QString next_file;

    QStringList xml_files = working_dir->entryList("*.xml");
    QString currentFile_nopath = currentFile.section( '/', -1 );
    QString currentFile_path = currentFile.section( '/', 0, -2 );


    QList<QString>::iterator f = xml_files.find( currentFile_nopath );

    if (f == xml_files.end()) {
      return;
    }

    ++f;
    if (f==xml_files.end()) {
      QMessageBox mb( "Read next leaf",
		      "No more files",
		      QMessageBox::Information,
		      QMessageBox::Ok | QMessageBox::Default,
		      QMessageBox::NoButton,
		      QMessageBox::NoButton);
      mb.exec();
      return;
    }
    next_file = *f;
    next_file = currentFile_path+"/"+next_file;

    readStateXML((const char*)next_file);
  }
}

void Main::readLastStateXML()
{

  // if we have already read a file, read the next file
  if (!currentFile.isEmpty() && working_dir) {
    QString next_file;

    QStringList xml_files = working_dir->entryList("*.xml");
    QString currentFile_nopath = currentFile.section( '/', -1 );
    QString currentFile_path = currentFile.section( '/', 0, -2 );


    next_file = xml_files.back();

    next_file = currentFile_path+"/"+next_file;

    readStateXML((const char*)next_file);
  }
}


void Main::readFirstStateXML()
{

  // if we have already read a file, read the next file
  if (!currentFile.isEmpty() && working_dir) {
    QString next_file;

    QStringList xml_files = working_dir->entryList("*.xml");
    QString currentFile_nopath = currentFile.section( '/', -1 );
    QString currentFile_path = currentFile.section( '/', 0, -2 );


    next_file = xml_files.front();

    next_file = currentFile_path+"/"+next_file;

    readStateXML((const char*)next_file);
  }
}

QDir Main::GetLeafDir(void) {
   
  QDir LeafDir(QApplication::applicationDirPath()); 
  QStringList plugin_filters; // filter for plugins, i.e "*.dll", "*.dylib"
  
  
#if defined(Q_OS_WIN) 
  if (LeafDir.dirName().toLower() =="debug" 
      ||LeafDir.dirName().toLower() =="release") 
    LeafDir.cdUp(); 
  //plugin_filters << "*.dll";
#elif defined(Q_OS_MAC) 
  if (LeafDir.dirName() =="MacOS"){ 
    LeafDir.cdUp(); 
    LeafDir.cdUp(); 
    LeafDir.cdUp(); 
  }
  
#endif
  // for all OS-es. Move from "bin" directory to root application folder.
  if (LeafDir.dirName() == "bin") {
    LeafDir.cdUp();
  }

  LeafDir.cd("data/leaves");
  /* if (!LeafDir.cd("data/leaves")) {
    QString status_message = QString("No directory data/leaves");
    statusBar()->showMessage(status_message);
    
    return LeafDir;
  } 
  */
  return LeafDir;
      
}
 

void Main::readStateXML()
{

  //  extern Mesh mesh;

  stopSimulation();
#ifdef QDEBUG
  qDebug() << "Trying to open an OptionFileDialog" << endl;
#endif
  OptionFileDialog *fd = new OptionFileDialog( this, "read dialog", TRUE );
  fd->setMode( OptionFileDialog::ExistingFile );
  fd->setFilter( "LeafML files (*.xml)");
  if (working_dir) {
    fd->setDir(*working_dir);
  } else {
    fd->setDir(par.datadir);
  }
  QString fileName;
  if ( fd->exec() == QDialog::Accepted ) {

    fileName = fd->selectedFile();
    if (working_dir) {
      delete working_dir;
    }
    working_dir = fd->dir();

    if (readStateXML((const char *)fileName,fd->readGeometryP(), fd->readParametersP()) )
      return readStateXML(); // user can try again
  }
}


void Main::clear()
{
  editor->clear();
}

void Main::about()
{
  static QMessageBox* about = new QMessageBox
    ( "VirtualLeaf V1.0.2",
      "<h3>VirtualLeaf V1.0.2</h3>\
       <p>\
         An Open Source framework for cell-based modeling of plant\
         tissue growth and development.\
       </p>\
       <p>(c) 2005-2008, Roeland Merks <i>et al.</i>\
         <a href=\"http://www.psb.vib-ugent.be\">VIB Department Plant Systems Biology</a>,\
         Ghent, Belgium.\
       </p>\
       <p>(c) 2008-2014,\
         <a href=\"http://www.cwi.nl/~merks\">Roeland Merks <i>et al.</i></a>\
         <a href=\"http://www.cwi.nl\">Centrum Wiskunde & Informatica (CWI)</a> and\
         <a href=\"http://www.ncsb.nl\"> Netherlands Consortium for Systems Biology (NCSB)</a>,\
         Amsterdam, the Netherlands.\
       </p>\
       <p>\
         VirtualLeaf is free software: you can redistribute it and/or\
         modify it under the terms of the GNU General Public License as\
         published by the Free Software Foundation, either version 3 of the\
         License, or (at your option) any later version.\
       </p>\
       <p>\
         If you use this code for your projects, please cite our paper in\
         <a href=\"http://www.plantphysiol.org\">Plant Physiology</a>, &lsquo;\
         Roeland M. H. Merks, Michael Guravage, Dirk Inz&eacute;, and Gerrit T.S. Beemster,\
         <a href=\"http://www.plantphysiol.org/cgi/reprint/155/2/656.pdf\">\
         VirtualLeaf: an Open Source framework for cell-based modeling of plant tissue growth and development</a>,<br>\
         Plant Physiology 2011: 155:656-666.\
       </p>\
       <p>\
         Please share your model plugins and extensions at\
         <a href=\"http://virtualleaf.googlecode.com\">http://virtualleaf.googlecode.com</a>.\
       </p>",
      QMessageBox::Information, 1, 0, 0, this, 0, FALSE );
  about->setButtonText( 1, "Dismiss" );
  about->setIconPixmap(QPixmap( leaficon_small ));
  about->show();
}


void Main::gpl()
{
  static QMessageBox* gpl = new QMessageBox ( "GPL License", "", 
      QMessageBox::Information, 1, 0, 0, this, 0, FALSE );

  QDir docDir(QApplication::applicationDirPath());
  docDir.cd("../doc"); // Where Linux expects gpl3.txt
  QString path = docDir.filePath("gpl3.txt");
  if (!docDir.exists("gpl3.txt")){
    docDir = QApplication::applicationDirPath();
    docDir.cd("doc"); // Where Windows expects gpl3.txt
    path = docDir.filePath("gpl3.txt");
  }

  // At this point path points either to the linux variant, which
  // exists, or the windows variant, which may exist. Testing the
  // ifstream object will determine whether we've found gpl3.txt.

  std::ifstream file(path.toStdString().c_str());
  std::string str;

  if (file) {
    file.seekg(0, std::ios::end);   
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(file)),
	       std::istreambuf_iterator<char>());

    gpl->setDetailedText(QString(str.c_str()));
  }

  gpl->setText(QString( "<h3>GNU GENERAL PUBLIC LICENSE</h3>"
			"<p>Version 3, 29 June 2007</p>"
			"<p>Copyright &copy; 2007 Free Software Foundation, Inc. "
			"&lt;<a href=\"http://fsf.org/\">http://fsf.org/</a>&gt;</p><p>"
			"Everyone is permitted to copy and distribute verbatim copies "
			"of this license document, but changing it is not allowed.</p>"
			"<h2>GNU GENERAL PUBLIC LICENSE</h2>"));

  gpl->setButtonText( 1, "Dismiss" );
  gpl->show();
}

void Main::aboutQt(){
  QMessageBox::aboutQt( this, "Virtual Leaf" );
}

void Main::toggleShowCellCenters()
{
  Plot();
}

void Main::toggleShowWalls()
{
  Plot();
}

void Main::toggleShowApoplasts()
{
  Plot();
}

void Main::toggleShowNodes()
{
  Plot();
}

void Main::toggleNodeNumbers(void)
{
  Plot();
}

void Main::toggleCellNumbers(void)
{
  Plot();
}

void Main::toggleCellAxes(void)
{
  Plot();
}

void Main::toggleCellStrain(void)
{
  Plot();
}

void Main::toggleShowFluxes(void)
{
  Plot();
}

void Main::toggleShowBorderCells()
{
  Plot();
}

void Main::toggleHideCells(void)
{
  Plot();
  editor->FullRedraw();
}

void Main::toggleMovieFrames(){}

void Main::toggleLeafBoundary(){}

void Main::toggleDynCells() {}

void Main::startSimulation(void)
{
  timer->start( 0 );
  statusBar()->message("Simulation started");
  running = true;
}

void Main::stopSimulation(void)
{
  timer->stop();
  cerr << "Stopping simulation" << endl;
  statusBar()->message("Simulation paused");
  running = false;
}

void Main::togglePaused()
{
  bool s = run->isItemChecked(paused_id);
  if (s) {
    cerr << "Calling start simulation" << endl;
    startSimulation();
  } else {
    cerr << "Calling stop simulation" << endl;
    stopSimulation();
  }
}

void Main::setFluxArrowSize(int size)
{
  flux_arrow_size = size/100.;
}


void Main::enlarge()
{
  canvas.setSceneRect( QRectF( 0,0, canvas.width()*4./3., canvas.height()*4./3.) );
}

void Main::shrink()
{
  canvas.setSceneRect( QRectF( 0,0, canvas.width()*3/4, canvas.height()*3/4) );
}


void Main::scale(double factor)
{
  QMatrix m = editor->matrix();
  m.scale(factor, factor);
  editor->setMatrix( m );
}

void Main::zoomIn()
{
  QMatrix m = editor->matrix();
  m.scale( 1.1, 1.1 );
  editor->setMatrix( m );
}

void Main::zoomOut()
{
  QMatrix m = editor->matrix();
  m.scale( 0.9, 0.9 );
  editor->setMatrix( m );
}


void Main::print()
{
  if ( !printer ) printer = new QPrinter;

  if ( printer->setup(this) ) {

    //    extern Mesh mesh;
    Vector bbll,bbur;
    mesh.BoundingBox(bbll,bbur);

#ifdef QDEBUG
    qDebug() << "bbll = " << bbll << endl;
    qDebug() << "bbur = " << bbur << endl;
#endif
    double cw = (bbur.x - bbll.x);
    double ch = (bbur.y - bbll.y);
    QPainter pp(printer);
    QRect vp=pp.viewport();
#ifdef QDEBUG
    qDebug() << "Paper width = " << vp.width() << " x " << vp.height() << endl;
#endif

    // Note that Cell is also translated...
    /* pp.translate(-bbur.x,-bbur.y);
    if (cw>ch) {
      pp.scale(vp.width()/(2*cw*Cell::Magnification()), vp.width()/(2*cw*Cell::Magnification()));
    } else {
      pp.scale(vp.height()/(2*ch*Cell::Magnification()), vp.height()/(2*ch*Cell::Magnification()));
      }*/
    canvas.render(&pp);//, QRectF(), QRectF(0.,0.,canvas.width(),canvas.height()));
  }
}


void Main::TimeStepWrap(void)
{
  static int t;
  stringstream fname;

  TimeStep();

  t = (int)mesh.getTime();

  if ((par.export_interval > 0) && !(t%par.export_interval)){
    fname << par.datadir << "/" << par.export_fn_prefix;
    fname.fill('0');
    fname.width(6);
    fname << t << ".csv";
    this->exportCellData(QString(fname.str().c_str()));
  }

  // check number of timesteps
  if (t == par.nit) {
    emit SimulationDone();
  }
}


void Main::RestartSim(void)
{

  stopSimulation();
  if ( QMessageBox::question(
			     this,
			     tr("Restart simulation?"),
			     tr("Restart simulation.\n"
				"Are you sure?"),
			     QMessageBox::Yes | QMessageBox::No, QMessageBox::NoButton ) == QMessageBox::Yes ) {

    cerr << "Restarting simulation" << endl;
    //    extern Mesh mesh;
    mesh.Clear();
    Init();
    Plot();
    editor->FullRedraw();
  } 
  //startSimulation();
}


void Main::FitCanvasToWindow(void)
{

  double scale_factor_x = (double)editor->width()/(double)canvas.width();
  double scale_factor_y = (double)editor->height()/(double)canvas.height();
  double scale_factor = scale_factor_x > scale_factor_y ? scale_factor_x : scale_factor_y;
  QMatrix m = editor->matrix();

#ifdef QDEBUG  
  qDebug() << "editor->width() = " << editor->width() << endl;
  qDebug() << "editor->height() = " << editor->height() << endl;

  qDebug() << "scale_factor = " << scale_factor << endl;
  qDebug() << "scale_factor_x = " << scale_factor_x << endl;
  qDebug() << "scale_factor_y = " << scale_factor_y << endl;
#endif
  m.scale( scale_factor, scale_factor );
  editor->setMatrix( m );
  editor->show();
}

void Main::PauseIfRunning(void)
{
  if (running) {
    timer->stop();
  }
}

void Main::ContIfRunning(void)
{
  if (running) {
    timer->start( 0 );
  }
}

void Main::FitLeafToCanvas(void) 
{

  Vector ll,ur;
  mesh.BoundingBox(ll, ur);

  ll*=Cell::Magnification(); ur*=Cell::Magnification();

  // give the leaf some space
  Vector border = ((ur-ll)/5.);
  
  QRectF bb( ll.x - border.x, ll.y - border.y, ur.x-ll.x + 2*border.x, ur.y-ll.y + 2*border.y );


  // cerr << ur << ", " << ll << endl;
  // editor->fitInView(bb, Qt::KeepAspectRatio);
  editor->ensureVisible(bb);
  canvas.setSceneRect(bb);
  //editor->setTransform(viewport);
}

void Main::CleanMesh(void) 
{
  vector<double> clean_chem(Cell::NChem());
  vector<double> clean_transporters(Cell::NChem());

  for (int i=0;i<Cell::NChem();i++) {
    clean_chem[i]=par.initval[i];
    clean_transporters[i]=0.;		
  }

  mesh.CleanChemicals(clean_chem);
  mesh.CleanTransporters(clean_transporters);

  mesh.setTime(0);
  Plot();

  editor->FullRedraw();
}

void Main::CleanMeshChemicals(void) 
{

  vector<double> clean_chem(Cell::NChem());

  for (int i=0;i<Cell::NChem();i++) {
    clean_chem[i]=par.initval[i];
  }

  mesh.CleanChemicals(clean_chem);
  mesh.setTime(0);
  Plot();

  editor->FullRedraw();
}

void Main::CleanMeshTransporters(void) 
{
  vector<double> clean_transporters(Cell::NChem());
  for (int i=0;i<Cell::NChem();i++) {
    clean_transporters[i]=0.;
  }

  mesh.CleanTransporters(clean_transporters);

  mesh.setTime(0);
  Plot();

  editor->FullRedraw();
}

void Main::RandomizeMesh(void) 
{

  vector<double> max_chem(Cell::NChem());
  vector<double> max_transporters(Cell::NChem());

  for (int i=0;i<Cell::NChem();i++) {
    max_transporters[i]=0.;
    max_chem[i]=par.initval[i];
  }

  // Amount of PIN1 at walls
  max_transporters[1] = 0.01;

  mesh.RandomizeChemicals(max_chem, max_transporters);

  Plot();
}

void Main::XMLReadSettings(xmlNode *settings) 
{

  MainBase::XMLReadSettings(settings);
  
  view->setItemChecked(com_id, showcentersp);
  view->setItemChecked(mesh_id, showmeshp);
  view->setItemChecked(border_id, showbordercellp);
  view->setItemChecked(node_number_id, shownodenumbersp);
  view->setItemChecked(cell_number_id, showcellnumbersp);
  view->setItemChecked(cell_axes_id, showcellsaxesp);
  view->setItemChecked(cell_strain_id, showcellstrainp);
  view->setItemChecked(movie_frames_id, movieframesp);
  view->setItemChecked(only_boundary_id, showboundaryonlyp);
  view->setItemChecked(fluxes_id, showfluxesp);
  view->setItemChecked(hide_cells_id, hidecellsp);
  options->setItemChecked(dyn_cells_id, dynamicscellsp);
  view->setItemChecked( cell_walls_id, showwallsp);
 // view->setItemChecked( apoplasts_id, showapoplastsp);
  
  editor->setTransform(viewport);
}

xmlNode *Main::XMLSettingsTree(void) 
{

  showcentersp = view->isItemChecked(com_id);
  showmeshp = view->isItemChecked(mesh_id);
  showbordercellp =  view->isItemChecked(border_id);
  shownodenumbersp =  view->isItemChecked(node_number_id);
  showcellnumbersp =  view->isItemChecked(cell_number_id);
  showcellsaxesp = view->isItemChecked( cell_axes_id );
  showcellstrainp = view->isItemChecked( cell_strain_id );
  movieframesp = view->isItemChecked(movie_frames_id);;
  showboundaryonlyp =  view->isItemChecked(only_boundary_id);
  showfluxesp = view->isItemChecked(fluxes_id);
  dynamicscellsp = options->isItemChecked(dyn_cells_id);
  showwallsp = view->isItemChecked( cell_walls_id);
  //showapoplastsp = view->isItemChecked( apoplasts_id);
  hidecellsp = view->isItemChecked( hide_cells_id);

  xmlNode *settings = MainBase::XMLSettingsTree();
  QTransform viewport(editor->transform());
  xmlAddChild(settings, XMLViewportTree(viewport));
  return settings;
}


void Main::exportCellData(QString fileName) {
  
#ifdef QDEBUG  
  qDebug() << "exportCellData fileName: " << fileName << endl;
#endif

  QFile file(fileName);
  if ( file.open( IO_WriteOnly ) ) {
    QTextStream stream( &file );
    mesh.CSVExportCellData(stream);
    mesh.CSVExportWallData(stream);
    mesh.CSVExportMeshData(stream);
    file.close();
  }
}


void Main::exportCellData() {
  QString fileName;
  Q3FileDialog *fd = new Q3FileDialog( this, "file dialog", TRUE );
  fd->setDir(par.datadir); 
  stopSimulation();
  fd->setMode( Q3FileDialog::AnyFile );
  if ( fd->exec() == QDialog::Accepted ) {
    fileName = fd->selectedFile();

    // extract extension from filename
    QFileInfo fi(fileName);
    QString extension = fi.suffix();
    
    if (extension.isEmpty()) {
      extension = "csv";
      fileName += ".";
      fileName += extension;
    }

    if (extension!="csv" && extension!="CSV") {
      
      if (
	  QMessageBox::question(
				 this,
				 tr("Change extension? -- Cell data export"),
				 tr("VirtualLeaf can only export data to CSV format (Excel-compatible)."
				    "Do you want to change the file extension to \".csv\"?"),
				 QMessageBox::Yes, QMessageBox::No ) == QMessageBox::No 
	  ) {

	return exportCellData();
      } else {
	fileName.replace(extension,"csv");
	extension="csv";
      }
    }
    if ( QFile::exists( fileName ) &&
	 QMessageBox::question(
			       this,
			       tr("Overwrite file? -- Cell data export"),
			       tr("A file called %1 already exists."
				  " Do you want to overwrite it?").arg( fileName ),
			       QMessageBox::Yes, QMessageBox::No 
			       ) == QMessageBox::No
	 ) {
      return exportCellData();
    } else {
      exportCellData(fileName);
      QString status_message = QString("Wrote data file to %1").arg(fileName);
      statusBar()->showMessage(status_message);
    }
  }
}


  /* finis */
