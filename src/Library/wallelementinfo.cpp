/*
 * WallElementInfo.cpp
 *
 *  Created on: Jun 2, 2023
 *      Author: ritchie
 */

#include "wallelementinfo.h"
#include "node.h"

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
    other->setLength();
    other->setBaseLength();
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

