
#include "ONode.h"
#include <cmath>
#include <iostream>
#include <algorithm>

ONode::ONode() {

}

ONode::~ONode() {

}

void ONode::set(double x, double y){
	this->x = x;
	this->y = y;
}

double ONode::distance(double x,double y){
	return sqrt((this->x - x)*(this->x - x) +(this->y - y)*(this->y - y));
}

double ONode::distance(ONode * other){
	return this->distance(other->x,other->y);
}

double ONode::distance(ONode other){
	return this->distance(other.x,other.y);
}

void ONode::setToFractionAlong(ONode* from, ONode* to,double fraction){
	this->x= from->x+(to->x - from->x)*fraction;
	this->y= from->y+(to->y - from->y)*fraction;
}

void ONode::set(ONode * other) {
	this->x=other->x;
	this->y=other->y;
}

ONode* ONode::nextNode(int count) {
	ONode* current = this->next;
	while (count >1) {
		current = current->next;
		count--;
	}
	return current;
}

ONode* ONode::prevNode(int count) {
	ONode* current = this->prev;
	while (count >1) {
		current = current->prev;
		count--;
	}
	return current;
}

void ONode::link(ONode *prev,ONode *next) {
	this->next=next;
	this->prev=prev;
}

double ONode::calculate() {
	ONode next3 = this->nextNode(3)->nomalize(this);
	ONode prev3 = this->prevNode(3)->nomalize(this);
	ONode next6 = this->nextNode(6)->nomalize(this);
	ONode prev6 = this->prevNode(6)->nomalize(this);
	double distance10p = next3.distance(&prev3);
	double distance20p = next6.distance(&prev6);
	return this->weigth= distance10p+distance20p;
}

ONode * ONode::checkMinima(double minimumRise, ONode ** minima) {
	ONode * nextDeep = this;
	while (nextDeep->weigth < this->weigth ) {
		nextDeep = nextDeep->nextNode(1);
		if (nextDeep==this) {
			return this;
		}
	}
	if (nextDeep->weigth < minimumRise) {
		*minima=nextDeep;
		ONode * nextRise = nextDeep;
		while (nextRise->weigth < minimumRise ) {
			if ((*minima)->weigth > nextRise->weigth) {
				*minima=nextRise;
			}
			nextRise = nextRise->nextNode(1);
			if (nextRise==this) {
				return this;
			}
		}
		return nextRise;
	}else {
		return nextDeep;
	}
}

int ONode::isDeeper(ONode * other) {
	return other == NULL || this->weigth < other->weigth;
}


double ONode::countNextDistanceTo(ONode* other){
	double count= 0;
	ONode * node = this;
	while (node!= other) {
		ONode * nextNode1 = node->nextNode(1);
		count+=node->distance(nextNode1);
		node=nextNode1;
	}
	return count;
}

double ONode::countPrevDistanceTo(ONode* other){
	double count= 0;
	ONode * node = this;
	while (node!= other) {
		ONode * prevNode1 = node->prevNode(1);
		count+=node->distance(prevNode1);
		node=prevNode1;
	}
	return count;
}

void ONode::followNextUntil(double distance,ONode* target){
	double count= distance;
	ONode * node = this;
	while (count>0) {
		ONode * nextNode1 = node->nextNode(1);
		count-=node->distance(nextNode1);
		node=nextNode1;
	}
	//now we went to far, roll back
	ONode* prev=node->prevNode(1);
	double distanceBetweenLastTwo = node->distance(prev);
	double shouldHaveGone = distanceBetweenLastTwo+count;
	target->setToFractionAlong(prev, node, shouldHaveGone/distanceBetweenLastTwo);
}

void ONode::followPrevUntil(double distance,ONode* target){
	double count= distance;
	ONode * node = this;
	while (count>0) {
		ONode * prevNode1 = node->prevNode(1);
		count-=node->distance(prevNode1);
		node=prevNode1;
	}
	//now we went to far, roll back
	ONode* next=node->nextNode(1);
	double distanceBetweenLastTwo = node->distance(next);
	double shouldHaveGone = distanceBetweenLastTwo+count;
	target->setToFractionAlong(next, node, shouldHaveGone/distanceBetweenLastTwo);
}

ONode ONode::nomalize(ONode* base){
	ONode result;
	double dx=this->x-base->x;
	double dy=this->y-base->y;
	double factor = 1.0/this->distance(base);
	result.x=dx*factor+base->x;
	result.y=dy*factor+base->y;
	return result;
}

std::string ONode::print(){
	std::string result = "(" + std::to_string(this->x) + " *" + std::to_string(this->y) + " )";
	std::replace(result.begin(), result.end(), ',', '.');
	std::replace(result.begin(), result.end(), '*', ',');
	return result ;
}

void ONode::copyCoord(Vector * target) {
	target->x= this->x;
	target->y= this->y;
}

