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

#ifndef _CELL_H_
#define _CELL_H_

#include <list>
#include <vector>
#include <iostream>
#include <QString>
#include "vector.h"
#include "parameter.h"
#include "wall.h"
#include "warning.h"
#include "cellbase.h"
//#include "cell.h"

#include <QGraphicsScene>
#include <qcolor.h>
#include <QObject>
#include <QMouseEvent>

class Cell : public CellBase 
{

  Q_OBJECT
    friend class Mesh;
  friend class FigureEditor;

 public:
  Cell(double x, double y, double z = 0.);
  Cell(void);
  Cell(const Cell &src);
  Cell operator=(const Cell &src);
  bool Cmp(Cell*) const;
  bool Eq(Cell*) const;

  inline bool IndexEquals(int i) { return i == index; }

  static void SetMagnification(const double &magn) {
    factor=magn;
  }
  static Vector Offset(void) {
    Vector offs;
    offs.x=offset[0];
    offs.y=offset[1];
    return offs;
  }

  static void Translate(const double &tx,const double &ty) {
    offset[0]+=tx;
    offset[1]+=ty;
  }

  inline static double Factor(void) {
    return factor;
  }
  static void setOffset(double ox, double oy) {
    offset[0]=ox;
    offset[1]=oy;
  }
  static double Magnification(void) {
    return factor;
  }

  static double Scale(const double scale) {
    factor*=scale;
    return factor;
  }

  void DivideOverAxis(Vector axis); // divide cell over axis

  // divide over the line (if line and cell intersect)
  bool DivideOverGivenLine(const Vector v1, const Vector v2, bool wall_fixed = false, NodeSet *node_set = 0);

  void Divide(void) { // Divide cell over short axis

    Vector long_axis; 
    Length(&long_axis); 
    DivideOverAxis(long_axis.Perp2D()); 
  }

  //void CheckForGFDrivenDivision(void);
  inline int NNodes(void) const { return nodes.size(); }

  void Move(Vector T);
  void Move(double dx, double dy, double dz=0) {
    Move( Vector (dx, dy, dz) );
  }

  double Displace(double dx, double dy, double dh);
  void Displace(void);
  double Energy(void) const;
  bool SelfIntersect(void);
  bool MoveSelfIntersectsP(Node *nid, Vector new_pos);
  bool LinePieceIntersectsP(const Vector v1, const Vector v2) const;
  bool IntersectsWithLineP(const Vector v1, const Vector v2);

  void XMLAdd(QDomDocument &doc, QDomElement &cells_node) const;

  void ConstructWalls(void);
  void Flux(double *flux, double *D);

  void OnClick(QMouseEvent *e);
  inline Mesh& getMesh(void) const { return *m; }
  double MeanArea(void);

  void Apoptose(void); // Cell kills itself
  list<Wall *>::iterator RemoveWall( Wall *w );
  void AddWall( Wall *w );
  void InsertWall( Wall *w );

  void Draw(QGraphicsScene *c, QString tooltip = "");

  // Draw a text in the cell's center
  void DrawText(QGraphicsScene *c, const QString &text) const;
  void DrawIndex(QGraphicsScene *c) const;
  void DrawCenter(QGraphicsScene *c) const;
  void DrawNodes(QGraphicsScene *c) const;
  void DrawMiddleLamella(QGraphicsScene *c, QString tooltip = "");

  void DrawAxis(QGraphicsScene *c) const;
  void DrawStrain(QGraphicsScene *c) const;
  void DrawFluxes(QGraphicsScene *c, double arrowsize = 1.);
  void DrawWalls(QGraphicsScene *c) const;
  void DrawValence(QGraphicsScene *c) const;
  void EmitValues(double t);

 signals:
  void ChemMonValue(double t, double *x);

 protected:
  void XMLAddCore(QDomDocument &doc, QDomElement &xmlcell) const;
  int XMLRead(QDomElement &cur);
  void DivideWalls(ItList new_node_locations, const Vector from, const Vector to, bool wall_fixed = false, NodeSet *node_set = 0);

 private:

  static QPen *cell_outline_pen;
  static double offset[3];
  static double factor;
  Mesh *m;
  void ConstructConnections(void);
  void SetWallLengths(void);
};


// Boundarypolygon is a special cell; will not increase ncells
//  and will not be part of Mesh::cells
class BoundaryPolygon : public Cell {

 public:
 BoundaryPolygon(void) : Cell() {
    NCells()--;
    index=-1;
  }

 BoundaryPolygon(double x,double y,double z=0) : Cell (x,y,z) {
    NCells()--;
    index=-1;
  }

  BoundaryPolygon &operator=(Cell &src) {
    Cell::operator=(src);
    index=-1;
    return *this;
  }
  virtual void Draw(QGraphicsScene *c, QString tooltip = "");

  virtual void XMLAdd(QDomDocument &doc, QDomElement &parent_node) const;

  virtual bool BoundaryPolP(void) const { return true; } 
};

#endif

/* finis */
