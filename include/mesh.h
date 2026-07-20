#ifndef MESH_H
#define MESH_H

#include "mesh.h"
#include "triangle.h"
#include "Geometry.h"


#define N_MESH_VERTICES 8
extern geom::vec3 mesh_vertices[N_MESH_VERTICES];

#define N_MESH_FACES (6 * 2) // 6 cube faces, 2 triangles per face
extern Face mesh_faces[N_MESH_FACES];


#endif