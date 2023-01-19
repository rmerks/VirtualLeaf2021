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

#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#ifdef QTGRAPHICS
#include <QGraphicsScene>
#include <qpainter.h>
#include <qcolor.h>
#include <qfont.h>
#include <qwidget.h>

#include <fstream>
#include "nodeitem.h"
#include "cellitem.h"
#include "qcanvasarrow.h"
#endif
#include "nodeset.h"

#include "cellbase.h"
#include "wall.h"
#include "random.h"
#include "parameter.h" 
#include "mesh.h"
#include "sqr.h"
#include "tiny.h"
#include "pi.h"

static const std::string _module_id("$Id$");

extern Parameter par;

const char* CellBase::boundary_type_names[4] = {"None", "NoFlux", "SourceSink", "SAM"};

#ifndef VLEAFPLUGIN
CellsStaticDatamembers *CellBase::static_data_members = new CellsStaticDatamembers();
#else
CellsStaticDatamembers *CellBase::static_data_members = 0;
#endif

CellBase::CellBase(QObject *parent) : 
  QObject(parent),
  Vector()
{

  chem=new double[NChem()];
  for (int i=0;i<NChem();i++) {
    chem[i]=0.;
  }
  new_chem=new double[NChem()];
  for (int i=0;i<NChem();i++) {
    new_chem[i]=0.;
  }
  boundary=None;
  index=(NCells()++);
  area=0.;
  target_area=1;
  target_length=0; //par.target_length;
  lambda_celllength = 0; //par.lambda_celllength;
  intgrl_xx=0.; intgrl_xy=0.; intgrl_yy=0.;
  intgrl_x=0.; intgrl_y=0.;
  source = false;
  source_conc = 0.;
  source_chem = 0;
  at_boundary=false;
  fixed = false;
  pin_fixed = false;
  stiffness = 0;
  marked = false;
  dead = false;
  div_counter=0;
  cell_type = 0;
  flag_for_divide = false;
  division_axis = 0;
}


CellBase::CellBase(double x,double y,double z) : QObject(), Vector(x,y,z)
{
#ifndef VLEAFPLUGIN
  if (static_data_members == 0) {
    static_data_members = new CellsStaticDatamembers();
  }
#endif
  chem=new double[NChem()];
  for (int i=0;i<NChem();i++) {
    chem[i]=0.;
  }
  new_chem=new double[NChem()];
  for (int i=0;i<NChem();i++) {
    new_chem[i]=0.;
  }
  boundary=None;
  area=0.;
  target_area=1;
  target_length=0; //par.target_length;
  lambda_celllength=0; // par.lambda_celllength;

  index=(NCells()++);

  intgrl_xx=0.; intgrl_xy=0.; intgrl_yy=0.;
  intgrl_x=0.; intgrl_y=0.;

  source = false;
  fixed = false;
  at_boundary=false;
  pin_fixed = false;
  stiffness = 0;
  marked=false;
  dead  = false;
  div_counter = 0;
  cell_type = 0;
  flag_for_divide = false;
  division_axis = 0;
}

CellBase::CellBase(const CellBase &src) :  QObject(), Vector(src)
{

  chem=new double[NChem()];
  for (int i=0;i<NChem();i++) {
    chem[i]=src.chem[i];
  }
  new_chem=new double[NChem()];
  for (int i=0;i<NChem();i++) {
    new_chem[i]=src.new_chem[i];
  }
  boundary=src.boundary;
  area=src.area;
  target_length=src.target_length;
  lambda_celllength=src.lambda_celllength;

  intgrl_xx=src.intgrl_xx; intgrl_xy=src.intgrl_xy; intgrl_yy=src.intgrl_yy;
  intgrl_x=src.intgrl_x; intgrl_y=src.intgrl_y;

  target_area=src.target_area;
  index=src.index;
  nodes=src.nodes;
  neighbors=src.neighbors;
  walls=src.walls;
  source = src.source;
  fixed = src.fixed;
  source_conc = src.source_conc;
  source_chem = src.source_chem;
  cellvec = src.cellvec;
  at_boundary=src.at_boundary;
  pin_fixed = src.pin_fixed;
  stiffness = src.stiffness;
  marked = src.marked;
  dead = src.dead;
  cell_type = src.cell_type;
  div_counter = src.div_counter;
  flag_for_divide = src.flag_for_divide;
  division_axis = src.division_axis;
}


