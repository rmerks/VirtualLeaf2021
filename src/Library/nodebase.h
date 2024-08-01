
#ifndef NODEBASE_H_
#define NODEBASE_H_
#include "vector.h"
#include "wallelement.h"


class WallElement;
class CellBase;

class NodeBase : public Vector {
	  vector<WallElement *> wallElements;
	 public:
	NodeBase(void);

	NodeBase(int index);

	NodeBase(double x,double y, double z=0);

	NodeBase(const NodeBase &src);

	  explicit NodeBase(const Vector &src);

	  virtual ~NodeBase() ;
	  virtual void removeCell(CellBase * cell) ;

	  WallElement* getWallElement(CellBase *);

	  WallElement* insertWallElement(CellBase *);

	  virtual int Index(void);
	  virtual int countNeighbors(void);

	  inline bool BoundaryP(void) const { return boundary; }

	  inline void SetBoundary(void) { boundary = true; }

	  inline void UnsetBoundary(void) { boundary = false; }

	  inline void toggleBoundary(void) {
	    boundary = !boundary;
	  }

	  bool boundary; // true if node is at the edge of the leaf

};

#endif /* NODEBASE_H_ */
