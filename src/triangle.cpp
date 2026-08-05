#include "triangle.h"
#include "color.h"
#include "display.h"

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
                // C2Renderer::draw_pixel(engine_core, x, y, 0xFF00FFFF);
                C2Renderer::draw_pixel(engine_core, x, y, (x % 2 == 0 && y % 2 == 0) ? C2::Color::DeepPink : C2::Color::Indigo);
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
                C2Renderer::draw_pixel(engine_core, x, y, (x % 2 == 0 && y % 2 == 0) ? C2::Color::DeepPink : C2::Color::Indigo);
            }
        }
    }
}
