/*
 */

#ifndef WALLELEMENT_H_
#define WALLELEMENT_H_

#include <cmath>
#include "cellbase.h"

class CellBase;

class WallElement {
	CellBase * cell;
	double stiffness = std::nan("1");
public:
	WallElement();
	WallElement(WallElement* copy);
	WallElement(CellBase* cell);

	void setStiffness(double stiffness) {this->stiffness=stiffness;};

	double getStiffness() {return this->stiffness;};

	virtual ~WallElement();
	inline int isWallOfCell(CellBase* other) {
		return cell==other;
	}


};

#endif /* WALLELEMENT_H_ */
