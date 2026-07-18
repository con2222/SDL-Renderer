#include "display.h"
#include "Geometry.h"

using namespace C2Renderer;

geom::vec2 project(EngineCore& engine_core, geom::vec3 point);

const int NUM_POINTS = 9 * 9 * 9;
geom::vec3 cube_points[NUM_POINTS];
const float fov_factor = 64;
geom::vec3 camera_position { 0, 0, -2.f };


void process_input(bool& is_running) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                is_running = false;
            }
            break;
        default:
            break;
    }
}

void update() {

}

void render(EngineCore& engine_core) {
    // SDL_SetRenderDrawColor(engine_core.renderer, 255, 0, 0, 255);
    // SDL_RenderClear(engine_core.renderer);
    
    for (int i = 0; i < NUM_POINTS; i++) {
        geom::vec3 point = cube_points[i];
        point.z -= camera_position.z;
        geom::vec2 new_point = project(engine_core, point);
        draw_pixel(engine_core, new_point.x + engine_core.window.window_width/2, new_point.y + engine_core.window.window_height/2, 0xFFFFFF00);
    }
    render_color_buffer(engine_core);
    clear_color_buffer(engine_core, 0xFF000000); 
    SDL_RenderPresent(engine_core.renderer);
}

void setup(EngineCore& engine_core) {
    engine_core.color_buffer = new uint32_t[engine_core.window.window_width * engine_core.window.window_height]; 
    engine_core.color_buffer_texture = SDL_CreateTexture(engine_core.renderer, SDL_PIXELFORMAT_ARGB8888, 
        SDL_TEXTUREACCESS_STREAMING, engine_core.window.window_width, engine_core.window.window_height);
    
    int point_count = 0;
    for (float x = -1; x <= 1; x += 0.25) {
        for (float y = -1; y <= 1; y += 0.25) {
            for (float z = -1; z <= 1; z += 0.25) {
                cube_points[point_count++] = geom::vec3(x, y, z);
            }
        }
    }
}

geom::vec2 project(EngineCore& engine_core, geom::vec3 point) {
    return geom::vec2(point.x * fov_factor / point.z,
                  point.y * fov_factor / point.z);
}

int main(int argc, char* argv[]) {

    C2Renderer::EngineCore engine_core;

    engine_core.is_running = init_window(engine_core);
    setup(engine_core);

    while (engine_core.is_running) {
        process_input(engine_core.is_running);
        update();
        render(engine_core);
    }
    
    destroy_window(engine_core);

    return 0;
}