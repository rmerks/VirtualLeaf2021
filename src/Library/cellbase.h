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

// CellBase derives from Vector, where Vector is simply used as a Vertex

#ifndef _CELLBASE_H_
#define _CELLBASE_H_

#include <list>
#include <vector>
#include <iostream>
#include <QString>
#include <QDebug>

#include "vector.h"
#include "parameter.h"
#include "nodebase.h"
#include "wall.h"
#include "warning.h"
#include "assert.h"
#include "wallelementinfo.h"


//#include "wallelementinfo.h"

extern Parameter par;
using namespace std;

class Mesh;
class Node;
class CellBase;
class NodeSet;
class WallElementInfo;
class WallElement;
class NodeBase;

struct ParentInfo {
  Vector polarization;
  double PINmembrane;
  double PINendosome;
};

// We need a little trick here, to make sure the plugin and the main application will see the same static datamembers
// otherwise each have their own instantation.
// My solution is as follows. I collect all original statics in a class. The main application instantiates it and
// has a static pointer to it. After loading the plugin I set a static pointer to the same class 
class CellsStaticDatamembers {

 public:
  CellsStaticDatamembers(void) {
    ncells = 0;
    nchem = 0;
    base_area = 0.;
  }
  ~CellsStaticDatamembers() {
/*#ifdef QDEBUG
    qDebug() << "Oops! Desctructor of CellsStaticDatamembers called" << endl;
#endif*/
  }
  int ncells;
  int nchem;
  double base_area;
};

class CellBase :  public QObject, public Vector 
{

  Q_OBJECT

  friend class Mesh;
  friend class CellInfo;
  friend class Node;
  friend class WallBase;
  friend class WallElement;
  friend class WallElementInfo;
  friend class SimPluginInterface;

 public:
  CellBase(QObject *parent=0);
  CellBase(double x,double y,double z=0); // constructor

  virtual ~CellBase() {
    delete[] chem;
    delete[] new_chem;
    if (division_axis) delete division_axis;
    //cerr << "CellBase " << index << " is dying. " << endl;
  }

  CellBase(const CellBase &src); // copy constructor
  virtual bool BoundaryPolP(void) const { return false; } 


  CellBase operator=(const CellBase &src); // assignment operator
  CellBase operator=(const Vector &src);

  void SetChemical(int chem, double conc);
  inline void SetNewChem(int chem, double conc)
  { 
    new_chem[chem] = conc;
  }

  void SetSource(int chem, double conc)
  {
    source=true;
    source_chem = chem;
    source_conc = conc;
  }

  // set chem 1 to conc in all membranes of this cell
  void SetTransporters(int chem, double conc);
  void UnfixNodes(void);
  void FixNodes(void);
  void UnsetSource(void) {
    source = false;
  }

  inline bool Source(void) { return source; }
  enum boundary_type {None, Noflux, SourceSink, SAM};
  static const char * boundary_type_names[4];

  inline const char *BoundaryStr(void) { return boundary_type_names[boundary]; }

  ostream &print(ostream &os) const;

  inline double Chemical(int c) const { // returns the value of chemical c
#ifdef _undefined_
    qDebug() << endl << "Entering cellbase::chemical(" << c << "), and nchem is: " << NChem() << "." << endl;
#endif

    int nchem = NChem();

#ifdef _undefined_
    if ((c<0) || (c>=nchem))
      MyWarning::warning("CellBase::Chemical says: index c is: %d, but nchem is: %d. Merely return zero", c, nchem);
#endif

    return ((c<0) || (c>=nchem)) ? 0 : chem[c];
  }


  //void print_nblist(void) const;

  boundary_type SetBoundary(boundary_type bound)
  {
    if (bound!=None) {
    }
    return boundary=bound;
  }

  boundary_type ResetBoundary(void) { return boundary=None; }

  boundary_type Boundary(void) const { return boundary; }

  static int &NChem(void) { return static_data_members->nchem; }

  double CalcArea(void) const;

  double RecalcArea(void) { return area = CalcArea(); }

  Vector Centroid(void) const;

  void SetIntegrals(void) const;

  double Length(Vector *long_axis = 0, double *width = 0) const;
  double CalcLength(Vector *long_axis = 0, double *width = 0) const;

  double ExactCircumference(void) const;
  inline int Index(void) const { return index; }


  void SetTargetArea(double tar_ar) { target_area=tar_ar; }

  inline void SetTargetLength(double tar_l) { target_length=tar_l; }

  inline void SetLambdaLength(double lambda_length) { lambda_celllength = lambda_length; }

