/**
 * At what angle should the walls collapse together? and what would be the properties?
 */


#include "cellwallcurve.h"

void CellWallCurve::set(CellWallCurve *other) {
	cell = other->cell;
	from = other->from;
	over = other->over;
	to = other->to;
	borderCase=other->borderCase;
	potential_slide_angle= other->potential_slide_angle;
}

	WallBase* CellWallCurve::findWallBetweenEndingAt(CellBase *&c1, CellBase *&c2, NodeBase *&c) {
		WallBase *wallBetween = NULL;
		WallBase **pwallBetween = &wallBetween;
		c1->LoopWalls([this, c1, c2, c, pwallBetween](auto wall) {
			if (isWallBetweenEndingAt(wall, c1, c2, c)) {
				// this wall now ends at b instead of c
				(*pwallBetween) = wall;
			}
		}
		);
		return wallBetween;
	}

	WallBase* CellWallCurve::otherWallEndingAt(CellBase *c3, NodeBase *&c, WallBase *&wallc2c3) {
		WallBase *wallc3ToC;
		WallBase **pwallc3ToC = &wallc3ToC;
		c3->LoopWalls([this, c, wallc2c3, pwallc3ToC](auto wall) {
			NodeBase * n1= (NodeBase *)wall->N1();
			NodeBase * n2= (NodeBase *)wall->N2();
			if ((n1->Index() == c->Index() || n2->Index() == c->Index()) && wall->Index() != wallc2c3->Index()) {
				(*pwallc3ToC) = wall;
			}
		}
		);
		return wallc3ToC;
	}

	void CellWallCurve::shift(NodeBase * node) {
		  if (from == NULL) {
			  from=node;
		  } else if (over == NULL) {
			  over=node;
		  } else if (to==NULL) {
			  to = node;
		  } else {
			  from=over;
			  over=to;
			  to=node;
		  }
	}

	bool CellWallCurve::checkBudEnd(NodeBase * node) {
		if (to == NULL||over == NULL||from == NULL) {
			return false;
		}
		bool isBudEnd = false;
		NodeBase *tFrom = from;
		NodeBase *tOver = over;
		NodeBase *tTo = node;
		if (potential_slide_angle*2 > (*tFrom-*tOver).Angle((*tTo-*tOver))) {
			tOver = to;
			tTo=node;
			if (potential_slide_angle*2 > (*tFrom-*tOver).Angle((*tTo-*tOver))) {
				isBudEnd =true;
			}
		}
		if (isBudEnd)
			attachToCell();
		return isBudEnd;
	}

	bool CellWallCurve::checkAngleInternal() {
		double angle = (*from-*over).Angle((*to-*over));
		return potential_slide_angle > angle;
	}

