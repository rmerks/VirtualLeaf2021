#ifndef _HULL_H_
#define _HULL_H_

// Class point needed by 2D convex hull code
class Point {

public:
  Point(float xx, float yy) {
    x=xx;
    y=yy;
  }
  Point(void) {
    x=0; y=0;
  }
  float x,y;


};

// required to sort points (e.g. Qt's qSort())
bool operator<(const Point & p1, const Point & p2);

int chainHull_2D( Point* P, int n, Point* H );
#endif
