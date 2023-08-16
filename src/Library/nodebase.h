
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

	  virtual ~NodeBase() {
		  for (vector<WallElement*>::iterator i=wallElements.begin(); i!=wallElements.end(); i++) {
			  delete *i;
		  }
	  }

	  WallElement* getWallElement(CellBase *);

	  WallElement* insertWallElement(CellBase *);
};

#endif /* NODEBASE_H_ */
