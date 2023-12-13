

#ifndef _SPRINGS_H_
#define _SPRINGS_H_

#include <list>
#include <vector>
#include <iostream>
#include <QString>
#include <QDebug>

#include "vector.h"
#include "parameter.h"
#include "wall.h"
#include "warning.h"
#include "assert.h"


class Spring{

    friend class Cell;
    friend class CellBase;
    friend class Mesh;
    Node *n1;  // first node of the spring
    Node *n2;  // second node of the spring
    CellBase *c;


public:
    Spring(Node *n1, Node *n2, CellBase *c);
    Spring(const Spring& src)
        {

            n1 = src.n1;
            n2 = src.n2;
            c = src.c;

        }

    inline Node *N1(void) const { return n1; }
    inline Node *N2(void) const { return n2; }

};
#endif

/* finis*/
