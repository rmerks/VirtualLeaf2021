
#ifndef ORIENTATION_OSHAPE_H_
#define ORIENTATION_OSHAPE_H_

#define NODE_COUNT 30
#include <string>

#include "ONode.h"

class OShape {
	ONode nodes[NODE_COUNT];
	// progressDistance is from the last cell to this node.
	ONode progressNode;
	double progressDistance;
	int currentNode;
	double nodeDistance;
	void addOneNode(double x,double y);
	bool addMinima(ONode** minimas,ONode*minima);
	void overflowCheck();
	double calculateThreashold();

public:
	OShape(double nodeDistance);
	virtual ~OShape();
	void addNode(double x,double y);
	void finalise();
	ONode* get(int index);
	std::string print(bool printPicture);
};

#endif /* ORIENTATION_OSHAPE_H_ */