CellBase CellBase::operator=(const CellBase &src)
{
  Vector::operator=(src);

  for (int i=0;i<NChem();i++) {
    chem[i]=src.chem[i];
  }
  for (int i=0;i<NChem();i++) {
    new_chem[i]=src.chem[i];
  }
  boundary=src.boundary;
  area=src.area;
  intgrl_xx=src.intgrl_xx; intgrl_xy=src.intgrl_xy; intgrl_yy=src.intgrl_yy;
  intgrl_x=src.intgrl_x; intgrl_y=src.intgrl_y;
  target_area=src.target_area;
  target_length=src.target_length;
  lambda_celllength=src.lambda_celllength;

  index=src.index;

  nodes=src.nodes;
  neighbors=src.neighbors;
  walls=src.walls;
  source = src.source;
  fixed = src.fixed;
  source_conc = src.source_conc;
  source_chem = src.source_chem;
  cellvec = src.cellvec;
  at_boundary=src.at_boundary;
  pin_fixed = src.pin_fixed;
  stiffness = src.stiffness;
  marked = src.marked;
  dead = src.dead;
  cell_type = src.cell_type;
  div_counter = src.div_counter;
  flag_for_divide = src.flag_for_divide;
  division_axis = src.division_axis;
  return *this;
}

void CellBase::SetChemical(int c, double conc)
{
  if (c>=NChem()) {
    stringstream error;
    error << "SetChemical: value c = " << c << " is out of range\n";
    throw error.str().c_str();
  }
  chem[c]=conc;
}

void CellBase::SetTransporters(int ch, double conc)
{
  if (ch>=NChem()) {
    stringstream error;
    error << "SetChemical: value ch = " << ch << " is out of range\n";
    throw error.str().c_str();
  }
  for (list<Wall *>::iterator w=walls.begin(); w!=walls.end(); w++) {
    (*w)->setTransporter(this, ch, conc);
  }
}

ostream &CellBase::print(ostream &os) const
{


  os << "[ index = " << index << " {" << x << ", " << y << ", " << z << "}: {";

  for (int i=0;i<NChem()-1;i++) {
    os << chem[i] << ", ";
  }

  os << chem[NChem()-1] << " } ]";

  os << endl << "Nodelist = { " << endl;

  for (list<Node *>::const_iterator i =  nodes.begin(); i!=nodes.end(); i++) {
    os << (*i)->Index() << "( " << *i << ") ";
  }
  os << " } ";

  for (list<Wall *>::const_iterator i =  walls.begin(); i!=walls.end(); i++) {
    (*i)->print(os);
    os << ", ";
  } 
  os << endl;

  os << " [ area = " << area << " ]";
  os << " [ walls = ";

  for (list<Wall *>::const_iterator i= walls.begin(); i!=walls.end(); i++) {
    os << (*i)->n1->Index() << " -> " << (*i)->n2->Index() << ", " <<  (*i)->c1->Index() << " | " << (*i)->c2->Index() << ", ";
  }
  os << " ] ";
  os << "div_counter = " << div_counter << endl;
  os << "cell_type = " << cell_type << endl;
  os << endl;
  return os;
}

ostream &operator<<(ostream &os, const CellBase &c)
{
  c.print(os);
  return os;
}


double CellBase::CalcArea(void) const
{

  double loc_area=0.;

  for (list<Node *>::const_iterator i=nodes.begin(); i!=(nodes.end()); i++) {

    list<Node *>::const_iterator i_plus_1=i; i_plus_1++;
    if (i_plus_1==nodes.end())
      i_plus_1=nodes.begin();

    loc_area+= (*i)->x * (*i_plus_1)->y;
    loc_area-= (*i_plus_1)->x * (*i)->y;
  }

  // http://technology.niagarac.on.ca/courses/ctec1335/docs/arrays2.pdf	
  return fabs(loc_area)/2.0; 
} 

