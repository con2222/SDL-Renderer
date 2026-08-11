#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_scancode.h"
#include "color.h"
#include "triangle.h"
#include "texture.h"
#include "pipeline.h"
#include "display.h"
#include "C2Core/c2_profiler.hpp"
#include <cstdint>
#include <algorithm>
#include "scene.h"


using namespace C2;


void process_input(bool& is_running, SceneData& scene, RenderContext& render_context, FrameData& frame_data) {
    SDL_Event event;
    
    float pitch, yaw;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) is_running = false;
                if (event.key.keysym.sym == SDLK_1) render_context.render_method = RenderMethod::RENDER_WIRE_VERTEX;
                if (event.key.keysym.sym == SDLK_2) render_context.render_method = RenderMethod::RENDER_WIRE;
                if (event.key.keysym.sym == SDLK_3) render_context.render_method = RenderMethod::RENDER_FILL_TRIANGLE;
                if (event.key.keysym.sym == SDLK_4) render_context.render_method = RenderMethod::RENDER_FILL_TRIANGLE_WIRE;
                if (event.key.keysym.sym == SDLK_5) render_context.render_method = RenderMethod::RENDER_TEXTURED;
                if (event.key.keysym.sym == SDLK_6) render_context.render_method = RenderMethod::RENDER_TEXTURED_WIRE;
                if (event.key.keysym.sym == SDLK_c) render_context.cull_method = CullMethod::CULL_BACKFACE;
                if (event.key.keysym.sym == SDLK_x) render_context.cull_method = CullMethod::CULL_NONE;
                break;
            case SDL_MOUSEMOTION:
                yaw = event.motion.xrel * scene.camera.sensitivity;
                pitch = event.motion.yrel * scene.camera.sensitivity;
                scene.camera.yaw += yaw * frame_data.delta_time;
                scene.camera.pitch += pitch * frame_data.delta_time;
                break;
            default:
                break;
        }
    }

    const uint8_t* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_W]) {
        scene.camera.forward_velocity = scene.camera.direction * scene.camera.speed * frame_data.delta_time;
        scene.camera.position = scene.camera.position + scene.camera.forward_velocity;
    }
    if (key_state[SDL_SCANCODE_S]) {
        scene.camera.forward_velocity = scene.camera.direction * scene.camera.speed * frame_data.delta_time;
        scene.camera.position = scene.camera.position - scene.camera.forward_velocity;
    }
    if (key_state[SDL_SCANCODE_A]) {
        geom::vec3 up = { 0.f, 1.f, 0.f };
        geom::vec3 right = geom::normalize(geom::cross(scene.camera.direction, up));
        geom::vec3 right_velocity = right * scene.camera.speed * frame_data.delta_time;
        scene.camera.position = scene.camera.position + right_velocity;
    }
    if (key_state[SDL_SCANCODE_D]) {
        geom::vec3 up = { 0.0f, 1.0f, 0.0f };
        geom::vec3 right = geom::normalize(geom::cross(scene.camera.direction, up));
        
        geom::vec3 right_velocity = right * scene.camera.speed * frame_data.delta_time;
        scene.camera.position = scene.camera.position - right_velocity;
    }
    if (key_state[SDL_SCANCODE_LEFT]) {
        scene.camera.yaw -= 1.5f * frame_data.delta_time;
    }
    if (key_state[SDL_SCANCODE_RIGHT]) {
        scene.camera.yaw += 1.5f * frame_data.delta_time;
    }
    if (key_state[SDL_SCANCODE_UP]) {
        scene.camera.pitch -= 1.5f * frame_data.delta_time;
    }
    if (key_state[SDL_SCANCODE_DOWN]) {
        scene.camera.pitch += 1.5f * frame_data.delta_time;
    }
    if (key_state[SDL_SCANCODE_SPACE]) {
        scene.camera.position.y += scene.camera.up_speed * frame_data.delta_time;
    }
    if (key_state[SDL_SCANCODE_RSHIFT] || key_state[SDL_SCANCODE_LSHIFT]) {
        scene.camera.position.y -= scene.camera.up_speed * frame_data.delta_time;
    }

    scene.camera.pitch = std::clamp(scene.camera.pitch, -1.55f, 1.55f);
}


void update(EngineCore& engine_core, SceneData& scene, RenderContext& render_context, FrameData& frame_data) {
    C2Core::Profiler::ScopedProfiler profiler("Update", C2Core::Profiler::TimeUnit::Nanoseconds);
    render_context.triangles_to_render.clear();

    uint64_t time_to_wait = frame_data.frame_target_time - (SDL_GetTicks64() - frame_data.previous_frame_time);
    if (time_to_wait > 0 && time_to_wait <= frame_data.frame_target_time) {
        SDL_Delay(time_to_wait);
    }
    
    frame_data.delta_time = (SDL_GetTicks64() - frame_data.previous_frame_time) / 1000.0;
    frame_data.previous_frame_time = SDL_GetTicks64();

    update_camera_and_view(scene, frame_data);

    std::vector<Polygon> all_visible_polygons;

    for (auto& mesh : scene.meshes) {
        std::vector<Polygon> mesh_polygons = process_and_cull_geometry(mesh, scene, render_context.cull_method);
        all_visible_polygons.insert(all_visible_polygons.end(), mesh_polygons.begin(), mesh_polygons.end());
    }

    std::vector<Triangle> clipped_triangles = clip_geometry(all_visible_polygons, render_context.frustum);
    project_geometry(clipped_triangles, render_context, engine_core);
}

