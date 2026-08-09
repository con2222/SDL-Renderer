#ifndef CLIPPING_H
#define CLIPPING_H

#include "Geometry.h"


constexpr int MAX_NUM_POLY_VERTICES = 100;
constexpr int MAX_NUM_POLY_TRIANGLES = 98; // max - 2

struct Triangle;

struct Plane {
    geom::vec3 point;
    geom::vec3 normal;
};

struct Frustum {
    Plane left;
    Plane right;
    Plane top;
    Plane bottom;
    Plane near;
    Plane far;
};

struct Polygon {
    geom::vec3 vertices[MAX_NUM_POLY_VERTICES];
    geom::vec2 texcoords[MAX_NUM_POLY_VERTICES];
    int num_vertices;
    uint32_t color;

    int texture_index = -1;
};

Frustum create_frustum(float fov_x, float fov_y, float z_near, float z_far);
Polygon create_polygon_from_triangle(geom::vec3 v0, geom::vec3 v1, geom::vec3 v2, geom::vec2 t0, geom::vec2 t1, geom::vec2 t2);
float float_lerp(float a, float b, float t);
void clip_polygon(Polygon& polygon, Frustum frustum);
void clip_polygon_againts_plain(Polygon& polygon, const Plane& plane, Frustum frustum);
void triangles_from_polygon(Polygon& polygon, Triangle triangles[], int& num_triangles);

#endif // CLIPPING_H
