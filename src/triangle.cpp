#include "triangle.h"
#include "color.h"
#include "display.h"
#include "texture.h"


geom::vec3 barycentric_weights(geom::vec2 a, geom::vec2 b, geom::vec2 c, geom::vec2 p) {

    geom::vec2 ac = c - a;
    geom::vec2 ab = b - a;
    geom::vec2 pc = c - p;
    geom::vec2 pb = b - p;
    geom::vec2 ap = p - a;

    float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x); // || AC x AB ||
    float alpha = (pc.x * pb.y - pb.x * pc.y) / area_parallelogram_abc;
    float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;
    float gamma = 1.0 - alpha - beta;

    geom::vec3 weights = { alpha, beta, gamma };
    return weights;
}


void draw_textured_triangle(C2Renderer::EngineCore &engine_core, int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, int x2, int y2, float u2, float v2, uint32_t* texture) {
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
        std::swap(u0, u1);
        std::swap(v0, v1);
    }
    if (y1 > y2) {
        std::swap(y1, y2);
        std::swap(x1, x2);
        std::swap(u1, u2);
        std::swap(v1, v2);
    }
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
        std::swap(u0, u1);
        std::swap(v0, v1);
    }

    // create vector points after we sort verices
    geom::vec2 point_a = geom::vec2(x0, y0);
    geom::vec2 point_b = geom::vec2(x1, y1);
    geom::vec2 point_c = geom::vec2(x2, y2);
    

    // render the upper part of triangle
    float inv_slope1 = 0;
    float inv_slope2 = 0;
    if (y1 - y0 != 0) inv_slope1 = (x1 - x0) / static_cast<float>(abs(y1 - y0));
    if (y2 - y0 != 0) inv_slope2 = (x2 - x0) / static_cast<float>(abs(y2 - y0));

    if (y1 - y0 != 0) {
        for (int y = y0; y < y1; y++) {
            int dy = y - y0;

            int x_start = x0 + dy * inv_slope1;
            int x_end = x0 + dy * inv_slope2;
        
            if (x_start > x_end) std::swap(x_start, x_end);

            for (int x = x_start; x < x_end; x++) {
                draw_texel(engine_core, x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2);
            }
        }
    }

    // render the bottom part of triangle
    if (y2 - y1 != 0) {
        if (y2 - y1 != 0) inv_slope1 = (x2 - x1) / static_cast<float>(abs(y2 - y1));

        for (int y = y1; y < y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope1;
            int x_end = x0 + (y - y0) * inv_slope2;
            
            if (x_start > x_end) std::swap(x_start, x_end);

            for (int x = x_start; x < x_end; x++) {
                draw_texel(engine_core, x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2);
            }
        }
    }
}

void draw_texel(C2Renderer::EngineCore& engine_core, int x, int y, uint32_t* texture, geom::vec2 point_a, geom::vec2 point_b, geom::vec2 point_c, float u0, float v0, float u1, float v1, float u2, float v2) {
    
    geom::vec2 point_p = geom::vec2(x, y);
    geom::vec3 weights = barycentric_weights(point_a, point_b, point_c, point_p);
    float alpha = weights[0];
    float beta = weights[1];
    float gamma = weights[2];

    float interpolated_u = u0 * alpha + u1 * beta + u2 * gamma;
    float interpolated_v = v0 * alpha + v1 * beta + v2 * gamma;

    int tex_x = abs(static_cast<int>(interpolated_u * texture_width));
    int tex_y = abs(static_cast<int>(interpolated_v * texture_height));

    C2Renderer::draw_pixel(engine_core, x, y, texture[(texture_width * tex_y) + tex_x]);
}
