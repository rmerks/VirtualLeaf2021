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


// Cell derives from Vector, where Vector is simply used as a Vertex

#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include <set>
#include <algorithm>
#include <queue>
#include <iterator>
#include <functional>
#ifdef QTGRAPHICS
#include <QGraphicsScene>
#endif
#include "cell.h"
#include "node.h"
#include "simplugin.h"
#include <QVector>
#include <QPair>
#include <QDebug>
#include <QTextStream>
#include <QtXml>

#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
namespace Qt
{
    static auto endl = ::endl;
    static auto SkipEmptyParts = QString::SkipEmptyParts;
}
#endif

using namespace std;
// new queue which rejects duplicate elements
template<class T, class C = deque<T> > class unique_queue : public queue<T,C> {

 public:
 typedef typename C::value_type value_type;
 // reimplements push: reject element if it exists already
 void push(const value_type &x) {
   if (find (queue<T,C>::c.begin(),queue<T,C>::c.end(),x)==queue<T,C>::c.end()) {
     queue<T,C>::c.push_back(x);
   }
 }
 void clear(void) {
   queue<T,C>::c.clear();
 }
};

template<class P> P& deref_ptr ( P *obj) { return *obj; }

class DeltaIntgrl;


#define WALL_STIFFNESS_HAMILTONIAN 1
#define WALL_SLIDING 3


class Mesh {

  friend class Cell;
  friend class Node;
  friend class FigureEditor;

 public: 
  Mesh(void) {
    // Make sure the reserved value is large enough if a cell is added
    // in "Divide" when the capacity is insufficient, "cells" might be
    // relocated including the current Cell (i.e. the value of *this)
    // calling "Mesh::IncreaseCapacityIfNecessary" (from another
    // object than Cell, e.g. Mesh) before entering Divide will solve
    // this issue (solved now).
    cells.reserve(2);
    nodes.reserve(500);

    time = 0.;
    plugin = 0;
    boundary_polygon=0;

  };
  ~Mesh(void) {
    if (boundary_polygon) {
      delete boundary_polygon;
      boundary_polygon=0;
    }
  };

  void Clean(void);
  Cell &EllipticCell(double xc, double yc, double ra, double rb, int nnodes=10, double rotation=0);
  Cell &CircularCell(double xc, double yc, double r, int nnodes=10) {
    return EllipticCell(xc, yc, r, r, nnodes, 0);
  }
  Cell &LeafPrimordium(int n, double pet_length);
  Cell &LeafPrimordium2(int n);
  Cell *RectangularCell(const Vector ll, const Vector ur, double rotation = 0);
  void CellFiles(const Vector ll, const Vector ur);

  inline Cell &getCell(int i) {
    if ((unsigned)i<cells.size())
      return *cells[i];
    else {
#ifdef QDEBUG
      qDebug() << i << endl;
      qDebug() << "size is " << cells.size() << endl;
#endif
      abort();
    }
  }

  inline Node &getNode(int i) {
    return *nodes[i];    
  }

  //double Diffusion(void);
  inline int size(void) {
    return cells.size();
  }
  inline int nnodes(void) {
    return nodes.size();
  }

  template<class Op> void LoopCells(Op f) {
    for (vector <Cell *>::iterator i=cells.begin();
	 i!=cells.end();
	 i++) {
      f(*i);
    }
  }

  template<class Op> void LoopWalls(Op f) {
    for (list <Wall *>::iterator i=walls.begin();
	 i!=walls.end();
	 i++) {
      f(*i);
    }
  }

  // if the amount of cells might increase, during looping, use this template
  template<class Op> void LoopCurrentCells(Op f) {
    vector<Cell *> current_cells = cells;
    for (vector <Cell *>::iterator i=current_cells.begin();
	 i!=current_cells.end();
	 i++) {
      f(*i);

    }
  }

  template<class Op> void LoopNodes(Op f) {
    for (vector<Node *>::iterator i=nodes.begin();
	 i!=nodes.end();
	 i++) {
      f(*i);
    }
  }

