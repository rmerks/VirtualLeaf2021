
#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>

#include "cellbase.h"
#include "random.h"
#include "mesh.h"
#include "spring.h"
#include "pi.h"


Spring::Spring(Node* an1, Node* an2, CellBase* ac)
{
    n1=an1;
    n2=an2;
    c=ac;
}

// set spring stiffness according to orientation?

/* finis*/
