#include "triangle.h"
#include "display.h"
#include "texture.h"

#include <utility>


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

void draw_filled_triangle(C2Renderer::EngineCore& engine_core, 
        int x0, int y0, float z0, float w0, 
        int x1, int y1, float z1, float w1, 
        int x2, int y2, float z2, float w2,
        uint32_t color
) {
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
        std::swap(z0, z1);
        std::swap(w0, w1);
    }
    if (y1 > y2) {
        std::swap(y1, y2);
        std::swap(x1, x2);
        std::swap(z1, z2);
        std::swap(w1, w2);
    }
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
            std::swap(z0, z1);
        std::swap(w0, w1);
    }

    geom::vec2 point_a = geom::vec2(x0, y0);
    geom::vec2 point_b = geom::vec2(x1, y1);
    geom::vec2 point_c = geom::vec2(x2, y2);

    float rec_w0 = 1.0f / w0;
    float rec_w1 = 1.0f / w1;
    float rec_w2 = 1.0f / w2;
 
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
                if (x < 0 || x >= engine_core.window.window_width || y < 0 || y >= engine_core.window.window_height) {
                    continue;
                }

                geom::vec2 point_p = geom::vec2(x, y);
                geom::vec3 weights = barycentric_weights(point_a, point_b, point_c, point_p);
                float alpha = weights[0];
                float beta = weights[1];
                float gamma = weights[2];
                float interpolated_reciprocal_w = rec_w0 * alpha + rec_w1 * beta + rec_w2 * gamma;
                interpolated_reciprocal_w = 1 - interpolated_reciprocal_w; 
                int buffer_index = engine_core.window.window_width * y + x;
                if (interpolated_reciprocal_w < engine_core.z_buffer[buffer_index]) {
                    C2Renderer::draw_pixel(engine_core, x, y, color);
                    engine_core.z_buffer[buffer_index] = interpolated_reciprocal_w;
                }
            }
        }
    }

    if (y2 - y1 != 0) {
        if (y2 - y1 != 0) inv_slope1 = (x2 - x1) / static_cast<float>(abs(y2 - y1));

        for (int y = y1; y < y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope1;
            int x_end = x0 + (y - y0) * inv_slope2;
            
            if (x_start > x_end) std::swap(x_start, x_end);

            for (int x = x_start; x < x_end; x++) {
                if (x < 0 || x >= engine_core.window.window_width || y < 0 || y >= engine_core.window.window_height) {
                    continue;
                }
                geom::vec2 point_p = geom::vec2(x, y);
                geom::vec3 weights = barycentric_weights(point_a, point_b, point_c, point_p);
                float alpha = weights[0];
                float beta = weights[1];
                float gamma = weights[2];
                float interpolated_reciprocal_w = rec_w0 * alpha + rec_w1 * beta + rec_w2 * gamma;
                interpolated_reciprocal_w = 1 - interpolated_reciprocal_w; 
                int buffer_index = engine_core.window.window_width * y + x;
                if (interpolated_reciprocal_w < engine_core.z_buffer[buffer_index]) {
                    C2Renderer::draw_pixel(engine_core, x, y, color);
                    engine_core.z_buffer[buffer_index] = interpolated_reciprocal_w;
                }
            }
        }
    }
}


void draw_textured_triangle(C2Renderer::EngineCore &engine_core, 
        int x0, int y0, float z0, float w0, float u0, float v0,
        int x1, int y1, float z1, float w1, float u1, float v1, 
        int x2, int y2, float z2, float w2, float u2, float v2,
        uint32_t* texture) {
    v0 = 1 - v0;
    v1 = 1 - v1;
    v2 = 1 - v2;


    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
        std::swap(z0, z1);
        std::swap(w0, w1);
        std::swap(u0, u1);
        std::swap(v0, v1);
    }
    if (y1 > y2) {
        std::swap(y1, y2);
        std::swap(x1, x2);
        std::swap(z1, z2);
        std::swap(w1, w2);
        std::swap(u1, u2);
        std::swap(v1, v2);
    }
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
        std::swap(z0, z1);
        std::swap(w0, w1);
        std::swap(u0, u1);
        std::swap(v0, v1);
    }

    // create vector points after we sort verices
    geom::vec4 point_a = geom::vec4(x0, y0, z0, w0);
    geom::vec4 point_b = geom::vec4(x1, y1, z1, w1);
    geom::vec4 point_c = geom::vec4(x2, y2, z2, w2);
    

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

void draw_texel(C2Renderer::EngineCore& engine_core, int x, int y, uint32_t* texture, geom::vec4 point_a, geom::vec4 point_b, geom::vec4 point_c, float u0, float v0, float u1, float v1, float u2, float v2) {
    
    if (x < 0 || x >= engine_core.window.window_width || y < 0 || y >= engine_core.window.window_height) {
        return;
    }

    geom::vec2 point_p = geom::vec2(x, y);
    geom::vec2 a = point_a.xy();
    geom::vec2 b = point_b.xy();
    geom::vec2 c = point_c.xy();

    geom::vec3 weights = barycentric_weights(a, b, c, point_p);
    float alpha = weights[0];
    float beta = weights[1];
    float gamma = weights[2];

    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w;

    // Perform the interpolate of all U/w and V/w values using barycentric weights and a factor of 1/w
    interpolated_u = (u0 / point_a.w) * alpha + (u1 / point_b.w) * beta + (u2 / point_c.w) * gamma;
    interpolated_v = (v0 / point_a.w) * alpha + (v1 / point_b.w) * beta + (v2 / point_c.w) * gamma;

    interpolated_reciprocal_w = (1/point_a.w) * alpha + (1/point_b.w) * beta + (1/point_c.w) * gamma;

    // Divide back both interpolated values
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    int tex_x = static_cast<int>(interpolated_u * texture_width) % texture_width;
    int tex_y = static_cast<int>(interpolated_v * texture_height) % texture_height;

    if (tex_x < 0) tex_x += texture_width;
    if (tex_y < 0) tex_y += texture_height; 

    // Adjust 1/w so the pixels that are closer to the camera have smaller values
    interpolated_reciprocal_w = 1.f - interpolated_reciprocal_w;
    
    // Only draw the pixel if the depth value is less than the one previously stored in the z_buffer
    if (interpolated_reciprocal_w < engine_core.z_buffer[engine_core.window.window_width * y + x]) {
        C2Renderer::draw_pixel(engine_core, x, y, texture[(texture_width * tex_y) + tex_x]);
        engine_core.z_buffer[engine_core.window.window_width * y + x] = interpolated_reciprocal_w;
    }
}