  inline double TargetArea(void) { return target_area; }

  inline void SetStiffness(double stiff) { stiffness = stiff; }

  inline double Stiffness(void) { return stiffness; }

  inline double EnlargeTargetArea(double da) { return target_area+=da; }

  inline double Area(void) const { return area; }

  inline double SetWallStiffness(double value_stiffness) {return wall_stiffness = value_stiffness;}

  inline double GetWallStiffness(void) const {return wall_stiffness;}

  inline void Divide(void) { flag_for_divide = true; }

  inline void DivideOverAxis(const Vector &v)
  {
    division_axis = new Vector(v);
    flag_for_divide = true;
  }

  inline double WallCircumference(void) const {
    double sum=0.;
    for (list<Wall *>::const_iterator w=walls.begin();
	 w!=walls.end();
	 w++) {
      sum +=  (*w)->Length();
    }
    return sum;
  }





  QList<WallBase *> getWalls(void) {
    QList<WallBase *> wall_list;
    for (list<Wall *>::iterator i=walls.begin();
	 i!=walls.end();
	 i++) {
      wall_list << *i;
    }
    return wall_list;
  }


  template<class Op> void LoopWallElements(Op f) {
			WallElementInfo info;
			list <Node *>::iterator i=nodes.begin();
			Node * first=*i;
			Node * from=first;
			Node * to=*(++i);
			while (i!=nodes.end()) {
				fillWallElementInfo(&info,from,to);
		        f(&info);
				if (stopWallElementInfo(&info)) {
		        	return;
		        }
		        from=to;
		        to=*(++i);
			}
			fillWallElementInfo(&info,from,first);
			f(&info);
  }

  template<class Op> void LoopWalls(Op f) {
    for (list <Wall *>::iterator i=walls.begin();i!=walls.end();i++) {
      f(*i);
    }
  }

  template<class Op> void LoopNeighbors(Op f) {
    for (list <CellBase *>::iterator i=neighbors.begin();i!=neighbors.end();i++) {
      f(*i);
    }
  }

  void Dump(ostream &os) const;

  QString printednodelist(void);

  inline bool DeadP(void) { return dead; }
  inline void MarkDead(void) { dead  = true; }

  static double &BaseArea(void)
  { 
    return static_data_members->base_area;
  }

  void CheckForDivision(void);

  // write flux from neighboring cells into "flux"
  void Flux(double *flux, double *D); 
  inline bool FixedP(void) { return fixed; }
  inline bool Fix(void) {  FixNodes(); return (fixed=true); }
  inline bool Unfix(void) { UnfixNodes(); return (fixed=false);}
  inline void setCellVec(Vector cv) { cellvec = cv; }

  bool AtBoundaryP(void) const;

  static inline int &NCells(void)
  {
    return static_data_members->ncells;
  }

  inline void Mark(void)
  {
    marked=true;
  }

  inline void Unmark(void)
  {
    marked=false;
  }

  inline bool Marked(void) const {
    return marked;
  }

  //! Returns the number of neighbors excluding the BoundaryPolygon
  int CountNeighbors(void) {
      int n=0;
     // for (list<Wall *>::const_iterator w=walls.begin();
      for (auto w=walls.begin();
       w!=walls.end();
       w++) {
          if ((*w)->c1==this) {
              // only count actual neighbors
              if (!(*w)->c2->BoundaryPolP()) {
                  n++;
              }
          } else {
              if (!(*w)->c1->BoundaryPolP()) {
                  n++;
              }
          }
      }
      return n;
  }

  //! Returns the sum of chemical "chem" of this CellBase's neighbors
  double SumChemicalsOfNeighbors(int chem)
  {
    double sum=0.;
    for (auto w=walls.begin();
	 w!=walls.end();
	 w++) {
      sum +=  (*w)->c1!=this ? (*w)->c1->Chemical(chem) : (*w)->c2->Chemical(chem) ;
    }
    return sum;
  }

  //! Generalization of the previous member function
  template<class P, class Op> P ReduceNeighbors(Op f) {
    P sum=0;
    for (list<Wall *>::const_iterator w=walls.begin();
	 w!=walls.end();
	 w++) {
      sum += (*w)->c1 != this ? f( *((*w)->c1) ) : f ( *((*w)->c2) ); 
    }
    return sum;
  }

  //! The same, but now for the walls
  template<class P, class Op> P ReduceWalls(Op f, P sum) {
    for (list<Wall *>::const_iterator w=walls.begin();
	 w!=walls.end();
	 w++) {
      sum += f( **w ); 
    }
    return sum;
  }

