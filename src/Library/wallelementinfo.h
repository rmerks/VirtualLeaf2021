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

      double length;

      bool stop = false;

public:
	inline void setWallElement(WallElement* wallElement) { this->wallElement=wallElement; };
	bool hasCounterWall(WallElementInfo * other) ;
	inline void setCell(CellBase* cell) { this->cell=cell; };
	WallElementInfo();
	virtual ~WallElementInfo();
	WallElement* getWallElement();
    inline bool hasWallElement() {return wallElement!= NULL;};
	inline Vector* getFrom() {return from;};
    inline Vector* getTo() {return to;};
	inline CellBase* getCell() {return cell;};
	inline bool isFrom(Vector * other) {return from==other;};
	inline bool isTo(Vector * other) {return to==other;};
	inline void stopLoop() {stop=true;};
	inline bool isStop() {return stop;};
	inline void setNodes(Vector* from,Vector* to) {this->from =from;this->to =to;};
	void divide(WallElementInfo * other, double ratio);
    double stiffness();

    double getBaseLength();
    double calcLength(void) const;
    double setLength() {return this->length=calcLength();};
    double getLength() {return this->length;};

    void updateBaseLength();
    void relax();
    bool plasticStretch();

};

#endif /* WALLELEMENTINFO_H_ */