  template<class Op> void RandomlyLoopNodes(Op f) {

    MyUrand r(shuffled_nodes.size());
    vl_shuffle(shuffled_nodes.begin(),shuffled_nodes.end(),r);

    for (vector<Node *>::const_iterator i=shuffled_nodes.begin();
	 i!=shuffled_nodes.end();
	 i++) {
      f(*i);
    }
  }

  template<class Op> void RandomlyLoopCells(Op f) {

    MyUrand r(shuffled_cells.size());
    vl_shuffle(shuffled_cells.begin(),shuffled_cells.end(),r);

    for (vector<Cell *>::const_iterator i=shuffled_cells.begin();
	 i!=shuffled_cells.end();
	 i++) {
      f(*i);
    }
  }

  template<class Op1, class Op2> void LoopCells(Op1 f, Op2 &g) {
    for (vector<Cell *>::iterator i=cells.begin();
	 i!=cells.end();
	 i++) {
      f(*i,g);
    }
  }

  template<class Op1, class Op2, class Op3> void LoopCells(Op1 f, Op2 &g, Op3 &h) {
    for (vector<Cell *>::iterator i=cells.begin();
	 i!=cells.end();
	 i++) {
      f(*i,g,h);
    }
  }

  void DoCellHouseKeeping() {
	if (activateWallSliding()) {
		WallCollapse();
	}
    vector<Cell *> current_cells = cells;
    for (vector<Cell *>::iterator i = current_cells.begin();
    		i != current_cells.end();
    		i ++) {
    	plugin->CellHouseKeeping(*i);
    }
    for (vector<Cell *>::iterator i = current_cells.begin();
    		i != current_cells.end();
    		i ++) {
    	bool anyBorderSpikeRemoved=false;
    	if ((*i)->curvedWallElementToHandle->removeSpike()){
    		anyBorderSpikeRemoved=(*i)->curvedWallElementToHandle->isBorderCase();
    		cout << ' ' << (*i)->curvedWallElementToHandle->Index() << '\n';
    	}
    	(*i)->curvedWallElementToHandle->reset();
    	if (anyBorderSpikeRemoved) {
    		RepairBoundaryPolygon();
    	}
    }
    for (vector<Cell *>::iterator i = current_cells.begin();
    		i != current_cells.end();
    		i ++) {
		// Call functions of Cell that cannot be called from CellBase, including Division
		if ((*i)->flag_for_divide) {
			if ((*i)->division_axis) {
				(*i)->DivideOverAxis(*(*i)->division_axis);
				delete (*i)->division_axis;
				(*i)->division_axis = 0;
			} else {
				(*i)->Divide();
			}
			(*i)->flag_for_divide=false;
		}
    }

  }

  // Apply "f" to cell i
  // i.e. this is an adapter which allows you to call a function
  // operating on Cell on its numeric index index
  template<class Op> void cell_index_adapter(Op f,int i) {
    f(cells[i]);
  }

  double SlideWallElements(void);
  double SlideWallElements2(void);
  double DisplaceNodes(void);
  void WallRelaxation(void);
  void ElasticModulus(double elastic_modulus) {this->elastic_modulus=elastic_modulus;}
  void PotentialSlideAngle(double potential_slide_angle) {this->potential_slide_angle=potential_slide_angle;}
  void WallCollapse();
  void CompatibilityLevel(int compatibility_level) {this->compatibility_level=compatibility_level;}
  bool activateWallStiffnessHamiltonian() {return (this->compatibility_level & WALL_STIFFNESS_HAMILTONIAN) != 0;}
  bool activateWallSliding() {return (this->compatibility_level & WALL_SLIDING) != 0;}

