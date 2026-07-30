#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Geometry.h"

struct Face {
    int a;
    int b;
    int c;
};

struct Triangle {
    geom::vec2 points[3];
};


#endif