Vector CellBase::Centroid(void) const
{

  double area=0.;
  double integral_x_dxdy=0.,integral_y_dxdy=0.;

  for (list<Node *>::const_iterator i=nodes.begin(); i!=(nodes.end()); i++) {

    list<Node *>::const_iterator i_plus_1=i; i_plus_1++;
    if (i_plus_1==nodes.end())
      i_plus_1=nodes.begin();

    area+= (*i)->x * (*i_plus_1)->y;
    area-= (*i_plus_1)->x * (*i)->y;

    integral_x_dxdy+=
      ((*i_plus_1)->x+(*i)->x)*
      ((*i)->x*(*i_plus_1)->y-
       (*i_plus_1)->x*(*i)->y);
    integral_y_dxdy+=
      ((*i_plus_1)->y+(*i)->y)*
      ((*i)->x*(*i_plus_1)->y-
       (*i_plus_1)->x*(*i)->y);
  }

  area = fabs(area)/2.0;

  integral_x_dxdy/=6.;
  integral_y_dxdy/=6.;

  Vector centroid(integral_x_dxdy,integral_y_dxdy,0);
  centroid/=area;
  return centroid;
}



void CellBase::SetIntegrals(void) const
{

  // Set the initial values for the integrals over x^2,
  // xy, yy, x, and y

  // these values will be updated after each move of the CellBase wall

  intgrl_xx=0.; intgrl_xy=0.; intgrl_yy=0.;
  intgrl_x=0.; intgrl_y=0.;
  area=0.;
  list<Node *>::const_iterator nb;
  list<Node *>::const_iterator i=nodes.begin();

  for (; i!=(nodes.end()); i++) {

    nb = i; nb++; if (nb==nodes.end()) nb=nodes.begin();

    area+=(*i)->x*(*nb)->y;
    area-=(*nb)->x*(*i)->y;
    intgrl_xx+= 
      ((*i)->x*(*i)->x+
       (*nb)->x*(*i)->x+
       (*nb)->x*(*nb)->x ) *
      ((*i)->x*(*nb)->y-
       (*nb)->x*(*i)->y);
    intgrl_xy+= 
      ((*nb)->x*(*i)->y-
       (*i)->x*(*nb)->y)*
      ((*i)->x*(2*(*i)->y+(*nb)->y)+
       (*nb)->x*((*i)->y+2*(*nb)->y));
    intgrl_yy+=
      ((*i)->x*(*nb)->y-
       (*nb)->x*(*i)->y)*
      ((*i)->y*(*i)->y+
       (*nb)->y*(*i)->y+
       (*nb)->y*(*nb)->y );
    intgrl_x+=
      ((*nb)->x+(*i)->x)*
      ((*i)->x*(*nb)->y-
       (*nb)->x*(*i)->y);
    intgrl_y+=
      ((*nb)->y+(*i)->y)*
      ((*i)->x*(*nb)->y-
       (*nb)->x*(*i)->y);
  }
  area = fabs(area)/2.0;
}

double CellBase::Length(Vector *long_axis, double *width)  const
{

  // Calculate length and axes of CellBase

  // Calculate inertia tensor
  // see file inertiatensor.nb for explanation of this method
  if (!lambda_celllength) {

    // Without length constraint we do not keep track of the cells'
    // moments of inertia. So we must calculate them here.
    SetIntegrals();
  }

  double intrx=intgrl_x/6.;
  double intry=intgrl_y/6.;
  double ixx=(intgrl_xx/12.)-(intrx*intrx)/area;
  double ixy=(intgrl_xy/24.)+(intrx*intry)/area;
  double iyy=(intgrl_yy/12.)-(intry*intry)/area;

  double rhs1=(ixx+iyy)/2., rhs2=sqrt( (ixx-iyy)*(ixx-iyy)+4*ixy*ixy )/2.;

  double lambda_b=rhs1+rhs2;

  // see: http://scienceworld.wolfram.com/physics/MomentofInertiaEllipse.html
  //    cerr << "n = " << n << "\n";

  if (long_axis) {
    *long_axis = Vector(-ixy, lambda_b - ixx, 0);
    //   cerr << "ixx = " << ixx << ", ixy = " << ixy << ", iyy = " << iyy << ", area = " << area << endl;
  }

  if (width) {
    *width = 4*sqrt((rhs1-rhs2)/area);
  }

  return 4*sqrt(lambda_b/area);
}

