#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Geometry.h"
#include "texture.h"

namespace C2Renderer {
    struct EngineCore;
}


struct Face {
    int a;
    int b;
    int c;
    Texel a_uv;
    Texel b_uv;
    Texel c_uv;
    uint32_t color;
};

struct Triangle {
    geom::vec2 points[3];
    Texel texcoords[3];
    uint32_t color;
    float avg_depth;
};

geom::vec3 barycentric_weights(geom::vec2 a, geom::vec2 b, geom::vec2 c, geom::vec2 p);

void draw_texel(C2Renderer::EngineCore& engine_core, int x, int y, uint32_t* texture, geom::vec2 point_a, geom::vec2 point_b, geom::vec2 point_c, float u0, float v0, float u1, float v1, float u2, float v2);

void draw_textured_triangle(C2Renderer::EngineCore& engine_core, int x0, int y0, float u0, float v0, 
        int x1, int y1, float u1, float v1, 
        int x2, int y2, float u2, float v2,
        uint32_t* texture
    );

#endif
