/*
 * WallElementInfo.h
 *
 *  Created on: Jun 2, 2023
 *      Author: ritchie
 */

#ifndef WALLELEMENTINFO_H_
#define WALLELEMENTINFO_H_

#include "wallelement.h"
#include "wallbase.h"
#include "vector.h"

class WallElement;

class WallElementInfo {
	  WallElement* wallElement=NULL;

	  WallBase* partOfWall=NULL;

	  CellBase* cell=NULL;

	Vector* from=NULL;

	Vector* to=NULL;
public:
	inline void setWallElement(WallElement* elementInfo) { this->wallElement=wallElement; };
	inline void setCell(CellBase* cell) { this->cell=cell; };
	WallElementInfo();
	virtual ~WallElementInfo();
	WallElement* getWallElement();
	inline bool hasWallElement() {return wallElement!= NULL;};
	inline Vector* getFrom() {return from;};
	inline Vector* getTo() {return to;};
	inline bool isFrom(Vector * other) {return from==other;};
	inline bool isTo(Vector * other) {return to==other;};
	inline void setNodes(Vector* from,Vector* to) {this->from =from;this->to =to;};
	inline void write(){} ;
};

#endif /* WALLELEMENTINFO_H_ */
