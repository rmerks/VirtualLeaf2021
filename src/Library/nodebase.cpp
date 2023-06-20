/*
 * NodeBase.cpp
 *
 *  Created on: Jun 20, 2023
 *      Author: ritchie
 */

#include "nodebase.h"

NodeBase::NodeBase(void) : Vector()
{
}

NodeBase::NodeBase(int ind) : Vector()
{
}

NodeBase::NodeBase(const Vector &src) : Vector(src)
{
}

NodeBase::NodeBase(double x,double y, double z) : Vector (x,y,z)
{
}

NodeBase::NodeBase(const NodeBase &src) : Vector(src)
{
}



WallElement* NodeBase::getWallElement(CellBase * c) {
	  for (vector<WallElement*>::iterator i=wallElements.begin(); i!=wallElements.end(); i++) {
		  if((*i)->isWallOfCell(c)) {
			  return (*i);
		  }
	  }
	  return NULL;
}

WallElement* NodeBase::insertWallElement(CellBase* cell) {
	WallElement * we = new WallElement(cell);
	wallElements.push_back(we);
	return we;
}