double CellBase::CalcLength(Vector *long_axis, double *width)  const
{

  // Calculate length and axes of CellBase, without touching cells raw moments

  // Calculate inertia tensor
  // see file inertiatensor.nb for explanation of this method

  double my_intgrl_xx=0., my_intgrl_xy=0., my_intgrl_yy=0.;
  double my_intgrl_x=0., my_intgrl_y=0., my_area=0.;
  my_area=0.;
  list<Node *>::const_iterator nb;
  list<Node *>::const_iterator i=nodes.begin();

  for (; i!=(nodes.end()); i++) {

    nb = i; nb++; if (nb==nodes.end()) nb=nodes.begin();

    my_area+=(*i)->x*(*nb)->y;
    my_area-=(*nb)->x*(*i)->y;
    my_intgrl_xx+= 
      ((*i)->x*(*i)->x+
       (*nb)->x*(*i)->x+
       (*nb)->x*(*nb)->x ) *
      ((*i)->x*(*nb)->y-
       (*nb)->x*(*i)->y);
    my_intgrl_xy+= 
      ((*nb)->x*(*i)->y-
       (*i)->x*(*nb)->y)*
      ((*i)->x*(2*(*i)->y+(*nb)->y)+
       (*nb)->x*((*i)->y+2*(*nb)->y));
    my_intgrl_yy+=
      ((*i)->x*(*nb)->y-
       (*nb)->x*(*i)->y)*
      ((*i)->y*(*i)->y+
       (*nb)->y*(*i)->y+
       (*nb)->y*(*nb)->y );
    my_intgrl_x+=
      ((*nb)->x+(*i)->x)*
      ((*i)->x*(*nb)->y-
       (*nb)->x*(*i)->y);
    my_intgrl_y+=
      ((*nb)->y+(*i)->y)*
      ((*i)->x*(*nb)->y-
       (*nb)->x*(*i)->y);
  }


  //my_area/=2.0;
  my_area = fabs(my_area)/2.0;


  double intrx=my_intgrl_x/6.;
  double intry=my_intgrl_y/6.;
  double ixx=(my_intgrl_xx/12.)-(intrx*intrx)/my_area;
  double ixy=(my_intgrl_xy/24.)+(intrx*intry)/my_area;
  double iyy=(my_intgrl_yy/12.)-(intry*intry)/my_area;

  double rhs1=(ixx+iyy)/2., rhs2=sqrt( (ixx-iyy)*(ixx-iyy)+4*ixy*ixy )/2.;

  double lambda_b=rhs1+rhs2;

  // see: http://scienceworld.wolfram.com/physics/MomentofInertiaEllipse.html
  //    cerr << "n = " << n << "\n";

  if (long_axis) {
    *long_axis = Vector(-ixy, lambda_b - ixx, 0);
    //   cerr << "ixx = " << ixx << ", ixy = " << ixy << ", iyy = " << iyy << ", my_area = " << my_area << endl;
  }

  if (width) {
    *width = 4*sqrt((rhs1-rhs2)/my_area);
  }

  return 4*sqrt(lambda_b/my_area);
}