  //! The same, but now for the walls AND neighbors
  template<class P, class Op> P ReduceCellAndWalls(Op f)
  {
    P sum = 0;
    for (list<Wall *>::const_iterator w=walls.begin();
	 w!=walls.end();
	 w++) {
      sum += ((*w)->c1 == this) ? 
	f( ((*w)->c1), ((*w)->c2), *w ) :  
	f( ((*w)->c2), ((*w)->c1), *w );

    }
    return sum;
  }


  //! Sum transporters at this CellBase's side of the walls
  double SumTransporters(int ch)
  {
    double sum=0.;
    for (list<Wall *>::const_iterator w=walls.begin();
	 w!=walls.end();
	 w++) {
      sum += (*w)->getTransporter(this, ch);

    }
    return sum;
  }

  inline int NumberOfDivisions(void) { return div_counter; }

  //! Sum transporters at this CellBase's side of the walls
  double SumLengthTransporters(int ch)
  {
    double sum=0.;
    for (list<Wall *>::const_iterator w=walls.begin();
	 w!=walls.end();
	 w++) {
      sum += (*w)->getTransporter(this, ch) * (*w)->Length();

    }
    return sum;
  }



  double SumLengthTransportersChemical(int trch, int ch)
  {
    double sum=0.;
    for (list<Wall *>::const_iterator w=walls.begin();
	 w!=walls.end();
	 w++) {
      sum += (*w)->getTransporter(this, trch) * ( (*w)->c1!=this ? (*w)->c1->Chemical(ch) : (*w)->c2->Chemical(ch) );

    }
    return sum;
  }
  inline int CellType(void) const { return cell_type; } 
  inline void SetCellType(int ct) { cell_type = ct; }


  static void SetNChem(int new_nchem)
  {
    if (NCells()) {
      MyWarning::error("CellBase::SetNChem says: not permitted, call SetNChem after deleting all cells.");
    } else {
      NChem() = new_nchem;
    }
  }

  inline double TargetLength() const { return target_length; } 

  static inline CellsStaticDatamembers *GetStaticDataMemberPointer(void) { return static_data_members; }
  void fillWallElementInfo(WallElementInfo * info,Node* from,Node* to) ;
  bool stopWallElementInfo(WallElementInfo * info);
  inline void removeWall(Wall * wall) {walls.remove(wall);}

 protected:
  // (define a list of Node* iterators)
  typedef list < list<Node *>::iterator > ItList;

  int index;

  inline void SetChemToNewchem(void)
  {
    for (int c=0;c<CellBase::NChem();c++) {
      chem[c]=new_chem[c];
    }
  }
  inline void SetNewChemToChem(void)
  {
    for (int c=0;c<CellBase::NChem();c++) {
      new_chem[c]=chem[c];
    }
  }
  inline double NewChem(int c) const { return new_chem[c]; }

  list<Node *> nodes;
  void ConstructNeighborList(void);
  long wall_list_index (Wall *elem) const;

  // DATA MEMBERS

  // list of nodes, in clockwise order

  // a (non-ordered) list of neighboring cells (actually I think the
  // introduction of ConstructWalls() has made these
  // lists ordered (clockwise), but I am not yet 100% sure...).
  list<CellBase *> neighbors;

  list<Wall *> walls;

  double *chem;
  double *new_chem;

  boundary_type boundary;
  mutable double area;
  double target_area;
  double target_length;
  double lambda_celllength;
  double wall_stiffness; // Lebovka et al

  double stiffness; // stiffness like in Hogeweg (2000)

  bool fixed;
  bool pin_fixed;
  bool at_boundary; 
  bool dead; 
  bool flag_for_divide;

  Vector *division_axis;
  int cell_type;

  // for length constraint
  mutable double intgrl_xx, intgrl_xy, intgrl_yy, intgrl_x, intgrl_y;

  bool source;
  Vector cellvec;

  // STATIC DATAMEMBERS MOVED TO CLASS
  static CellsStaticDatamembers *static_data_members;
  double source_conc;
  int source_chem;

  // PRIVATE MEMBER FUNCTIONS
  inline static void ClearNCells(void)
  {
    NCells()=0;
  }

  bool marked;
  int div_counter;

};

ostream &operator<<(ostream &os, const CellBase &v);

inline Vector PINdir(CellBase *here, CellBase *nb, Wall *w)
{
  return w->getTransporter( here, 1)  *  w->getInfluxVector(here);
}

#endif

/* finis*/
