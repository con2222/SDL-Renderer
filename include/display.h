#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
#include <SDL2/SDL.h>

namespace C2Renderer {

struct Window {
    SDL_Window* SDL_window = nullptr;
    int window_width = 800;
    int window_height = 600;
};

struct EngineCore {
    Window window;
    SDL_Renderer* renderer = nullptr;
    bool is_running = false;
    uint32_t* color_buffer = nullptr;
    SDL_Texture* color_buffer_texture = nullptr;
};



bool init_window(EngineCore& engineCore);
void render_color_buffer(EngineCore& engine_core);
void clear_color_buffer(EngineCore& engine_core, uint32_t color);
void draw_grid(EngineCore& engine_core, uint32_t color, int height, int width, int offset);
void draw_pixel(EngineCore& engine_core, int x, int y, uint32_t color);
void draw_rectangle(EngineCore& engine_core, int x, int y, int width, int height, uint32_t color);
void draw_dot_grid(EngineCore& engine_core, uint32_t color, int offset);
void destroy_window(EngineCore& engine_core);

}; // namespace C2Renderer

#endif
