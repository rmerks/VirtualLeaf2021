/*
 * WallElement.cpp
 *
 *  Created on: Jun 2, 2023
 *      Author: ritchie
 */

#include "wallelement.h"

WallElement::WallElement() {
}

WallElement::~WallElement() {
}

WallElement::WallElement(WallElement* copy) {
	cell = copy->cell;
	stiffness = copy->stiffness;
}

WallElement::WallElement(CellBase* cell) {
	this->cell = cell;
}
