#include "display.h"
#include "Geometry.h"

using namespace C2Renderer;

geom::vec2 project(EngineCore& engine_core, geom::vec3 point);

const int NUM_POINTS = 9 * 9 * 9;
geom::vec3 cube_points[NUM_POINTS];
geom::vec2 projected_points[NUM_POINTS];
const float fov_factor = 64;
geom::vec3 camera_position { 0, 0, -2.f };
geom::vec3 cube_rotation { 0.f, 0.f, 0.f };

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

void update(EngineCore& engine_core) {
    cube_rotation.y += 0.01;
    cube_rotation.x += 0.01;
    cube_rotation.z += 0.01;

    for (int i = 0; i < NUM_POINTS; i++) {
        geom::vec3 point = cube_points[i];

        geom::vec3 transformed_point = geom::vec3_rotate_y(point, cube_rotation.y);
        transformed_point = geom::vec3_rotate_x(transformed_point, cube_rotation.x);
        transformed_point = geom::vec3_rotate_z(transformed_point, cube_rotation.z);

        // Translate the points away from the camera
        transformed_point.z -= camera_position.z;

        projected_points[i] = project(engine_core, transformed_point);
    }
}

void render(EngineCore& engine_core) {
    for (int i = 0; i < NUM_POINTS; i++) {
        draw_pixel(engine_core, projected_points[i].x + engine_core.window.window_width/2, projected_points[i].y + engine_core.window.window_height/2, 0xFFFFFF00);
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
        update(engine_core);
        render(engine_core);
    }
    
    destroy_window(engine_core);

    return 0;
}