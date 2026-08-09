// include/Scene.h
#pragma once
#include "camera.h"
#include "mesh.h"
#include "clipping.h"
#include <vector>

struct Scene {
    Camera camera;
    std::vector<Mesh> meshes;
    std::vector<Texture> textures;
    Frustum frustum;
    geom::vec3 light_direction = { 0, 0, 1 };
};