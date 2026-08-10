#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Geometry.h"
#include "texture.h"

namespace C2 {

struct EngineCore;
struct Face {
    int a;
    int b;
    int c;

    int a_t;
    int b_t;
    int c_t;

    uint32_t color;
};

struct Triangle {
    geom::vec4 points[3];
    geom::vec2 texcoords[3];
    uint32_t color;
    float avg_depth;
    int texture_index = -1;
};

geom::vec3 barycentric_weights(geom::vec2 a, geom::vec2 b, geom::vec2 c, geom::vec2 p);
void draw_filled_triangle(EngineCore& engine_core, 
        int x0, int y0, float z0, float w0, 
        int x1, int y1, float z1, float w1, 
        int x2, int y2, float z2, float w2,
        uint32_t color
);

void draw_texel(EngineCore& engine_core, int x, int y, const Texture& texture, geom::vec4 point_a, geom::vec4 point_b, geom::vec4 point_c, float u0, float v0, float u1, float v1, float u2, float v2);

void draw_textured_triangle(EngineCore& engine_core, 
        int x0, int y0, float z0, float w0, float u0, float v0, 
        int x1, int y1, float z1, float w1, float u1, float v1, 
        int x2, int y2, float z2, float w2, float u2, float v2,
        const Texture& texture
    );

} // C2

#endif
