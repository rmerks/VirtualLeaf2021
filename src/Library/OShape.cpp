
#include "OShape.h"

#include <cmath>
#include <iostream>


#define NODE_COUNT_DBL ((double)NODE_COUNT)
#define DISTANCE_TOLERANCE 0.01
#define DISTANCE_TOLERANCE_MUL_UP (1.0+DISTANCE_TOLERANCE)
#define DISTANCE_TOLERANCE_MUL_DOWN (1.0-DISTANCE_TOLERANCE)


OShape::OShape(double circumverense) {
	this->currentNode=0;
	this->nodeDistance=circumverense/NODE_COUNT_DBL;
	this->progressDistance=0;
	for (int index = 0;index < NODE_COUNT;index++) {
		this->nodes[index].set(0.0,0.0);
		this->nodes[index].link(this->get(index-1),this->get(index+1));
	}
}

OShape::~OShape() {
}

void OShape::addNode(double x,double y){
	this->addOneNode(x, y);
	while (this->progressDistance>this->nodeDistance && currentNode < NODE_COUNT) {
		this->addOneNode(x, y);
	}
}

void OShape::addOneNode(double x,double y){
	if (currentNode==0) {
		this->nodes[currentNode++].set(x,y);
		this->progressNode.set(x,y);
	}else {
		ONode node;
		node.set(x,y);
		if (this->progressDistance> (this->nodeDistance*DISTANCE_TOLERANCE_MUL_UP)) {
			// nimm nächstes teilstück von progress
			// this->nodes[currentNode-1] nach this->progressNode
			double factor = this->nodeDistance/this->progressDistance;
			this->nodes[currentNode].setToFractionAlong(&(this->nodes[currentNode-1]),&(this->progressNode),factor);
			this->progressDistance -= this->nodeDistance;
			this->currentNode++;
		} else if (this->progressDistance > (this->nodeDistance*DISTANCE_TOLERANCE_MUL_DOWN)) {
				//nimm einfach das nächste stück als ganze.
				this->progressDistance = this->progressNode.distance(x,y);
				this->nodes[currentNode].set(&(this->progressNode));
				this->progressNode.set(x,y);
				this->currentNode++;
		} else {
			double includeNext =this->progressDistance + this->progressNode.distance(x,y);
			if (includeNext > (this->nodeDistance*DISTANCE_TOLERANCE_MUL_UP)) {
		    	// there will be a progress left. just set the next node along
				// this->progressNode to x,y
				double factor = (this->nodeDistance-this->progressDistance)/(this->progressNode.distance(x,y));
				this->nodes[currentNode].setToFractionAlong(&(this->progressNode),&node,factor);
				this->progressDistance = this->nodes[currentNode].distance(x,y);
				this->progressNode.set(x,y);
				this->currentNode++;
		    } else if (includeNext > (this->nodeDistance*DISTANCE_TOLERANCE_MUL_DOWN)) {
		    	// x,y wird das nächste element und progressDistance 0
				this->nodes[currentNode].set(x,y);
				//we skip the progress Node so reset it.
				this->progressDistance = 0.0;
				this->progressNode.set(x,y);
				this->currentNode++;
		    } else {
		    	// nicht genug, progress distance erhöhen und progress node auf x,y
		    	this->progressDistance += this->progressNode.distance(x,y);
		    	this->progressNode.set(x,y);
		    }
		}
	}
}

std::string OShape::print(bool printPicture){
	std::string info_string = "";
	if (printPicture) {
		info_string += "\\begin{tikzpicture}\n";
	}
	int lastIndex = NODE_COUNT - 1;
	for (int index = 0;index < NODE_COUNT;index++) {
		info_string += "\\draw[gray, thick, <->]" + this->nodes[lastIndex].print() + " -- " + this->nodes[index].print() + ";\n";
		lastIndex = index;
	}
	if (printPicture) {
		info_string += "\\end{tikzpicture}\n";
	}
	return info_string;
}

double OShape::calculateThreashold() {
	double minV = 4000000.0, maxV = 0.0;
	for (int index = 0; index < NODE_COUNT; index++) {
		double weigth = this->nodes[index].calculate();
		minV = fmin(minV, weigth);
		maxV = fmax(maxV, weigth);
	}
	// max distance = 4 minimum = 0 no minimas are calculated if there is no difference of at least 1.0
	double threashold = -1.0;
	if ((maxV - minV) > 1.0) {
		threashold = (maxV + minV) / 2.0;
	}
	std::cout << "%Form max " << maxV << " min " << minV << " Threashold " << threashold << std::endl;
	return threashold;
}

