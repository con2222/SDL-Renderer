#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "configurations.h"
#include "clipping.h"
#include "mesh.h"
#include "texture.h"
#include <vector>

namespace C2 {

struct SceneData {
    Camera camera;
    std::vector<Mesh> meshes;
    std::vector<Texture> textures;
    geom::mat4 view_matrix;

    geom::vec3 light_direction = { 0, 0, 1 };
};

struct RenderContext {
    std::vector<Triangle> triangles_to_render;
    geom::mat4 projection_matrix;
    Frustum frustum;
    RenderMethod render_method;
    CullMethod cull_method;

    float fov = 128;
    float z_near = 0.1f;
    float z_far = 100.f;
};

}; // C2

#endif // SCENE_H