void render(EngineCore& engine_core, SceneData& scene, RenderContext& context) {

    size_t triangles_count = context.triangles_to_render.size();

    for (int i = 0; i < triangles_count; i++) {
        Triangle triangle = context.triangles_to_render[i];

        if (context.render_method == RenderMethod::RENDER_FILL_TRIANGLE || context.render_method == RenderMethod::RENDER_FILL_TRIANGLE_WIRE) {
            draw_filled_triangle(engine_core, triangle.points[0], triangle.points[1], triangle.points[2], triangle.color);
        }
        
        if (context.render_method == RenderMethod::RENDER_TEXTURED || context.render_method == RenderMethod::RENDER_TEXTURED_WIRE) {
            if (triangle.texture_index >= 0 && triangle.texture_index < scene.textures.size()) {
                const Texture& current_texture = scene.textures[triangle.texture_index];
                
                draw_textured_triangle(engine_core, 
                        triangle.points[0], triangle.texcoords[0],
                        triangle.points[1], triangle.texcoords[1],
                        triangle.points[2], triangle.texcoords[2],
                        current_texture);
            }       
        }
        

        if (context.render_method == RenderMethod::RENDER_WIRE || context.render_method == RenderMethod::RENDER_WIRE_VERTEX || context.render_method == RenderMethod::RENDER_FILL_TRIANGLE_WIRE || context.render_method == RenderMethod::RENDER_TEXTURED_WIRE) {
            draw_triangle(engine_core,
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                C2::Color::Gold
            );
        }

        if (context.render_method == RenderMethod::RENDER_WIRE_VERTEX) {
            draw_rectangle(engine_core, triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, C2::Color::Red);
            draw_rectangle(engine_core, triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, C2::Color::Red);
            draw_rectangle(engine_core, triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, C2::Color::Red); 
        }
    }
    

    render_color_buffer(engine_core);
    clear_color_buffer(engine_core, 0xFF000000); 
    clear_z_buffer(engine_core);
    SDL_RenderPresent(engine_core.renderer);
}

void setup(EngineCore& engine_core, SceneData& scene, RenderContext& context) {
    context.triangles_to_render.reserve(15000);
    context.cull_method = CullMethod::CULL_BACKFACE;
    context.render_method = RenderMethod::RENDER_WIRE;
    
    engine_core.color_buffer = new uint32_t[engine_core.window.window_width * engine_core.window.window_height]; 
    engine_core.z_buffer = new float[engine_core.window.window_width * engine_core.window.window_height]; 
    engine_core.color_buffer_texture = SDL_CreateTexture(engine_core.renderer, SDL_PIXELFORMAT_RGBA32, 
        SDL_TEXTUREACCESS_STREAMING, engine_core.window.window_width, engine_core.window.window_height);

    float fov_y = M_PI / 3.0; // the same as 180 / 3, or 60 degrees
    

    float aspect_y = engine_core.window.window_height / static_cast<float>(engine_core.window.window_width);
    float aspect_x = engine_core.window.window_width / static_cast<float>(engine_core.window.window_height);

    float fov_x = std::atan(std::tan(fov_y / 2) * aspect_x) * 2;

    float znear = 0.1;
    float zfar = 100.0;
    context.projection_matrix = geom::mat4_make_perspective(fov_y, aspect_y, znear, zfar);
    context.frustum = create_frustum(fov_x, fov_y, znear, zfar);

    load_entity(scene, "assets/runway.obj", "assets/runway.png", {0, -1.5, +23}, {0, 0, 0}, {1, 1, 1});
    load_entity(scene, "./assets/f22.obj", "./assets/f22.png", {0, -1.3, +5}, {0, -M_PI/2, 0}, {1, 1, 1});
    load_entity(scene, "./assets/efa.obj", "./assets/efa.png", {-2, -1.3, +9},  {0, -M_PI/2, 0}, {1, 1, 1});
    load_entity(scene, "./assets/f117.obj", "./assets/f117.png",  {+2, -1.3, +9}, {0, -M_PI/2, 0}, {1, 1, 1});

    std::cout << "Vertices:" << scene.meshes[0].vertices.size() << " " << "Faces:" << scene.meshes[0].faces.size() << std::endl;
}

int main(int argc, char* argv[]) {

    EngineCore engine_core;
    SceneData scene_data;
    RenderContext render_context;
    FrameData frame_data;

    engine_core.is_running = init_window(engine_core);
    setup(engine_core, scene_data, render_context);

    while (engine_core.is_running) {
        process_input(engine_core.is_running, scene_data, render_context, frame_data);
        update(engine_core, scene_data, render_context, frame_data);
        render(engine_core, scene_data, render_context);
    }
    
    destroy_window(engine_core);

    return 0;
}
