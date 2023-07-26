/*
 * WallElementInfo.cpp
 *
 *  Created on: Jun 2, 2023
 *      Author: ritchie
 */

#include "wallelementinfo.h"
#include "node.h"
#include "Neighbor.h"

WallElementInfo::WallElementInfo() {
	// TODO Auto-generated constructor stub

}

WallElementInfo::~WallElementInfo() {
	// TODO Auto-generated destructor stub
}

WallElement* WallElementInfo::getWallElement(){
	if (wallElement == NULL) {
		 wallElement= ((Node*)this->from)->getWallElement(this->cell);
	}
	if (wallElement == NULL) {
		wallElement= ((Node*)this->from)->insertWallElement(cell);
	}
	return wallElement;
} ;

void WallElementInfo::divide(WallElementInfo* other){
	other->getWallElement()->setStiffness(wallElement->getStiffness());
	double length = wallElement->getBaseLength();
	other->getWallElement()->setBaseLength(length/2.);
	wallElement->setBaseLength(length/2.);
    other->setLength();
    setLength();
}

double WallElementInfo::stiffness(){
	double stiffness = std::nan("1");
	if (hasWallElement()){
		stiffness = getWallElement()->getStiffness();
	}
	if (std::isnan(stiffness)){
		stiffness = getCell()->GetWallStiffness();
	}
	return  stiffness;
}

double WallElementInfo::calcLength() const{
    double length = 0;
    Vector *from = this->from;
    Vector *to = this->to;
    Vector edge = to - from;
    length = sqrt(DSQR(to->x-from->x)+DSQR(to->y-from->y));
    return length;
}


bool WallElementInfo::hasCounterWall(WallElementInfo * other) {
	//find the wall on the connected cell, must be in the other direction along the same nodes
	bool found = false;
	bool* pfound = &found;
	for (list<Neighbor>::iterator c=((Node*)from)->owners.begin(); c!=((Node*)from)->owners.end(); c++) {
         c->getCell()->LoopWallElements([this,other,pfound](auto wallElementInfo){
			 bool toIsFrom = to == wallElementInfo->from;
			 bool fromIsTo = from == wallElementInfo->to;
			 if (toIsFrom && fromIsTo) {
				 wallElementInfo->getCell()->fillWallElementInfo(other, ((Node*)to), ((Node*)from));
				 wallElementInfo->stopLoop();
				 *pfound=true;
			 }else  if (toIsFrom || fromIsTo) {
				 // true case can not happen any more
				 wallElementInfo->stopLoop();
			 }
		 });
		 if (found) {
			 return true;
		 }
	}
	return false;
}

double WallElementInfo::getBaseLength() {return this->getWallElement()->getBaseLength();};
void WallElementInfo::updateBaseLength() {this->getWallElement()->setBaseLength(this->length/1.2);};
bool WallElementInfo::plasticStretch() {return this->length > 1.2*this->getWallElement()->getBaseLength();};
