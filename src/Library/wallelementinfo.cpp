/*
 * WallElementInfo.cpp
 *
 *  Created on: Jun 2, 2023
 *      Author: ritchie
 */

#include "wallelementinfo.h"
#include "node.h"
#include "Neighbor.h"

WallElementInfo* newWallElementInfo(){
	return new WallElementInfo();
}

void deleteWallElementInfo( WallElementInfo* info){
	delete info;
}

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

void WallElementInfo::divide(WallElementInfo* other,double ratio){
	other->getWallElement()->setStiffness(wallElement->getStiffness());
	double length = wallElement->getBaseLength();
	other->getWallElement()->setBaseLength(length*ratio);
	wallElement->setBaseLength(length*(1.-ratio));
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

void WallElementInfo::updateFrom(WallElementInfo* original,double ratio) {
	double stiffness = original->stiffness();
    double base_length = original->stiffness();
    if (!isnan(stiffness)) {
    	this->getWallElement()->setStiffness(stiffness);
    }
    if (!isnan(base_length)) {
    	this->getWallElement()->setBaseLength(base_length*ratio);
    } else {
    	updateBaseLength();
    }
}

double WallElementInfo::getBaseLength() {return this->getWallElement()->getBaseLength();};
void WallElementInfo::updateBaseLength() {this->getWallElement()->setBaseLength(this->length/1.2);};
bool WallElementInfo::plasticStretch() {return this->length > 1.2*this->getWallElement()->getBaseLength();};
