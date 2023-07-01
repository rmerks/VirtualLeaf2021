
#ifndef ORIENTATION_OSHAPE_H_
#define ORIENTATION_OSHAPE_H_

#define NODE_COUNT 30
#include <string>

#include "ONode.h"
#include "CellOrientation.h"

class OShape {
	ONode nodes[NODE_COUNT];
	// progressDistance is from the last cell to this node.
	ONode progressNode;
	double progressDistance;
	int currentNode;
	double nodeDistance;

	ONode *minimas[2];
	ONode alongNext25;
	ONode alongNext50;
	ONode alongNext75;
	ONode alongPrev25;
	ONode alongPrev50;
	ONode alongPrev75;

	void addOneNode(double x,double y);
	bool addMinima(ONode** minimas,ONode*minima);
	void overflowCheck();
	double calculateThreashold();
	void calculateMinima(double threashold,ONode**minimas);
	void fillResultOrientation(CellOrientation *result);

public:
	OShape(double nodeDistance);
	virtual ~OShape();
	void addNode(double x,double y);
	CellOrientation finalise();
	ONode* get(int index);
	std::string print(bool printPicture);


};

#endif /* ORIENTATION_OSHAPE_H_ */