void CellBase::ConstructNeighborList(void)
{

  neighbors.clear();
  for (//list<Wall *>::const_reverse_iterator wit=walls.rbegin();
       list<Wall *>::const_iterator wit=walls.begin();
       // somehow the reverse_iterator returns by walls needs to be casted to const to let this work.
       // it seems to me it is a bug in the STL implementation...

       wit!=walls.end();
       wit++) {

    if ((*wit)->C1() != this) {
      neighbors.push_back((*wit)->C1());
    } else {
      neighbors.push_back((*wit)->C2());
    }

  }


  // remove all boundary_polygons from the list
  list <CellBase *>::iterator e=neighbors.begin();
  at_boundary=false;

  do { 
    // Code crashes here after cutting off part of the leaf. I can't find the problem.
    // Leaving the "Illegal" walls in the simulation helps. (c1=-1 && c2=-1)
    // Work-around: define leaf primordium. Save to XML. Restart. Read XML file.
    // Sorry about this; I hope to solve this annoying issue later. RM :-).
    // All cells in neighbors seem to be okay (I might be messing some part of the memory elsewhere
    // during the cutting operation?).
    e = find_if(neighbors.begin(),neighbors.end(),mem_fn(&CellBase::BoundaryPolP));
    if (e!=neighbors.end()) {
      e=neighbors.erase(e);
      at_boundary=true;
    } else {
      break;
    }
  } while(1);
}

// Save the cell to a stream so we can reconstruct its state later
void CellBase::Dump(ostream &os) const
{


  os << index << " " << nodes.size() << endl;

  Vector::Dump(os);
  os << endl;

  for (list<Node *>::const_iterator i=nodes.begin();i!=nodes.end();i++) {
    os << *i << " ";
  }
  os << endl;


  os << index << " " << neighbors.size() << endl;
  for (list<CellBase *>::const_iterator i=neighbors.begin();i!=neighbors.end();i++) {
    os << *i << " ";
  }

  os << endl << walls.size() << endl << endl;
  os << NChem() << " ";

  for (int i=0;i<NChem();i++) {
    os << chem[i] << " ";
  }
  os << endl;

  os << NChem() << " ";
  for (int i=0;i<NChem();i++) {
    os << new_chem[i] << " ";
  }
  os << endl;

  os << boundary << " " << area << " " << target_area << " " << target_length 
     << " " << fixed << " " << intgrl_xx << " " << intgrl_xy << " " << intgrl_yy 
     << " " << intgrl_x << " " << intgrl_y << " " << source << " ";

  cellvec.Dump(os);

  os << " " << source_conc << " " << source_chem;
  os << endl;
}


void CellBase::UnfixNodes(void)
{
  for (list<Node *>::const_iterator i=nodes.begin(); i!=nodes.end(); i++) {
    (*i)->Unfix();
  }
}


void CellBase::FixNodes(void)
{
  for (list<Node *>::const_iterator i=nodes.begin(); i!=nodes.end(); i++) { 
    (*i)->Fix();
  }
}

// returns true if cell is at border
bool CellBase::AtBoundaryP(void) const
{
  return at_boundary;
}


QString CellBase::printednodelist(void)
{
  QString info_string = "Nodelist = { ";
  for (list<Node *>::const_iterator i =  nodes.begin(); i!=nodes.end(); i++) {
    info_string += QString("%1 ").arg((*i)->Index());
  }
  info_string += " } ";
  return info_string;
}

Vector CellBase::step(list<Node *>::const_iterator i,double add,double substract)
{
    list<Node *>::const_iterator step=i;
    Vector current;
    while (add>0) {
    	current.x=(*step)->x;
    	current.y=(*step)->y;
    	step++;
    	if (step==nodes.end()) {
    		step=nodes.begin();
    	}
    	current.x-=(*step)->x;
    	current.y-=(*step)->y;
    	add-=(current.Norm());
    }
    while (substract>0){
    	current.x=(*step)->x;
    	current.y=(*step)->y;
        if (step==nodes.begin()) {
        	step=nodes.end();
        }
        step--;
    	current.x-=(*step)->x;
    	current.y-=(*step)->y;
    	substract-=(current.Norm());
    }
    Vector result;
    result.x=(*step)->x;
    result.y=(*step)->y;
    return result;
}

