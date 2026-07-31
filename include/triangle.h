#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Geometry.h"


struct Face {
    int a;
    int b;
    int c;
    uint32_t color;
};

struct Triangle {
    geom::vec2 points[3];
    uint32_t color;
    float avg_depth;
};


#endif
