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

#include <ctime>
#include <string>
#include <fstream>
#include <streambuf>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <qdatetime.h>
#include <qmainwindow.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <QPainter>
#include <QPrinter>
#include <qlabel.h>
#include <qimage.h>

#include <qtimer.h>
#include <qslider.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qdir.h>
#include <QGraphicsItem>
#include <QList>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QImageWriter>
#include <QFileDialog>
#include <QPrintDialog>
#include <QGraphicsLineItem>
#include <QSettings>

#include <set>

#include <QDialogButtonBox>
#include <QMouseEvent>
#include <typeinfo>
#include <cstring>
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
#include "tissuegenerator.h"
#include <cstdlib>
#include <cstdio>
#include "modelcatalogue.h"

#include <algorithm>
#include <iostream>

#include "leaficon_small.xpm"

static const std::string _module_id("$Id$");

using namespace std;

// We use a global variable to save memory - all the brushes and pens in
// the mesh are shared.

//#define CUTTINGTOOL

#define FNAMESIZE 100
#define QUOTE_ME(s) QUOTE_ME_2NDLEV(s)
#define QUOTE_ME_2NDLEV(s) #s

static QColor dark_red("darkRed");


//static const int imageRTTI = 984376;
extern Parameter par;
const QString Main::caption("Virtual leaf");
const QString Main::caption_with_file("Virtual leaf: %1");

FigureEditor::FigureEditor(
        QGraphicsScene& c, Mesh &m, QWidget* parent,
        const char* name, Qt::WindowFlags f) :
    QGraphicsView(&c,parent), mesh(m)
{
    #ifdef CUTTINGTOOL
    intersection_line = 0;
    #endif
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
    QPoint rotate = event->angleDelta();
   // scaleView(pow((double)2, -event->delta() / 240.0));
     scaleView(pow((double)2, -rotate.y() / 240.0));
}


void FigureEditor::scaleView (qreal scaleFactor)
{
    //qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)). width();
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)). width();

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
#ifdef CUTTINGTOOL
#ifdef QDEBUG  
    qDebug() << "MousePressEvents, items: " << l.size() << endl;
    qDebug() << "Mouse button modifier: " << e->modifiers() << endl;
#endif

    if (e->button()==Qt::RightButton || l.size()==0) {

        //cerr << "Drawing an intersection line from " << p.x() << ", " << p.y() << endl;
        intersection_line = new QGraphicsLineItem( QLineF(p,p), 0 );
        intersection_line->setPen( QPen( QColor("red"), 3, Qt::DashLine ) );
        //intersection_line->setLine( QLineF(p,p) );

        intersection_line->setZValue( 100 );
        scene()->addItem(intersection_line);
        intersection_line->show();
    }
