#ifndef MESH_H
#define MESH_H

#include "triangle.h"
#include "Geometry.h"
#include <vector>

namespace C2 {

struct SceneData;

struct Mesh {
    std::vector<geom::vec3> vertices; // dynamic array of vertices
    std::vector<geom::vec2> tex_coords;
    std::vector<Face> faces; // dynamic array of faces
    geom::vec3 rotation; // rotation with x, y, and z values
    geom::vec3 scale = { 1.0, 1.0, 1.0 }; // scale with x, y and z values
    geom::vec3 translation; // translation with x, y and z values

    int texture_index = -1;
};


Mesh load_obj_file_data(const char* filename, 
        geom::vec3 translation = { 0.f, 0.f, 0.f }, 
        geom::vec3 rotation = { 0.f, 0.f, 0.f }, 
        geom::vec3 scale = { 1.f, 1.f, 1.f }
);

void load_entity(SceneData& scene, const char* obj_filename, const char* png_filename, geom::vec3 translation = { 0.f, 0.f, 0.f }, geom::vec3 rotation = {0.f, 0.f, 0.f}, geom::vec3 scale = { 1.f, 1.f, 1.f });

} // C2

#endif
