#include "triangle.h"
#include "display.h"
#include "texture.h"
#include <algorithm>


namespace C2 {

geom::vec3 barycentric_weights(geom::vec2 a, geom::vec2 b, geom::vec2 c, geom::vec2 p) {

    geom::vec2 ac = c - a;
    geom::vec2 ab = b - a;
    geom::vec2 pc = c - p;
    geom::vec2 pb = b - p;
    geom::vec2 ap = p - a;

    float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x); // || AC x AB ||
    if (std::abs(area_parallelogram_abc) < 1e-5f) {
        return { -1.f, -1.f, -1.f };
    }
    float alpha = (pc.x * pb.y - pb.x * pc.y) / area_parallelogram_abc;
    float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;
    float gamma = 1.0 - alpha - beta;

    geom::vec3 weights = { alpha, beta, gamma };
    return weights;
}

float edge_cross(const geom::vec2& a, const geom::vec2& b, const geom::vec2& p) {
    geom::vec2 ab = { b.x - a.x, b.y - a.y };
    geom::vec2 ap = { p.x - a.x, p.y - a.y };
    return (ab.x * ap.y) - (ab.y * ap.x);
}

void draw_filled_triangle(EngineCore& engine_core, 
        geom::vec4 p0,
        geom::vec4 p1,
        geom::vec4 p2,
        uint32_t color
) {
    int x_min = static_cast<int>(std::floor(std::min({p0.x, p1.x, p2.x})));
    int y_min = static_cast<int>(std::floor(std::min({p0.y, p1.y, p2.y})));
    int x_max = static_cast<int>(std::ceil(std::max({p0.x, p1.x, p2.x})));
    int y_max = static_cast<int>(std::ceil(std::max({p0.y, p1.y, p2.y})));  

    x_min = std::max(0, x_min);
    y_min = std::max(0, y_min);
    x_max = std::min(engine_core.window.window_width - 1, x_max);
    y_max = std::min(engine_core.window.window_height - 1, y_max);

    geom::vec2 v0 = p0.xy();
    geom::vec2 v1 = p1.xy();
    geom::vec2 v2 = p2.xy();

    float area = edge_cross(v0, v1, v2);

    if (area <= 0) {
        return;
    }

    float w0_step_x = v1.y - v2.y;
    float w1_step_x = v2.y - v0.y;
    float w2_step_x = v0.y - v1.y;

    float w0_step_y = v2.x - v1.x;
    float w1_step_y = v0.x - v2.x;
    float w2_step_y = v1.x - v0.x;

    geom::vec2 p_start = { x_min + 0.5f, y_min + 0.5f };
    float w0_row = edge_cross(v1, v2, p_start);
    float w1_row = edge_cross(v2, v0, p_start);
    float w2_row = edge_cross(v0, v1, p_start);

    for (int y = y_min; y <= y_max; y++) {
        float w0 = w0_row;
        float w1 = w1_row;
        float w2 = w2_row;

        for (int x = x_min; x <= x_max; x++) {
            
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float alpha = w0 / area;
                float beta = w1 / area;
                float gamma = w2 / area;

                float interpolated_reciprocal_w = (1.0f / p0.w) * alpha + (1.0f / p1.w) * beta + (1.0f / p2.w) * gamma;
                interpolated_reciprocal_w = 1.0f - interpolated_reciprocal_w; 
                
                int buffer_index = engine_core.window.window_width * y + x;
                if (interpolated_reciprocal_w < engine_core.z_buffer[buffer_index]) {
                    draw_pixel(engine_core, x, y, color);
                    engine_core.z_buffer[buffer_index] = interpolated_reciprocal_w;
                }
            }

            w0 += w0_step_x;
            w1 += w1_step_x;
            w2 += w2_step_x;
        }

        w0_row += w0_step_y;
        w1_row += w1_step_y;
        w2_row += w2_step_y;
    }
}

void draw_textured_triangle(EngineCore& engine_core, 
        geom::vec4 p0, geom::vec2 uv0,
        geom::vec4 p1, geom::vec2 uv1,
        geom::vec4 p2, geom::vec2 uv2,
        const Texture& texture
) {
    uv0.y = 1.0f - uv0.y;
    uv1.y = 1.0f - uv1.y;
    uv2.y = 1.0f - uv2.y;

    int x_min = static_cast<int>(std::floor(std::min({p0.x, p1.x, p2.x})));
    int y_min = static_cast<int>(std::floor(std::min({p0.y, p1.y, p2.y})));
    int x_max = static_cast<int>(std::ceil(std::max({p0.x, p1.x, p2.x})));
    int y_max = static_cast<int>(std::ceil(std::max({p0.y, p1.y, p2.y})));

    x_min = std::max(0, x_min);
    y_min = std::max(0, y_min);
    x_max = std::min(engine_core.window.window_width - 1, x_max);
    y_max = std::min(engine_core.window.window_height - 1, y_max);

    geom::vec2 v0 = p0.xy();
    geom::vec2 v1 = p1.xy();
    geom::vec2 v2 = p2.xy();

    float area = edge_cross(v0, v1, v2);

    if (area <= 0) {
        return;
    }

    float w0_step_x = v1.y - v2.y;
    float w1_step_x = v2.y - v0.y;
    float w2_step_x = v0.y - v1.y;

    float w0_step_y = v2.x - v1.x;
    float w1_step_y = v0.x - v2.x;
    float w2_step_y = v1.x - v0.x;

    geom::vec2 p_start = { x_min + 0.5f, y_min + 0.5f };
    float w0_row = edge_cross(v1, v2, p_start);
    float w1_row = edge_cross(v2, v0, p_start);
    float w2_row = edge_cross(v0, v1, p_start);

    for (int y = y_min; y <= y_max; y++) {
        
        float w0 = w0_row;
        float w1 = w1_row;
        float w2 = w2_row;

        for (int x = x_min; x <= x_max; x++) {
            
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float alpha = w0 / area;
                float beta  = w1 / area;
                float gamma = w2 / area;

                float interpolated_reciprocal_w = (1.0f / p0.w) * alpha + (1.0f / p1.w) * beta + (1.0f / p2.w) * gamma;
                float z_depth = 1.0f - interpolated_reciprocal_w; 

                int buffer_index = engine_core.window.window_width * y + x;
                
                if (z_depth < engine_core.z_buffer[buffer_index]) {
                    
                    float interpolated_u = (uv0.x / p0.w) * alpha + (uv1.x / p1.w) * beta + (uv2.x / p2.w) * gamma;
                    float interpolated_v = (uv0.y / p0.w) * alpha + (uv1.y / p1.w) * beta + (uv2.y / p2.w) * gamma;

                    interpolated_u /= interpolated_reciprocal_w;
                    interpolated_v /= interpolated_reciprocal_w;

                    int tex_x = static_cast<int>(interpolated_u * texture.width) % texture.width;
                    int tex_y = static_cast<int>(interpolated_v * texture.height) % texture.height;
                    
                    if (tex_x < 0) tex_x += texture.width;
                    if (tex_y < 0) tex_y += texture.height;

                    uint32_t texel_color = texture.pixels[(texture.width * tex_y) + tex_x];
                    draw_pixel(engine_core, x, y, texel_color);
                    engine_core.z_buffer[buffer_index] = z_depth;
                }
            }
            
            w0 += w0_step_x;
            w1 += w1_step_x;
            w2 += w2_step_x;
        }
        
        w0_row += w0_step_y;
        w1_row += w1_step_y;
        w2_row += w2_step_y;
    }
}

} // C2
