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
