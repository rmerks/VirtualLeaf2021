

#ifndef ORIENTATION_ONODE_H_
#define ORIENTATION_ONODE_H_

#include <string>

class ONode {
	double x,y;
	ONode* next;
	ONode* prev;
	double weigth;

	void overflowCheck();

public:
	void set(double x,double y);
	void set(ONode * other);
	ONode * nextNode(int count);
	ONode * prevNode(int count);
	double distance(double x,double y);
	double distance(ONode * other);
	double distance(ONode other);
	ONode* checkMinima(double riseWeigth,ONode ** minima);
	ONode();
	virtual ~ONode();
	void setToFractionAlong(ONode* from, ONode* to,double fraction);
	void link(ONode* prev, ONode* next);
	double calculate();
	int isDeeper(ONode* other);
	double countNextDistanceTo(ONode* other);
	double countPrevDistanceTo(ONode* other);
	void followNextUntil(double distance,ONode* target);
	void followPrevUntil(double distance,ONode* target);
	ONode nomalize(ONode* base);
	std::string print();
};

#endif /* ORIENTATION_ONODE_H_ */