#endif
    for (QList<QGraphicsItem *>::Iterator it=l.begin(); it!=l.end(); ++it) {
#ifdef QDEBUG
        qDebug() << typeid(**it).name() << endl;
#endif
        QGraphicsItem *gi=*it;
        if ( !strcmp(typeid(*gi).name(),"8NodeItem")) {
            stringstream data_strstream;
            data_strstream << (dynamic_cast<NodeItem*>(*it))->getNode();
            dynamic_cast<Main *>(parent())->UserMessage(QString(data_strstream.str().c_str()));

            (dynamic_cast<NodeItem*>(*it))->OnClick(e->button());
        	break;
        }
        else
            if ( !strcmp(typeid(*gi).name(),"8CellItem") ) {

                Cell &c=((dynamic_cast<CellItem *>(*it))->getCell());
                // OnClick to be defined in end-user code
                c.OnClick(e);
            	break;
            } else {
                if ( !strcmp(typeid(*gi).name(),"8WallItem") ) {
                	WallItem *w=((dynamic_cast<WallItem *>(*it)));
                	w->OnClick(e);

                	break;
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
    #ifdef CUTTINGTOOL
    if ( intersection_line ) {

        QPointF sp = intersection_line -> line().p1(); // startpoint
        QPointF ep = mapToScene(e->pos()); // endpoint
        intersection_line -> setLine( QLineF(sp, ep) );
        scene()->update();
        // Need this for Mac
        FullRedraw();
    }
    #endif
}

//void FigureEditor::contentsMouseReleaseEvent(QMouseEvent* e)
void FigureEditor::mouseReleaseEvent(QMouseEvent* e)
{

    emit MouseReleased();
    // intersection line for leaf was finished now.
#ifdef CUTTINGTOOL
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
  } 
  #endif
   /* else
    if (e->button()==Qt::RightButton) {

      if (intersection_line) { // && !angle_line

    QPointF p = mapToScene(e->pos());
    QPointF sp = intersection_line->line().p1();

    viewport()->setMouseTracking( true );
      }
    }*/
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
        QGraphicsItem *gi=*it;
#ifdef QDEBUG
        qDebug() << typeid(*gi).name() << endl;
#endif

        if ( !strcmp(typeid(*gi).name(),"8CellItem") ) {
            colliding_cells.push_back(dynamic_cast<CellItem *>(gi));
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
        QGraphicsItem *gi=*it;
        if ( !strcmp(typeid(*gi).name(),"8NodeItem")) {
            // the object under the mouse click is a Nodeitem
            nodeItem = dynamic_cast<NodeItem *>(gi);
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
}

static uint mainCount = 0;

Main::Main(QGraphicsScene& c, Mesh &m, QWidget* parent, const char* name, Qt::WindowFlags f) :
    QMainWindow(parent,f),
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

    file = menu->addMenu("&File");

    file->addAction("&Read leaf", this, SLOT(readStateXML()));
    file->addAction("&Save leaf", this, SLOT(saveStateXML()));
    file->addAction("Generate new tissue", this, SLOT(GenerateNewTissue()));
    file->addAction("Snapshot", this, SLOT(snapshot()), Qt::CTRL|Qt::SHIFT|Qt::Key_S);

    file->addSeparator();
    file->addAction("Read next leaf", this, SLOT(readNextStateXML()), Qt::Key_PageDown);
    file->addAction("Read previous leaf", this, SLOT(readPrevStateXML()), Qt::Key_PageUp);
    file->addAction("Read last leaf", this, SLOT(readLastStateXML()), Qt::Key_End);
    file->addAction("Read first leaf", this, SLOT(readFirstStateXML()), Qt::Key_Home);
    file->addAction("Export cell data", this, SLOT(exportCellData()));

    file->addSeparator();
    file->addAction("&Print...", this, SLOT(print()), Qt::CTRL|Qt::Key_P);
    file->addSeparator();
    file->addAction("E&xit", qApp, SLOT(quit()), Qt::CTRL|Qt::Key_Q);
    //menu->addAction("&File", file);

    edit = menu->addMenu("&Edit");
    
    edit->addAction("Reset Chemicals and Transporters", this, SLOT( CleanMesh()), Qt::CTRL|Qt::Key_R );
    edit->addAction("Reset Chemicals", this, SLOT( CleanMeshChemicals()) );
    edit->addAction("Reset Transporters", this, SLOT( CleanMeshTransporters()) );
    edit->addAction("Randomize Chemicals and Transporters", this, SLOT( RandomizeMesh()) );
    edit->addAction("Cut away SAM", this, SLOT( CutSAM() ));

    run = menu->addMenu("&Run");
    running = false;
    paused_act = run->addAction("&Start Simulation", this, SLOT(togglePaused()), Qt::Key_Space);
    paused_act->setCheckable(true);
    paused_act->setChecked(true);

    view = menu->addMenu("&View");
    view->addAction("&Zoom in", this, SLOT(zoomIn()), Qt::CTRL|Qt::Key_Equal);
    view->addAction("Zoom &out", this, SLOT(zoomOut()), Qt::CTRL|Qt::Key_Minus);
    view->addSeparator();
    com_act = view->addAction("Show cell &centers", this, SLOT(toggleShowCellCenters()));
    com_act->setCheckable(true);
    
    mesh_act = view->addAction("Show &nodes", this, SLOT(toggleShowNodes()), Qt::CTRL|Qt::SHIFT|Qt::Key_N);
    mesh_act->setCheckable(true);
    
    node_number_act = view->addAction("Show node numbers", this, SLOT(toggleNodeNumbers()), Qt::CTRL|Qt::SHIFT|Qt::Key_M);
    node_number_act->setCheckable(true);
    
    cell_number_act = view->addAction("Show cell numbers", this, SLOT(toggleCellNumbers()));
    cell_number_act->setCheckable(true);
    cell_number_act->setChecked(false);
    
    hide_cells_act = view->addAction("Hide cells", this, SLOT(toggleHideCells()));
    hide_cells_act->setCheckable(false);
    hide_cells_act->setChecked(false);
    border_act = view->addAction("Show &border cells", this, SLOT(toggleShowBorderCells()));
    border_act->setCheckable(true);
    border_act->setChecked(false);
    
    cell_axes_act = view->addAction("Show cell &axes", this, SLOT(toggleCellAxes()));
    cell_axes_act->setCheckable(true);
    cell_axes_act->setChecked(false);
    
    cell_strain_act = view->addAction("Show cell &strain", this, SLOT(toggleCellStrain()));
    cell_strain_act->setCheckable(true);
    cell_strain_act->setChecked(false);
    
    fluxes_act = view->addAction("Show &fluxes", this, SLOT(toggleShowFluxes()));
    fluxes_act->setCheckable(true);
    fluxes_act->setChecked(false);
    
    cell_walls_act = view->addAction("Show transporters", this, SLOT(toggleShowWalls()));
    cell_walls_act->setCheckable(true);
    cell_walls_act->setChecked(false);
    
    walls_middle_act = view->addAction("Show middle lamella", this, SLOT(toggleShowWalls()));
    walls_middle_act->setCheckable(true);
    walls_middle_act->setChecked(true);

    walls_stiff_act = view->addAction("Show stiffness", this, SLOT(toggleShowWalls()));
    walls_stiff_act->setCheckable(true);
    walls_stiff_act->setChecked(true);
    
    // apoplasts_act = view->addAction("Show apoplasts", this, SLOT(toggleShowApoplasts()));
    // view->setItemChecked(apoplasts_act, false);
    view->addSeparator();
    
    only_boundary_act = view->addAction("Show only leaf &boundary", this, SLOT(toggleLeafBoundary()));
    only_boundary_act->setCheckable(true);
    only_boundary_act->setChecked(false);
    
    view->addSeparator();

    output = menu->addMenu("&Output");
    
    output->addAction("Snapshot", this, SLOT(snapshot()), Qt::CTRL|Qt::SHIFT|Qt::Key_S);
    output->addAction("Export cell data", this, SLOT(exportCellData()));
    //output->addAction("Export Cell in DXF format", this, SLOT(exportDXF()));
    
    output->addSeparator();
    
    movie_frames_act = output->addAction("Start saving movie &frames", this, SLOT(toggleMovieFrames()));
    movie_frames_act->setCheckable(true);
    movie_frames_act->setChecked(par.movie);
    
    /*export_data_act = output->addAction("Export cell data over time", this, SLOT(exportOverTime()));
    export_data_act->setCheckable(true);
    export_data_act->setChecked(false);*/
    
    /* ignore_boundary_act = output->addAction("Ignore cells at boundary", this, SLOT(toggleIgnore()));
    ignore_boundary_act->setCheckable(true);
    ignore_boundary_act->setChecked(false);*/
    
    options = menu->addMenu("&Options");
    
    dyn_cells_act = options->addAction("Cell growth", this, SLOT(toggleDynCells()));
    dyn_cells_act->setCheckable(true);
    dyn_cells_act->setChecked(true);

    options->addSeparator();
    options->addAction("Edit &parameters", this, SLOT(EditParameters()), Qt::CTRL|Qt::Key_E);
    
    rotation_mode_act = options->addAction("Rotate leaf", this, SLOT(EnterRotationMode()), Qt::CTRL | Qt::SHIFT | Qt::Key_R);
    rotation_mode_act->setCheckable(true);
    rotation_mode_act->setChecked(false);

    // Menu of models
    modelmenu = menu->addMenu("&Models");


    menu->addSeparator();

    helpmenu =  menu->addMenu("&Help");
    tooltips_act = helpmenu->addAction("Show Cell&Info", this, SLOT(Refresh()));
    tooltips_act->setCheckable(true);
    tooltips_act->setChecked(true);
    
    helpmenu->addSeparator();
    //helpmenu->addSeparator();
    helpmenu->addAction("&LICENSE", this, SLOT(gpl()) );
    helpmenu->addAction("About", this, SLOT(about()) ); //, Key_F1);

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
    setWindowTitle(caption);
    gifanim = 0;

    infobar = new InfoBar();
    addDockWidget(Qt::BottomDockWidgetArea, infobar);


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

/*void Main::newView()
{
  // Open a new view... have it delete when closed.
  Main *m = new Main(canvas, mesh, 0, 0, Qt::WA_DeleteOnClose);
  //qApp->setMainWidget(m); // not needed anymore in Qt4?
  m->show();
  //qApp->setMainWidget(0);
}
*/

void Main::EditParameters()
{

    ParameterDialog *pardial = new ParameterDialog(this);

    // Make sure the values in the parameter dialog are updated after a file is read
    // each method changing the parameters (reading XML or PAR files) should
    // emit this signal
    QObject::connect(   this, SIGNAL( ParsChanged() ), pardial, SLOT( Reset() ) );
}

void Main::savePars()
{

    stopSimulation();

    QFileDialog *fd = new QFileDialog( this, "Save Parameters", QString(par.datadir), QString("Parameter files (*.par)"));
    fd->setFileMode( QFileDialog::AnyFile );
    fd->setAcceptMode(QFileDialog::AcceptSave);


    QString fileName;
    if ( fd->exec() == QDialog::Accepted ) {
        //fileName = fd->selectedFile();
        QStringList files = fd->selectedFiles();
        if (!files.isEmpty())
            fileName = files[0];

        ofstream parfile(fileName.toLatin1());
        par.Write(parfile);
    }

    startSimulation();
}

void Main::readPars()
{

    stopSimulation();

    QFileDialog *fd = new QFileDialog( this, "Read Parameters", QString(par.datadir), QString("Parameter files (*.par)"));
    fd->setFileMode( QFileDialog::ExistingFile );
    //fd->setFilter( "Parameter files (*.par)");
    //fd->setDir(par.datadir);

    QString fileName;
    if ( fd->exec() == QDialog::Accepted ) {
        //fileName = fd->selectedFile();
        QStringList files = fd->selectedFiles();
        if (!files.isEmpty())
            fileName = files[0];

        par.Read(fileName.toLatin1());
    }

    emit ParsChanged();
}


void Main::saveStateXML(void)
{

    stopSimulation();
    QFileDialog *fd = new QFileDialog( this, "Save State as LeafML", QString(par.datadir), QString("LeafML files (*.xml)"));
    fd->setFileMode( QFileDialog::AnyFile );
    fd->setAcceptMode(QFileDialog::AcceptSave);

    QString fileName;

    if ( fd->exec() == QDialog::Accepted ) {
        //fileName = fd->selectedFile();
        QStringList files = fd->selectedFiles();
        if (!files.isEmpty())
            fileName = files[0];

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
                 QString(), 1, 1 ) ) {
            return saveStateXML();

        } else {
        QDomElement settings=XMLSettingsTree();
            mesh.XMLSave(fileName.toLatin1(),settings);
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
    QFileDialog *fd = new QFileDialog( this, "Save snapshot", QString(par.datadir), QString("*") );

    fd->setFileMode( QFileDialog::AnyFile );
    fd->setAcceptMode(QFileDialog::AcceptSave);

    QString supported_file_formats = " *.pdf";
    foreach (QString format, QImageWriter::supportedImageFormats()){
        supported_file_formats += (" *." + format);
    }

    fd->setNameFilter("Image files (" + supported_file_formats + " )");

    QString fileName;
    if ( fd->exec() == QDialog::Accepted ) {
        //fileName = fd->selectedFile();
        QStringList files = fd->selectedFiles();

        if (!files.isEmpty())
            fileName = files[0];


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
            if (Save(fileName.toLatin1(), extension.toLatin1(), 1024, 768)==0) {
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

        QStringList xml_files = working_dir->entryList(QStringList("*.xml"));
        QString currentFile_nopath = currentFile.section( '/', -1 );
        QString currentFile_path = currentFile.section( '/', 0, -2 );

        //QList<QString>::iterator f = xml_files.indexOf( currentFile_nopath );
        int f = xml_files.indexOf( currentFile_nopath );

        //  if (f == xml_files.end()) {
        if (f == -1) {
            return;
        }

        // if (f==xml_files.begin()) {
        if (f==0) {
            QMessageBox mb( "Read previous leaf",
                            "No more files",
                            QMessageBox::Information,
                            QMessageBox::Ok | QMessageBox::Default,
                            QMessageBox::NoButton,
                            QMessageBox::NoButton);
            mb.exec();
            return;
        }

        //next_file = *(--f);
        QString nextfile = xml_files[--f];
        next_file = currentFile_path+"/"+next_file;

        readStateXML(next_file.toLatin1());

    }
}

int Main::readStateXML(const char *filename, bool geometry, bool pars, bool simtime)
{

    try {
        QDomElement settings;
        mesh.XMLRead((const char *)filename, settings, geometry, pars, simtime);
#ifdef QDEBUG
        qDebug() << "Reading done."<< endl;
#endif
        XMLReadSettings(settings);
        //xmlFree(settings);
        Cell::SetMagnification(1);
        Cell::setOffset(0,0);

        FitLeafToCanvas();

        currentFile =  QString(filename);

        Plot();
        QString status_message = QString("Successfully read leaf from file %1. Time is %2 h.").arg(currentFile).arg(mesh.getTimeHours().c_str());
        cerr << status_message.toStdString().c_str() << endl;
        setWindowTitle(caption_with_file.arg(filename));
        statusBar()->showMessage(status_message);
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

        QStringList xml_files = working_dir->entryList(QStringList("*.xml"));
        QString currentFile_nopath = currentFile.section( '/', -1 );
        QString currentFile_path = currentFile.section( '/', 0, -2 );


        //   QList<QString>::iterator f = xml_files.find( currentFile_nopath );
        int f = xml_files.indexOf( currentFile_nopath );

        if (f == -1) {
            return;
        }

        ++f;
        if (f==-1) {
            QMessageBox mb( "Read next leaf",
                            "No more files",
                            QMessageBox::Information,
                            QMessageBox::Ok | QMessageBox::Default,
                            QMessageBox::NoButton,
                            QMessageBox::NoButton);
            mb.exec();
            return;
        }
        next_file = xml_files[f];
        next_file = currentFile_path+"/"+next_file;

        readStateXML(next_file.toLatin1());
    }
}

void Main::readLastStateXML()
{

    // if we have already read a file, read the next file
    if (!currentFile.isEmpty() && working_dir) {
        QString next_file;

        QStringList xml_files = working_dir->entryList(QStringList("*.xml"));
        QString currentFile_nopath = currentFile.section( '/', -1 );
        QString currentFile_path = currentFile.section( '/', 0, -2 );


        next_file = xml_files.back();

        next_file = currentFile_path+"/"+next_file;

        readStateXML(next_file.toLatin1());
    }
}


void Main::readFirstStateXML()
{

    // if we have already read a file, read the next file
    if (!currentFile.isEmpty() && working_dir) {
        QString next_file;

        QStringList xml_files = working_dir->entryList(QStringList("*.xml"));
        QString currentFile_nopath = currentFile.section( '/', -1 );
        QString currentFile_path = currentFile.section( '/', 0, -2 );


        next_file = xml_files.front();

        next_file = currentFile_path+"/"+next_file;

        readStateXML(next_file.toLatin1());
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
    OptionFileDialog *fd = new OptionFileDialog( this, "Read LeafML file", true );
    fd->setFileMode( QFileDialog::ExistingFile );
    fd->setNameFilter( "LeafML files (*.xml)");
    if (working_dir) {
        fd->setDirectory(*working_dir);
    } else {
        fd->setDirectory(par.datadir);
    }
    QString fileName;
    if ( fd->exec() == QDialog::Accepted ) {

        // fileName = fd->selectedFile();
        QStringList files = fd->selectedFiles();

        if (!files.isEmpty())
            fileName = files[0];

        if (working_dir) {
            delete working_dir;
        }
        working_dir = new QDir(fd->directory());

        if (readStateXML(fileName.toLatin1(),fd->readGeometryP(), fd->readParametersP()) )
            return readStateXML(); // user can try again
    }
}


void Main::clear()
{
    editor->clear();
}

void Main::setAbout(int state) {
    QSettings settings;
    settings.setValue("main/showAbout",state);
}

void Main::about()
{


    QCheckBox *cb = new QCheckBox("Show at startup.");
    QSettings settings;
    cb->setChecked(settings.value("main/showAbout").toInt());

    QMessageBox msgbox;
    msgbox.setText(QString(
                       "<h3>VirtualLeaf V2.0.0</h3>\
                       <p>\
                       An Open Source framework for cell-based modeling of plant\
                       tissue growth and development.\
                       </p>\
                       <p>(c) 2022-2024,\
    				    Ruth Großeholz, Richard W. van Nieuwenhoven,\
                       Bruno Hay Mele, Roeland M.H. Merks\
                       Maastricht University, TU Wien,\
                       University of Naples and Leiden University \
                       <a href=\"https://doi.org/10.1101/2024.08.01.605200\">DOI:10.1101/2024.08.01.605200</a>\
                       </p>\
                       <p>(c) 2014-2019,\
                       Roeland Merks <i>et al.</i></a>\
                       Mathematical Institute (MI) and\
                       Institute of Biology Leiden (IBL),\
                       <a href=\"www.leidenuniv.nl\">Leiden University</a> \
            Leiden, The Netherlands.\
                       </p>\
                       <p>(c) 2008-2018,\
                       <a href=\"http://biomodel.project.cwi.nl\">Roeland Merks <i>et al.</i></a>\
            <a href=\"http://www.cwi.nl\">Centrum Wiskunde & Informatica (CWI)</a> and\
            Netherlands Consortium for Systems Biology (NCSB),\
                       Amsterdam, the Netherlands.\
                       </p>\
                       <p>(c) 2005-2008, Roeland Merks <i>et al.</i>\
                       <a href=\"http://www.psb.vib-ugent.be\">VIB Department Plant Systems Biology</a>,\
            Ghent, Belgium.\
                       </p>\
                       <p>\
                       VirtualLeaf is free software: you can redistribute it and/or\
                       modify it under the terms of the GNU General Public License as\
                       published by the Free Software Foundation, either version 3 of the\
                       License, or (at your option) any later version.\
                       </p>\
                       <p>\
                       If you use this code for your projects, please cite our paper in\
                       <a href=\"http://www.plantphysiol.org\">Plant Physiology</a>,\
            Roeland M. H. Merks, Michael Guravage, Dirk Inz&eacute;, and Gerrit T.S. Beemster,\
                   <a href=\"http://www.plantphysiol.org/cgi/reprint/155/2/656.pdf\">\
            VirtualLeaf: an Open Source framework for cell-based modeling of plant tissue growth and development</a>,<br>\
                   Plant Physiology 2011: 155:656-666.</p> \
                   <p>Enhanced Cell Wall Mechanics in VirtualLeaf Enable Realistic Simulations of Plant Tissue Dynamics \
                       In Xxxxxxxxxxxxxxxx.\
                       <a href=\"http://doi.org/10.1101/2024.08.01.605200\">doi:10.1101/2024.08.01.605200</a> \
                   </p>\
                   <p>To learn how to use VirtualLeaf for your projects, see the tutorial at\
                   Merks, R. M. H., & Guravage, M. A. (2012). Building Simulation Models of Developing Plant Organs Using VirtualLeaf. \
                   In I. De Smet (Ed.), Methods in Molecular Biology (Vol. 959, pp. 333–352). Totowa, NJ: Humana Press.\
                   <a href=\"http://doi.org/10.1007/978-1-62703-221-6_23\">doi:10.1007/978-1-62703-221-6_23</a> \
            </p>\
            <p>\
            Please share your model plugins and extensions at\
            <a href=\"https://github.com/rmerks/VirtualLeaf.git\">https://github.com/rmerks/VirtualLeaf.git</a>.\
            </p>"));
            msgbox.setIcon(QMessageBox::Icon::Question);
            msgbox.addButton(QMessageBox::Ok);
    msgbox.setDefaultButton(QMessageBox::Ok);
    msgbox.setCheckBox(cb);
    msgbox.setIconPixmap(QPixmap(leaficon_small));
    QObject::connect(cb, &QCheckBox::stateChanged, [this](int state){
        this->setAbout(state);
    });

    msgbox.exec();


}


void Main::gpl()
{
    static QMessageBox* gpl = new QMessageBox ( QMessageBox::Information,
                                                "GPL License", "",
                                                QMessageBox::Ok);

    QDir docDir(QApplication::applicationDirPath());
    docDir.cd("../doc"); // Where Linux expects gpl3.txt
    QString path = docDir.filePath("gpl3.txt");
    if (!docDir.exists("gpl3.txt")){
      //docDir = QApplication::applicationDirPath();
        docDir.setPath(QApplication::applicationDirPath());
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
    statusBar()->showMessage("Simulation started");
    running = true;
}

void Main::stopSimulation(void)
{
    timer->stop();
    cerr << "Stopping simulation" << endl;
    statusBar()->showMessage("Simulation paused");
    running = false;
}

void Main::togglePaused()
{
    if (!paused_act->isChecked()) {
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
    QTransform m = editor->transform();
    //   QMatrix m = editor->matrix();
    m.scale(factor, factor);
    editor->setTransform(m );
}

void Main::zoomIn()
{
    QTransform m = editor->transform();
    m.scale( 1.1, 1.1 );
    editor->setTransform( m );
}

void Main::zoomOut()
{
    QTransform m = editor->transform();
    m.scale( 0.9, 0.9 );
    editor->setTransform( m );
}


void Main::print()
{
    if ( !printer ) printer = new QPrinter;

    //if ( printer->setup(this) ) {
    QPrintDialog dialog(printer, this);
    if (dialog.exec()) {
        
        //    extern Mesh mesh;
        Vector bbll,bbur;
        mesh.BoundingBox(bbll,bbur);

#ifdef QDEBUG
        qDebug() << "bbll = " << bbll << endl;
        qDebug() << "bbur = " << bbur << endl;
#endif
       // double cw = (bbur.x - bbll.x);
        //double ch = (bbur.y - bbll.y);
        QPainter pp(printer);
   //     QRect vp=pp.viewport();
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
    QTransform m = editor->transform();

#ifdef QDEBUG  
    qDebug() << "editor->width() = " << editor->width() << endl;
    qDebug() << "editor->height() = " << editor->height() << endl;

    qDebug() << "scale_factor = " << scale_factor << endl;
    qDebug() << "scale_factor_x = " << scale_factor_x << endl;
    qDebug() << "scale_factor_y = " << scale_factor_y << endl;
#endif
    m.scale( scale_factor, scale_factor );
    editor->setTransform( m );
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

void Main::XMLReadSettings(QDomElement &settings)
{

    MainBase::XMLReadSettings(settings);

    com_act->setChecked(showcentersp);
    mesh_act->setChecked(showmeshp);
    border_act->setChecked(showbordercellp);
    node_number_act->setChecked(shownodenumbersp);
    cell_number_act->setChecked(showcellnumbersp);
    cell_axes_act->setChecked(showcellsaxesp);
    cell_strain_act->setChecked(showcellstrainp);
    movie_frames_act->setChecked(movieframesp);
    only_boundary_act->setChecked(showboundaryonlyp);
    fluxes_act->setChecked(showfluxesp);
    hide_cells_act->setChecked(hidecellsp);
    dyn_cells_act->setChecked(dynamicscellsp);
    cell_walls_act->setChecked(showwallsp);
    walls_middle_act->setChecked(showmiddlelamella);
    walls_stiff_act->setChecked(showstiffness);
    // apoplasts_act->setChecked(showapoplastsp);
    
    editor->setTransform(viewport);
}

QDomElement Main::XMLSettingsTree()
{

    showcentersp = com_act->isChecked();
    showmeshp = mesh_act->isChecked();
    showbordercellp =  border_act->isChecked();
    shownodenumbersp =  node_number_act->isChecked();
    showcellnumbersp =  cell_number_act->isChecked();
    showcellsaxesp = cell_axes_act->isChecked(  );
    showcellstrainp = cell_strain_act->isChecked(  );
    movieframesp = movie_frames_act->isChecked();
    showmiddlelamella = walls_middle_act->isChecked();
	showstiffness =  walls_stiff_act->isChecked();

    //exportdatap = export_data_act->isChecked();
    //ignorep = ignore_boundary_act->isChecked();
    showboundaryonlyp =  only_boundary_act->isChecked();
    showfluxesp = fluxes_act->isChecked();
    //MonteCp = MonteC_act->isChecked();
    // metrop = metro_act->isChecked();
    dynamicscellsp = dyn_cells_act->isChecked();
    //rigidp = rigid_act->isChecked();
    //remodp = remod_act->isChecked();
    //vertexp = vertex_act->isChecked();
    //duplip = dupli_act->isChecked();
    //removep = remove_act->isChecked();
    showwallsp = cell_walls_act->isChecked( );
    //showapoplastsp = apoplasts_act->isChecked( );
    hidecellsp = hide_cells_act->isChecked( );
    
    QDomElement settings = MainBase::XMLSettingsTree();
    QTransform viewport(editor->transform());

    //xmlAddChild(settings, XMLViewportTree(viewport));
    settings.appendChild(XMLViewportTree(viewport));
    return settings;
}


void Main::exportCellData(QString fileName) {

#ifdef QDEBUG  
    qDebug() << "exportCellData fileName: " << fileName << endl;
#endif

    QFile file(fileName);
    if ( file.open( QFile::WriteOnly ) ) {
        QTextStream stream( &file );
        mesh.CSVExportCellData(stream);
        mesh.CSVExportWallData(stream);
        mesh.CSVExportMeshData(stream);
        file.close();
    }
}


void Main::exportCellData() {
    QString fileName;
    QFileDialog *fd = new QFileDialog( this, "file dialog", QString(par.datadir), QString("*") );
    fd->setAcceptMode(QFileDialog::AcceptSave);
    fd->setDirectory(par.datadir);
    stopSimulation();
    fd->setFileMode( QFileDialog::AnyFile );
    if ( fd->exec() == QDialog::Accepted ) {
        //fileName = fd->selectedFile();
        QStringList files = fd->selectedFiles();
        if (!files.isEmpty())
            fileName = files[0];

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

void Main::GenerateNewTissue()
{

    stopSimulation();
    TissueGenerator tisgen;

    QString filename = tisgen.GenerateTissue(this);
    if (filename.isEmpty()) {
        QString status_message;
        status_message = QString("failed to generate tissue").arg(filename);
        cerr << status_message.toStdString().c_str() << endl;
        statusBar()->showMessage(status_message);
    } else {
        QString status_message;
        status_message = QString("Generated new tissue in %1").arg(filename);
        cerr << status_message.toStdString().c_str() << endl;
        statusBar()->showMessage(status_message);
    }
}
/* finis */