  void BoundingBox(Vector &LowerLeft, Vector &UpperRight);
  int NEqs(void) {     int nwalls = walls.size();
    int ncells =cells.size();
    int nchems = Cell::NChem();

    // two eqs per chemical for each walls, and one eq per chemical for each cell
    // This is for generality. For a specific model you may optimize
    // this by removing superfluous (empty) equations.
    int neqs = 2 * nwalls * nchems + ncells * nchems;

    return neqs;
  }
  void IncreaseCellCapacityIfNecessary(void) {

    return;
    // cerr << "Entering Mesh::IncreaseCellCapacityIfNecessary \n";
    // make sure we always have enough space 
    // to have each cell divide at least once
    //
    // Note that we must do this, because Cell::Divide pushes a new Cell
    // onto Mesh::cells. As a result, Mesh::cells might be relocated 
    // if we are _within_ a Cell object: i.e. pointer "this" will be changed!!
    // 
    // An alternative solution could be to make "Mesh::cells" a list,
    // but this won't work because we need random access for 
    // the Monte Carlo algorithm.

    if (2*cells.size()>cells.capacity()) {
      cerr << "Increasing capacity to "  << 2*cells.capacity() << endl;
      cerr << "Current capacity is " << cells.capacity() << endl;
      cells.reserve(cells.capacity()*2);
    }
  }

  void ReserveMoreCells(int n) {
    if (nodes.size()+n>nodes.capacity()) {
      nodes.reserve(size()+n);
    }
  }
  double Area(void);
  double MeanArea(void) {
    double sum=0.;
    for (vector<Cell *>::const_iterator i=cells.begin();
	 i!=cells.end();
	 i++) {
      sum+=(*i)->Area();
    }
    return sum/(double)NCells();
  }

  void SetBaseArea(void);
  int NCells(void) const {
    return cells.size();
  }
  inline int NNodes(void) const {
    return nodes.size();
  }
  void PrintQueue(ostream &os) {
    while (!node_insertion_queue.empty()) {
      os << node_insertion_queue.front() << endl;
      node_insertion_queue.pop();
    }
  }

  void InsertNodes(void) {
    // insert the nodes in the insertion queue
    while (!node_insertion_queue.empty()) {

      //cerr << node_insertion_queue.front() << endl;
      InsertNode(node_insertion_queue.front());
      node_insertion_queue.pop();
    }

  }

  void Clear(); 

  void ReactDiffuse( double delta_t = 1 );
  double SumChemical(int ch);
  void SetChemical(int ch, double value) {
    for (vector<Cell *>::iterator c=cells.begin();
	 c!=cells.end();
	 c++) {
      (*c)->chem[ch]=value;
    }
  }

  // used for interacing with ODE-solvers (e.g. NRCRungeKutta)
  void setValues(double x, double *y);
  double *getValues(int *neqs);
  void Derivatives(double *derivs);
#ifdef QTGRAPHICS
  inline void DrawBoundary(QGraphicsScene *c) {
    boundary_polygon->Draw(c);
  }
  void DrawNodes(QGraphicsScene *c) const;

#endif
  double max_chem;

  void XMLSave(const char *docname, QDomElement &settings) const;
  void XMLRead(const char *docname, QDomElement &settings, bool geometry = true, bool pars = true, bool simtime = true);
  //void QtXMLRead(const char *docname, QDomElement &settings, bool geometry = true, bool pars = true, bool simtime = true);
  void XMLReadPars(const QDomElement &a_node);
  void XMLReadGeometry(QDomElement root_node);
  void XMLReadSimtime(const QDomElement &root_node);
  void XMLReadNodes(QDomElement &cur);
  void XMLReadCells(QDomElement &root);
  void XMLParseTree(const QDomElement &root_node);
  void XMLReadWalls(QDomElement &cur, vector<Wall *> *tmp_cells);
  void XMLReadWallsToCells(QDomElement &root, vector<Wall *> *tmp_walls);
  void XMLReadNodeSets(QDomElement &root);
  void XMLReadNodeSetsToNodes(QDomElement &root);
  void PerturbChem(int chemnum, double range);
  void CleanUpCellNodeLists(void);
  void CleanUpWalls(void);
  void CutAwayBelowLine( Vector startpoint, Vector endpoint );
  void CutAwaySAM(void);
  void RepairBoundaryPolygon(void);
  void Rotate(double angle, Vector center);
  void PrintWallList( void );
  void TestIllegalWalls(void);
  Vector FirstConcMoment(int chem);
  inline Vector Centroid(void) {
    return boundary_polygon->Centroid();
  }