double angleRange(double angle) {
	angle=abs(angle);
    if (angle> Pi) {
    	angle = 2*Pi - angle;
    }
	return angle;
}

 double angle(Vector* v1,Vector* v2,Vector* m12){
    m12->x = (v1->x+v2->x)/2 ;
    m12->y = (v1->y + v2->y)/2 ;
    double angle1  = angleRange(v1->Angle(*v2));
    double angle1_ = angleRange(m12->Angle(*v2));
    if (angle1<(Pi/45)){
    	angle1=(Pi/45);
    	angle1_=angle1/2;
    }

    // angle1_ should be half of angle1 else take the other side
    if (angle1 < (angle1_*2)*1.001 && angle1 > (angle1_*2)*0.999) {
        return angle1;
    }
        angle1 = Pi - angle1;
        if (angle1 < (angle1_*2)*1.001 && angle1 > (angle1_*2)*0.999) {
            return angle1;
        }
        // if the other side also not fits, exit with biggest angle (probably rounding error.
    return Pi;
}
 double angleCostFunction(double angledir,double angle1,double angle2){
	    if (angledir> Pi) {
	    	angledir = abs(2*Pi - angle2);
	    }
	    return angledir+(angle1+angle2)*2;
 }


 int findMin(double closeAnglesMiddel,double*closeAngles,int count,double* resultCost){

 int index= 0;
 int flip=0;
 bool lastWasOver= closeAngles[index]> closeAnglesMiddel;
 //make sure we start searching after a gap
 while (!lastWasOver &&flip<3) {
	 if (++index==count) {index=0;flip++;}
	  lastWasOver= closeAngles[index]> closeAnglesMiddel;
 }
 //wait for the gap to start
 while (lastWasOver&&flip<3) {
	  if (++index==count) {index=0;flip++;}
	  lastWasOver= closeAngles[index]> closeAnglesMiddel;
 }
 int minIndex = index;
 int mincloseAngle = closeAngles[minIndex];
 //till end of gap
 while (!lastWasOver&&flip<3) {
	  if (closeAngles[index]<mincloseAngle) {
		  minIndex=index;
		  mincloseAngle = closeAngles[minIndex];
	  }
	  //do not use it again
	  closeAngles[index] = Pi*5;
	  if (++index==count) {index=0;flip++;}
	  lastWasOver= closeAngles[index]> closeAnglesMiddel;
 }
 *resultCost=mincloseAngle;
 return flip<3?minIndex:-1;
 }