void CellWallCurve::attachToCell() {
	CellBase *toAdd = cell;
	if (toAdd != NULL && toAdd->Index() == -1) {
		toAdd = cellBehindLongerWall();
	}
	if (toAdd != NULL && toAdd->Index() == -1) {
		toAdd = cellBehindShorterWall();
	}
	if (toAdd != NULL) {
		toAdd->attachToCell(this);
	}
}

	bool CellWallCurve::checkAngle() {
		if (to!= NULL && checkAngleInternal()){
			attachToCell();
			return true;
		}
		return false;
	}

	void CellWallCurve::reset() {
		cell=NULL;
		from=NULL;
		over=NULL;
		to=NULL;
		borderCase=false;
	}

	int CellWallCurve::Index() {
		return over->Index();
	}

	void CellWallCurve::setCell(CellBase * aCell) {
		cell = aCell;
	}

	void CellWallCurve::setTo(NodeBase * node) {
		to = node;
	}

	NodeBase* CellWallCurve::getFrom()  {
		return from;
	}

	NodeBase* CellWallCurve::getOver() {
		return over;
	}

	NodeBase* CellWallCurve::getTo() {
		return to;
	}

	NodeBase* CellWallCurve::longerWall() {
		int fromNorm = (*from-*over).Norm();
		int toNorm = (*to-*over).Norm();
		if (fromNorm>toNorm) {
			return from;
		}else {
			return to;
		}
	}

	CellBase* CellWallCurve::cellBehindLongerWall() {
		NodeBase * lw = longerWall();
		return cellBehindWallToNode(lw);
	}

	CellBase* CellWallCurve::cellBehindShorterWall() {
		NodeBase * lw = longerWall();
		NodeBase * sw = lw == to? from : to;
		return cellBehindWallToNode(sw);
	}

	CellBase* CellWallCurve::cellBehindWallToNode(NodeBase * spikeEnd) {
		return this->cell->getOtherWallElementSide(spikeEnd,over);
	}

	int n1index(WallBase* w) {
		return ((NodeBase*)w->N1())->Index();
	}
	int n2index(WallBase* w) {
		return ((NodeBase*)w->N2())->Index();
	}
	/**
	 * Now we collapse the spike by letting the longer wall end at the end of the shorter wall.
	 * Assume a sharp spike in a wall from node a to c (a-c) to b (c-b). were a-c is the longer
	 * wall. We remove this spike if the angle between a-c and b-c gets to small. We do this
	 * by replacing a-c and with a new wall a-b.
	 */
	bool CellWallCurve::removeSpike() {
		if (cell==NULL||from==NULL||over==NULL||to==NULL) {
			//incomplete ignore call
			return false;
		}
		CellBase * c1 = cellBehindLongerWall();
		CellBase * c2 = cell;
		CellBase * c3 = cellBehindShorterWall();
		if (c1==NULL ||c3 == NULL) {
			cout << "can't find cells behind walls cell=" << cell->Index() << " node=" << over->Index();
			return false;
		}
		NodeBase * a = longerWall();
		NodeBase * b = a == to? from : to;
		NodeBase * c = over;
		if (c1->Index() == -1 ||c2->Index() == -1 ||c3->Index() == -1) {
			borderCase = true;
			cout << "border case\n";
		}
		if (c->countNeighbors() ==  2) {
			// TODO delete node, spike into the cell itself
			cout << " inner spike ";
			cout << " c2=" << c2->Index() ;
			c2->removeNode(c);
			c2->correctNeighbors();
			return true;
		}
		if (a==b) {
			cout << " merged node ";
		}
		//we go over the c2 walls, here is the spike
		if (c1 == NULL||c3 == NULL|| c1==c3) {
			// unknow case
			cout<< " not handled spike "<< (c1==c3?"eq ":"");
			if (c1!= NULL)
				cout << " c1=" << c1->Index() ;
			else
				cout << " c1=NULL";
			cout << " c2=" << c2->Index() ;
			if (c3!= NULL)
				cout << " c3=" << c3->Index() ;
			else
				cout << " c3=NULL";
			return false;
		}

 		if (c2->Index()==-1) {
			cout<< " boundary node spike ";
			cout << " c2=" << c2->Index() ;
		}
		WallBase * wallc1c2 = findWallBetweenEndingAt(c1, c2, c);
		WallBase * wallc2c3 = findWallBetweenEndingAt(c2,c3,c);
		//for the extending wall we need a existing wall between c1 and c3 that ends at c.
		WallBase * wallc1c3 = findWallBetweenEndingAt(c1,c3,c);

		if (wallc1c2 != NULL&&wallc2c3 != NULL) {
			WallBase * wallc1ToC = NULL;
			WallBase * wallc3ToC = NULL;
			if (wallc1c3 == NULL) {
				// more than 3 cells in point c, now c1 and c3 are separated by other cells.
				wallc1ToC = otherWallEndingAt(c1, c, wallc1c2);
				wallc3ToC = otherWallEndingAt(c3, c, wallc2c3);
				if (wallc1ToC == NULL|| wallc3ToC==NULL) {
					cout << " a=" << a->Index() << " b=" << b->Index()<< " c=" << c->Index()<<"\n";
					cout << " not handled case\n";
					return false;
				}
			}

			// we extend the existing wall from c to b.
			cout << " spike removed\n";
			cout << " a=" << a->Index() << " b=" << b->Index()<< " c=" << c->Index()<<"\n";
			cout << " c1=" << c1->Index() << " c2=" << c2->Index()<< " c3=" << c3->Index()<<"\n";

			c2->removeNode(c);
			// add b node to c1 after a or c depending who comes first
			c1->insertNodeAfterFirst(c,a,b);
			if (a->BoundaryP() && b->BoundaryP() && c->BoundaryP() ) {
				c->UnsetBoundary();
			}
			if (a->BoundaryP() && !b->BoundaryP() && c->BoundaryP() ) {
				b->SetBoundary();
			}
			if (wallc1c3 != NULL) {
				cout << " wallc1c3=" << n1index(wallc1c3) << "->" << n2index(wallc1c3) << " wallc1c2=" << n1index(wallc1c2) << "->" << n2index(wallc1c2) << " wallc2c3=" << n1index(wallc2c3) << "->" << n2index(wallc2c3)<<"\n";
				wallc1c3->replaceNode(c,b);
			} else {
				wallc1c3 = c1->newWall(c,b,c3);
				wallc1c3->CopyWallContents(*wallc1c2);
				wallc1c3->SetLength();
				c1->InsertWall(wallc1c3);
				c3->InsertWall(wallc1c3);
			}
			wallc1c2->replaceNode(c,b);
			wallc1c2->SetLength();
			wallc2c3->replaceNode(c,b);
			wallc2c3->SetLength();
			c1->correctNeighbors();
			c2->correctNeighbors();
			c3->correctNeighbors();
			return true;
		} else {
			// a new wall is nessesary
			cout << " a=" << a->Index() << " b=" << b->Index()<< " c=" << c->Index()<<"\n";
			if (wallc1c3!= NULL) {
				cout << " wallc1c3=" << n1index(wallc1c3) << "->" << n2index(wallc1c3)<<"\n";
			} else {
				cout << " wallc1c3=NULL\n";
			}
			if (wallc1c2!= NULL) {
				cout << " wallc1c2=" << n1index(wallc1c2) << "->" << n2index(wallc1c2)<<"\n";
			} else {
				cout << " wallc1c2=NULL\n";
			}
			if (wallc2c3!= NULL) {
				cout << " wallc2c3=" << n1index(wallc2c3) << "->" << n2index(wallc2c3)<<"\n";
			} else {
				cout << " wallc2c3=NULL\n";
			}
			cout << " new wall nessesary ";
			return false;
		}
	}

	bool CellWallCurve::isWallBetweenEndingAt(WallBase * wall, CellBase* cell1, CellBase* cell2, NodeBase * node) {
	NodeBase *n1 = (NodeBase *)wall->N1();
	NodeBase *n2 = (NodeBase *)wall->N2();
	return (n1->Index() == node->Index() || n2->Index() == node->Index()) &&
				((wall->C1()->Index() == cell1->Index() && wall->C2()->Index() == cell2->Index()) || (wall->C1()->Index() == cell2->Index() && wall->C2()->Index() == cell1->Index()));

	}

