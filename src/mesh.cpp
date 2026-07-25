#include "mesh.h"

// TODO: Create implementation for mesh.h functions

Mesh mesh = {};

geom::vec3 cube_vertices[N_CUBE_VERTICES] = {
    { -1, -1, -1},
    { -1, 1, -1 },
    {  1, 1, -1 },
    { 1, -1, -1 },    
    { 1,  1,  1 },
    { 1, -1,  1 },
    { -1, 1,  1 },
    { -1, -1, 1 },
};

Face cube_faces[N_CUBE_FACES] = {
    // front
    {1, 2, 3},
    {1, 3, 4},
    // right
    {4, 3, 5},
    {4, 5, 6},
    // back
    {6, 5, 7},
    {6, 7, 8},
    // left
    {8, 7, 2},
    {8, 2, 1},
    // top
    {2, 7, 5},
    {2, 5, 3},
    // bottom
    {6, 8, 1},
    {6, 1, 4}
};

void load_cube_mesh_data() {
    for (int i = 0; i < N_CUBE_VERTICES; i++) {
        mesh.vertices.push_back(cube_vertices[i]);
    }

    for (int i = 0; i < N_CUBE_FACES; i++) {
        mesh.faces.push_back(cube_faces[i]);
    }
}