void OShape::finalise(){
	if (this->currentNode < NODE_COUNT && this->progressDistance > 0.0) {
		this->nodes[this->currentNode].set(&(this->progressNode));
		this->progressNode.set(0.0,0.0);
		this->progressDistance = 0.0;
		this->currentNode++;
	}
	while (this->currentNode < NODE_COUNT) {
		this->nodes[this->currentNode].setToFractionAlong(&(this->nodes[0]), &(this->nodes[this->currentNode-1]), 0.5);
		this->currentNode++;
	}
	double threashold = calculateThreashold();
	if (threashold > 0.0) {
		ONode* minimas[2]={NULL,NULL};
		ONode* step = this->nodes;
		while (step->nextNode(1) !=  this->nodes) {
			ONode* minima = NULL;
			step = step
					->checkMinima(threashold,&minima)
					->nextNode(1);
			if (minima != NULL) {
				if (addMinima(minimas, minima)){
					break;
				}
			}
		}
		if (minimas[0]!= NULL && minimas[1]!= NULL) {
			// TODO better minima if parabolic match for middle 3 points around the minima.
			double nextDistance = minimas[0]->countNextDistanceTo(minimas[1]);
			double prevDistance = minimas[0]->countPrevDistanceTo(minimas[1]);
			ONode alongNext25;
			minimas[0]->followNextUntil(nextDistance*.25,&alongNext25);
			ONode alongNext50;
			minimas[0]->followNextUntil(nextDistance*.50,&alongNext50);
			ONode alongNext75;
			minimas[0]->followNextUntil(nextDistance*.75,&alongNext75);
			ONode alongPrev25;
			minimas[0]->followPrevUntil(prevDistance*.25,&alongPrev25);
			ONode alongPrev50;
			minimas[0]->followPrevUntil(prevDistance*.50,&alongPrev50);
			ONode alongPrev75;
			minimas[0]->followPrevUntil(prevDistance*.75,&alongPrev75);



			std::cout << "\\begin{tikzpicture}\n" << std::endl;
			std::cout << print(false) << std::endl;

			std::string  info_string = "\\filldraw[black] " + minimas[0]->print() + " circle (3pt) node[anchor=west]{1};\n";
			info_string += "\\filldraw[black] " + minimas[1]->print() + " circle (3pt) node[anchor=west]{2};\n";
			info_string += "\\draw[gray, thick, <->]" + alongNext25.print() + " -- " + alongPrev25.print() + ";\n";
			info_string += "\\draw[gray, thick, <->]" + alongNext50.print() + " -- " + alongPrev50.print() + ";\n";
			info_string += "\\draw[gray, thick, <->]" + alongNext75.print() + " -- " + alongPrev75.print() + ";\n";
			std::cout << info_string << std::endl;

			std::cout << "\\end{tikzpicture}\n" << std::endl;

		}else {
			std::cout << print(true) << std::endl;
		}
	}
}

bool OShape::addMinima(ONode** minimas,ONode*minima) {
	if (minima != NULL) {
		if (minimas[0]==minima||minimas[1]==minima) {
			return true;
		}else if (minimas[0]==NULL) {
			minimas[0]=minima;
		}	else	if (minimas[1]==NULL) {
			minimas[1]=minima;
		} else if (minima->isDeeper(minimas[0])) {
			if (minimas[0]->isDeeper(minimas[1])) {
				minimas[1]=minimas[0];
			}
			minimas[0]=minima;
		}else if (minima->isDeeper(minimas[1])) {
			if (minimas[1]->isDeeper(minimas[0])) {
				minimas[0]=minimas[1];
			}
			minimas[1]=minima;
		}
	}
	return false;
}

ONode* OShape::get(int index){
	int protectedIndex=index;
	while (protectedIndex <0){
		protectedIndex+=NODE_COUNT;
	}
	while (protectedIndex >=NODE_COUNT){
		protectedIndex-=NODE_COUNT;
	}
	return &(this->nodes[protectedIndex]);
}