  inline Vector Offset(void) {
    return boundary_polygon->Offset();
  }

  inline double Factor(void) {
    return boundary_polygon->Factor();
  }

  void DeleteLooseWalls(void);
  void FitLeafToCanvas(double width, double height);
  void AddNodeSet(NodeSet *node_set) {
    node_sets.push_back(node_set);
  }

  void CleanChemicals(const vector<double> &clean_chem);
  void CleanTransporters(const vector<double> &clean_transporters);
  void RandomizeChemicals(const vector<double> &max_chem, const vector<double> &max_transporters);
  inline double getTime(void) const { return time; }
  string getTimeHours(void) const; 
  inline void setTime(double t) { time = t; }
  double CalcProtCellsWalls(int ch) const;  
  void SettoInitVals(void);
  QVector<qreal> VertexAngles(void);
  QVector< QPair<qreal,int> > VertexAnglesValues(void);
  void SetSimPlugin(SimPluginInterface *new_plugin) {
    plugin=new_plugin;
  }
  QString ModelID(void) { return plugin?plugin->ModelID():QString("undefined"); }
  void StandardInit(void);	
  double Compactness(double *res_compactness=0, double *res_area=0, double *res_cell_area=0, double *hull_circumference=0);
  void CSVExportCellData(QTextStream &csv_stream) const;
  void CSVExportWallData(QTextStream &csv_stream) const;
  void CSVExportMeshData(QTextStream &csv_stream);
  
  Node* findNextBoundaryNode(Node*);

 private:
  list<Node *>* cellNodes(Cell * cell) ;
  // Data members
  vector<Cell *> cells;
  vector<Node *> nodes;
  list<Wall *> walls; // we need to erase elements from this container frequently, hence a list.
  int compatibility_level;
  double elastic_modulus;
  double potential_slide_angle;
 public:
  vector<NodeSet *> node_sets;
 private:
  vector<Node *> shuffled_nodes;
  vector<Cell *> shuffled_cells;
  unique_queue<Edge> node_insertion_queue;
  BoundaryPolygon *boundary_polygon;
  double time;
  SimPluginInterface *plugin;

  // Private member functions
  double CellSpecificStiffnessOneSide(Node *nb,set<int> &nodeown);
  void AddNodeToCell(Cell *c, Node *n, Node *nb1 , Node *nb2);
  void AddNodeToCellAtIndex(Cell *c, Node *n, Node *nb1 , Node *nb2, list<Node *>::iterator ins_pos);
  void InsertNode(Edge &e);
  Cell * getOtherCell(Cell* c,Node* node1,Node * node2);
  double SlideWallElement2(Cell* c,Node* prev,Node* fromNode,Node* toNode) ;
  double SlideWallElement(Cell* c,Node* w0,Node* w1,Node* w2,Node* w3,Node* w4) ;
  bool findOtherSide(Cell * c,Node * z1,Node * z2,Node ** w0,Node ** w1,Node ** w2,Node ** w3);
  inline Node *AddNode(Node *n) {
    nodes.push_back(n);
    shuffled_nodes.push_back(n);
    n->m=this;
    return n;
  }

  inline Cell *AddCell(Cell *c) {
    cells.push_back(c);
    shuffled_cells.push_back(c);
    //cerr << "Shuffled cell indices:  ";
    /*copy(shuffled_cells.begin(),shuffled_cells.end(),ostream_iterator<int>(cerr," "));
      cerr << endl;*/
    c->m=this;
    return c;
  }

  void CircumCircle(double x1,double y1,double x2,double y2,double x3,double y3,
		    double *xc,double *yc,double *r);
  void calculateWallStiffness(CellBase *c,Node* node, double *w_p1,double *w_p2, double *bl_minus_1, double *bl_plus_1);

  void updateAreasOfCells(list<DeltaIntgrl> * delta_intgrl_list,Node * node);
};
#endif

/* finis */
