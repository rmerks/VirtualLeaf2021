
#ifndef CELLORIENTATION_H_
#define CELLORIENTATION_H_

#include "vector.h"

class CellOrientation {

public:
	bool initialized = false;

	Vector minimaStart;

	Vector divide25Start;
	Vector divide25End;

	Vector divide50Start;
	Vector divide50End;

	Vector divide75Start;
	Vector divide75End;

	Vector minimaEnd;
	CellOrientation();
	virtual ~CellOrientation();
};

#endif /* CELLORIENTATION_H_ */