QString CellBase::printednodes(void)
{
	  QString info_string = "\\begin{tikzpicture}\n";


	  double steplength = ExactCircumference()*0.05;
	  int count =   nodes.size();
	  double closeAnglesMax = 0;
	  double closeAnglesMin = Pi*5;
	  double *closeAngles =(double*)malloc(sizeof(double)*count);
	  int index= 0;
	  for (list<Node *>::const_iterator i=nodes.begin(); i!=(nodes.end()); i++) {
		    Vector v0;v0.x=(*i)->x;v0.y=(*i)->y;

		    Vector v10 = step(i,steplength*1,0)-v0;v10.Normalise();
		    Vector v20 = step(i,steplength*2,0)-v0;v20.Normalise();
			Vector v01 = step(i,0,steplength*1)-v0;v01.Normalise();
			Vector v02 = step(i,0,steplength*2)-v0;v02.Normalise();

			Vector m1;
			Vector m2;
		    double angle1  = angle(&v10,&v01,&m1);
		    double angle2  = angle(&v20,&v02,&m2);


		    double closeAngle=  angleCostFunction(m1.Angle(m2),angle1,angle2);
		    closeAngles[index++]=closeAngle;
			info_string += QString("% p(%1,%2) angle1 %3 angle2 %4 cost %5\n").arg(v0.x).arg(v0.y).arg(angle1).arg(angle2).arg(closeAngle);

			if (closeAnglesMax<closeAngle)
				closeAnglesMax=closeAngle;
			if (closeAnglesMin>closeAngle)
				closeAnglesMin=closeAngle;
	  }
	  double closeAnglesMiddel = (closeAnglesMax- closeAnglesMin)/2 +closeAnglesMin;
	  info_string += QString("% middle costs %1\n").arg(closeAnglesMiddel);

	  double minCost1 = Pi*5;
	  int sIndex1=-1;
	  double minCost2= Pi*5;
	  int sIndex2=-1;
	  double minCost;
	  int sIndex= findMin(closeAnglesMiddel,closeAngles,count,&minCost);
	  while (sIndex>=0) {
		  if (minCost<minCost1) {
			  minCost2=minCost1;
			  sIndex2=sIndex1;
			  minCost1=minCost;
			  sIndex1=sIndex;
		  }else if(minCost<minCost2){
			  minCost2=minCost;
			  sIndex2=sIndex;
		  }
		  sIndex= findMin(closeAnglesMiddel,closeAngles,count,&minCost);
	  }

	  free(closeAngles);
	  index = 0;
	  double rdistance1=-1;
	  double rdistance2=-1;
	  if (sIndex2 >=0) {
	  double distance1=-1;
	  double distance2=-1;
	  for (list<Node *>::const_iterator i=nodes.begin(); i!=(nodes.end()); ) {
		    list<Node *>::const_iterator i_plus_1=i; i_plus_1++;
		    if (i_plus_1==nodes.end())
		      i_plus_1=nodes.begin();

		    if (index==sIndex1) {
		  		distance1=0;
		  	}
	  	    double dx=((*i_plus_1)->x-(*i)->x);
	  	    double dy=((*i_plus_1)->y-(*i)->y);
	  	    double l=sqrt(dx*dx+dy*dy);
		  	if (distance1>=0) {
		  		if (index == sIndex2){
		  			rdistance1=distance1;
		  			distance2=0;
		  		}
		  		distance1+=l;
		  	}
		  	if (distance2>=0) {
		  		if (index == sIndex1){
		  			rdistance2=distance2;
		  			break;
		  		}
		  		distance2+=l;
		  	}
		    index++;
		    i++;
		    if (i==nodes.end()) {
		    	i=nodes.begin();
		    	index=0;
		    }
	  }
	  }

	  info_string += QString("% distance1 %1 distance2 %2\n").arg(rdistance1).arg(rdistance2);


	  index = 0;

	  for (list<Node *>::const_iterator i=nodes.begin(); i!=(nodes.end()); i++) {
		    list<Node *>::const_iterator i_plus_1=i; i_plus_1++;
		    if (i_plus_1==nodes.end())
		      i_plus_1=nodes.begin();
		    list<Node *>::const_iterator i_minus_1=i;
		    if (i_minus_1==nodes.begin())
		    	i_minus_1=nodes.end();
		    i_minus_1--;
		  	if (index==sIndex1) {
		  		double x= ((*i_minus_1)->x+(*i_plus_1)->x+(*i)->x)/3;
		  		double y= ((*i_minus_1)->y+(*i_plus_1)->y+(*i)->y)/3;
		  	  info_string += QString("\\filldraw[black] (%1,%2) circle (10pt) node[anchor=west]{Intersection point};\n").arg(x).arg(y);
		  	}
		  	if (index==sIndex2) {
		  		double x= ((*i_minus_1)->x+(*i_plus_1)->x+(*i)->x)/3;
		  		double y= ((*i_minus_1)->y+(*i_plus_1)->y+(*i)->y)/3;
		  	  info_string += QString("\\filldraw[black] (%1,%2) circle (10pt) node[anchor=west]{Intersection point};\n").arg(x).arg(y);
		  	}
		    info_string += QString("\\draw[gray, thick] (%1, %2) -- (%3, %4);\n").arg((*i)->x).arg((*i)->y).arg((*i_plus_1)->x).arg((*i_plus_1)->y);
		    index++;
	  }


	  info_string += "\\end{tikzpicture}";
	  return info_string;
}

double CellBase::ExactCircumference(void) const
{

  // simply sum length of all edges
  double circumference=0.;
    
  for (list<Node *>::const_iterator i=nodes.begin(); i!=(nodes.end()); i++) {

    list<Node *>::const_iterator i_plus_1=i; i_plus_1++;
    if (i_plus_1==nodes.end())
      i_plus_1=nodes.begin();

    double dx=((*i_plus_1)->x-(*i)->x);
    double dy=((*i_plus_1)->y-(*i)->y);
    double l=sqrt(dx*dx+dy*dy);
    //    f << (*i)->x << " " << (*i)->y << " " << (*i_plus_1)->x << " " << (*i_plus_1)->y << " " << l << endl;

    circumference += l;
  }

  return circumference;
} 

/* finis*/
