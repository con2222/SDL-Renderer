#include "display.h"
#include "SDL.h"
#include "SDL_mouse.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include <iostream>


namespace C2Renderer {

bool init_window(EngineCore& engine_core) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        std::cerr << "SDL init error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    
    int fullscreen_width = display_mode.w;
    int fullscreen_height = display_mode.h;

    engine_core.window.window_width = fullscreen_width / 3;
    engine_core.window.window_height = fullscreen_height / 3;

    engine_core.window.SDL_window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        fullscreen_width, fullscreen_height, SDL_WINDOW_BORDERLESS);

    if (!engine_core.window.SDL_window) {
        std::cerr << "Error creating SDL window\n" << std::endl;
        return false;
    }

    engine_core.renderer = SDL_CreateRenderer(engine_core.window.SDL_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!engine_core.renderer) {
        std::cerr << "Error creating SDL renderer\n" << std::endl;
        return false;
    }

    SDL_SetWindowFullscreen(engine_core.window.SDL_window, SDL_WINDOW_FULLSCREEN);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    return true;
}

void render_color_buffer(EngineCore& engine_core) {
    SDL_UpdateTexture(engine_core.color_buffer_texture, nullptr, engine_core.color_buffer, engine_core.window.window_width * sizeof(uint32_t));

    SDL_RenderCopy(engine_core.renderer, engine_core.color_buffer_texture, nullptr, nullptr);
}

void clear_color_buffer(EngineCore& engine_core, uint32_t color) {
    for (int i = 0; i < engine_core.window.window_width * engine_core.window.window_height; i++) {
        engine_core.color_buffer[i] = color;
    }
}

void clear_z_buffer(EngineCore& engine_core) {
    for (int i = 0; i < engine_core.window.window_width * engine_core.window.window_height; i++) {
        engine_core.z_buffer[i] = 1.f;
    }
}

void draw_grid(EngineCore& engine_core, uint32_t color, int height, int width, int offset) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x % offset == 0 || y % offset == 0) {
                engine_core.color_buffer[y * engine_core.window.window_width + x] = color;
            }
        }
    }
}

void draw_pixel(EngineCore& engine_core, int x, int y, uint32_t color) {
    if (x < 0 || x >= engine_core.window.window_width || y < 0 || y >= engine_core.window.window_height) return;
    engine_core.color_buffer[engine_core.window.window_width * y + x] = color;
}

void draw_rectangle(EngineCore& engine_core, int x, int y, int width, int height, uint32_t color) {
    for (int y0 = y; y0 < y + height; y0++) {
        for (int x0 = x; x0 < x + width; x0++) {
            draw_pixel(engine_core, x0, y0, color);
        }
    }
}

void draw_dot_grid(EngineCore& engine_core, uint32_t color, int offset) {
    for (int y = 0; y < engine_core.window.window_height; y += offset) {
        for (int x = 0; x < engine_core.window.window_width; x += offset) {
            engine_core.color_buffer[y * engine_core.window.window_width + x] = color;
        }
    }
}

void destroy_window(EngineCore& engine_core) {
    delete[] engine_core.color_buffer;
    delete[] engine_core.z_buffer;
    SDL_DestroyRenderer(engine_core.renderer);
    SDL_DestroyWindow(engine_core.window.SDL_window);
    SDL_Quit();
}

}; // namespace C2Renderer
