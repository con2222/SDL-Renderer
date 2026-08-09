#ifndef MESH_H
#define MESH_H

#include "triangle.h"
#include "Geometry.h"

#include <vector>


const int N_CUBE_VERTICES = 8;
const int N_CUBE_FACES = 12;

extern geom::vec3 cube_vertices[N_CUBE_VERTICES];
extern Face cube_faces[N_CUBE_FACES];

struct Mesh {
    std::vector<geom::vec3> vertices; // dynamic array of vertices
    std::vector<geom::vec2> tex_coords;
    std::vector<Face> faces; // dynamic array of faces
    geom::vec3 rotation; // rotation with x, y, and z values
    geom::vec3 scale = { 1.0, 1.0, 1.0 }; // scale with x, y and z values
    geom::vec3 translation; // translation with x, y and z values
};

extern Mesh mesh;

void load_obj_file_data(const char* filename);


#endif
