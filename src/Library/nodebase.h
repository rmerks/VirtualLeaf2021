/*
 * NodeBase.h
 *
 *  Created on: Jun 20, 2023
 *      Author: ritchie
 */

#ifndef NODEBASE_H_
#define NODEBASE_H_
#include "vector.h"
#include "wallelement.h"

class NodeBase : public Vector {
	  vector<WallElement *> wallElements;
	 public:
	NodeBase(void);

	NodeBase(int index);

	NodeBase(double x,double y, double z=0);

	NodeBase(const NodeBase &src);

	  explicit NodeBase(const Vector &src);



	  WallElement* getWallElement(CellBase *);

	  WallElement* insertWallElement(WallElement* we);
};

#endif /* NODEBASE_H_ */
