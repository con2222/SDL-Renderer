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
float edge_cross(const geom::vec2& a, const geom::vec2& b, const geom::vec2& p);

void draw_filled_triangle(EngineCore& engine_core, 
        geom::vec4 p0, geom::vec4 p1, geom::vec4 p2,
        uint32_t color
);


void draw_textured_triangle(EngineCore& engine_core, 
        geom::vec4 p0, geom::vec2 uv0,
        geom::vec4 p1, geom::vec2 uv1,
        geom::vec4 p2, geom::vec2 uv2,
        const Texture& texture
);

} // C2

#endif
