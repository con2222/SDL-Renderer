#include "Geometry.h"
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_scancode.h"
#include "mesh.h"
#include "display.h"
#include "color.h"
#include "triangle.h"
#include "texture.h"
#include "clipping.h"
#include "scene.h"
#include "C2Core/c2_profiler.hpp"

#include <cstdint>
#include <vector>
#include <algorithm>


using namespace C2Renderer;

const char* OBJ_FILENAME = "assets/f117.obj";
const char* PNG_FILENAME = "assets/f117.png"; 

geom::vec2 project(EngineCore& engine_core, geom::vec3 point);


void draw_filled_triangle(EngineCore& engine_core, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_line_DDA(EngineCore& engine_core, int x0, int y0, int x1, int y1, uint32_t color);
void update_scene_matrices(SceneData& scene, FrameData& frame_data, Mesh& mesh);
std::vector<Polygon> process_and_cull_geometry(const Mesh& mesh, const SceneData& scene, CullMethod cull_method);

void load_entity(SceneData& scene, const char* obj_filename, const char* png_filename, geom::vec3 translation = { 0.f, 0.f, 0.f }, geom::vec3 rotation = {0.f, 0.f, 0.f}, geom::vec3 scale = { 1.f, 1.f, 1.f }) {
    Texture tex = load_png_texture_data(png_filename);
    int tex_index = scene.textures.size();
    scene.textures.push_back(tex);

    Mesh mesh = load_obj_file_data(obj_filename, translation, rotation, scale);
    mesh.texture_index = tex_index;
    
    scene.meshes.push_back(mesh);
}

uint32_t light_apply_intensity(uint32_t original_color, float percentage_factor) {
    uint32_t a = (original_color & 0xFF000000);
    uint32_t r = (original_color & 0x00FF0000) * percentage_factor;
    uint32_t g = (original_color & 0x0000FF00) * percentage_factor;
    uint32_t b = (original_color & 0x000000FF) * percentage_factor;
    uint32_t new_color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
    return new_color;
}

// DDA
void draw_line_DDA(EngineCore& engine_core, int x0, int y0, int x1, int y1, uint32_t color) {
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;

    int side_length = geom::abs(delta_x) > geom::abs(delta_y) ? geom::abs(delta_x) : geom::abs(delta_y);

    float x_inc = delta_x / static_cast<float>(side_length);
    float y_inc = delta_y / static_cast<float>(side_length);

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= side_length; i++) {
        draw_pixel(engine_core, geom::round_float_to_int(current_x), geom::round_float_to_int(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void draw_triangle(EngineCore& engine_core, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line_DDA(engine_core, x0, y0, x1, y1, color);
    draw_line_DDA(engine_core, x1, y1, x2, y2, color);
    draw_line_DDA(engine_core, x2, y2, x0, y0, color);

}

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

void update_camera_and_view(SceneData& scene, FrameData& frame_data) {
    geom::vec3 up = { 0, 1, 0 };
    geom::vec3 target = { 0, 0, 1 };
    geom::mat4 camera_yaw_rotation = geom::mat4_make_rotation_y(scene.camera.yaw);
    geom::mat4 camera_pitch_rotation = geom::mat4_make_rotation_x(scene.camera.pitch);
    scene.camera.direction = (camera_pitch_rotation * camera_yaw_rotation * geom::vec4_from_vec3(target)).xyz();
    target = scene.camera.position + scene.camera.direction;

    scene.view_matrix = look_at(scene.camera.position, target, up);
}

geom::mat4 make_world_matrix(const Mesh& mesh) {
    geom::mat4 scale_matrix = geom::mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    geom::mat4 translation_matrix = geom::mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    geom::mat4 rotation_matrix_x = geom::mat4_make_rotation_x(mesh.rotation.x);
    geom::mat4 rotation_matrix_y = geom::mat4_make_rotation_y(mesh.rotation.y);
    geom::mat4 rotation_matrix_z = geom::mat4_make_rotation_z(mesh.rotation.z);
    geom::mat4 rotation_matrix = rotation_matrix_z * rotation_matrix_y * rotation_matrix_x;

    return translation_matrix * rotation_matrix * scale_matrix;
}

std::vector<Polygon> process_and_cull_geometry(const Mesh& mesh, const SceneData& scene, CullMethod cull_method) {
    std::vector<Polygon> visible_polygon;
    geom::vec3 light_dir = geom::normalize(scene.light_direction * -1.0f);

    for (size_t i = 0; i < mesh.faces.size(); i++) {

        Face mesh_face = mesh.faces[i];

        geom::vec3 face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a];
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

        geom::vec2 face_textures[3];
        face_textures[0] = mesh.tex_coords[mesh_face.a_t];
        face_textures[1] = mesh.tex_coords[mesh_face.b_t];
        face_textures[2] = mesh.tex_coords[mesh_face.c_t];

        geom::vec4 transformed_vertices[3];

        // Apply transformations
        for (size_t j = 0; j < 3; j++) {
            geom::vec4 transformed_vertex = geom::vec4_from_vec3(face_vertices[j]);
            transformed_vertices[j] =  scene.view_matrix * make_world_matrix(mesh) * transformed_vertex;
        }

        if (cull_method == CullMethod::CULL_BACKFACE) {

            // Clockwise
            geom::vec3 vector_a = transformed_vertices[0].xyz(); /*   A   */
            geom::vec3 vector_b = transformed_vertices[1].xyz(); /*  / \  */
            geom::vec3 vector_c = transformed_vertices[2].xyz(); /* C---B */

            geom::vec3 vector_ab = vector_b - vector_a;
            geom::vec3 vector_ac = vector_c - vector_a;
            geom::vec3 normal = geom::cross(vector_ab, vector_ac); // left handed coordinate system
            
            geom::vec3 origin = { 0, 0, 0 };
            geom::vec3 camera_ray = origin - vector_a;
            float product = geom::dot(normal, camera_ray);
            if (product < 0.0f) {
                continue;
                }
        }

        // Test flat shade
        geom::vec3 vector_a = transformed_vertices[0].xyz(); /*   A   */
        geom::vec3 vector_b = transformed_vertices[1].xyz(); /*  / \  */
        geom::vec3 vector_c = transformed_vertices[2].xyz(); /* C---B */

        geom::vec3 normal_dir = geom::normalize(geom::cross(vector_b - vector_a, vector_c - vector_a));
        float product = geom::dot(normal_dir, light_dir);

        float ambient_light = 0.2f;
        float diffuse_light = std::max(0.f, product);
        float intensity = std::min(1.0f, ambient_light + diffuse_light);       
        mesh_face.color = light_apply_intensity(mesh_face.color, intensity);

        // Create a polygon from the original transformed triangle to be clipped
        Polygon polygon = create_polygon_from_triangle(
                transformed_vertices[0].xyz(), 
                transformed_vertices[1].xyz(), 
                transformed_vertices[2].xyz(),
                face_textures[0],
                face_textures[1],
                face_textures[2]
        );
        polygon.color = mesh_face.color;
        polygon.texture_index = mesh.texture_index;
        visible_polygon.push_back(polygon);
    }
    return visible_polygon;
}

std::vector<Triangle> clip_geometry(std::vector<Polygon>& polygons, const Frustum& frustum) {
    std::vector<Triangle> clipped_triangles;
    Triangle triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];

    for (auto& polygon : polygons) {
        clip_polygon(polygon, frustum);
        int num_triangles_after_clipping = 0;
        triangles_from_polygon(polygon, triangles_after_clipping, num_triangles_after_clipping);
        
        for (int i = 0; i < num_triangles_after_clipping; i++) {
            triangles_after_clipping[i].color = polygon.color;
            triangles_after_clipping[i].texture_index = polygon.texture_index;
            clipped_triangles.push_back(triangles_after_clipping[i]);
        }
    }
    return clipped_triangles;
}

void project_geometry(std::vector<Triangle>& clipped_triangles, RenderContext& render_context, EngineCore& engine_core) {
    for (auto& triangle : clipped_triangles) {
        geom::vec4 projected_points[3];

        float avg_depth = (triangle.points[0].z + triangle.points[1].z + triangle.points[2].z) / 3.0f;

        // Loop all three vertices to perform projection
        for (size_t j = 0; j < 3; j++) {
            projected_points[j] = geom::project(render_context.projection_matrix, triangle.points[j]); // NDC all cordinated (-1, 1)

            // 1. SCALE first (stretch -1..1 to -half_width..half_width)
            projected_points[j].x *= engine_core.window.window_width / 2.0f;
            projected_points[j].y *= engine_core.window.window_height / 2.0f;

            projected_points[j].y *= -1.f;

            // 2. SHIFT second (move the center from 0 to half_width)
            projected_points[j].x += engine_core.window.window_width / 2.0f;
            projected_points[j].y += engine_core.window.window_height / 2.0f;
        }

        Triangle triangle_to_render;
        triangle_to_render.points[0] = { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w };
        triangle_to_render.points[1] = { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w };
        triangle_to_render.points[2] = { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w };

        triangle_to_render.color = triangle.color;
        triangle_to_render.avg_depth = avg_depth;
        triangle_to_render.texcoords[0] = triangle.texcoords[0];
        triangle_to_render.texcoords[1] = triangle.texcoords[1];
        triangle_to_render.texcoords[2] = triangle.texcoords[2];
        triangle_to_render.texture_index = triangle.texture_index;
        render_context.triangles_to_render.push_back(triangle_to_render); 
    }
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
            draw_filled_triangle(engine_core,
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                triangle.color
            );
        }
        
        if (context.render_method == RenderMethod::RENDER_TEXTURED || context.render_method == RenderMethod::RENDER_TEXTURED_WIRE) {
            if (triangle.texture_index >= 0 && triangle.texture_index < scene.textures.size()) {
                const Texture& current_texture = scene.textures[triangle.texture_index];
                
                draw_textured_triangle(engine_core,
                   triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].x, triangle.texcoords[0].y,
                   triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].x, triangle.texcoords[1].y,
                   triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].x, triangle.texcoords[2].y,
                   current_texture
                );
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

        if (context.render_method == C2Renderer::RenderMethod::RENDER_WIRE_VERTEX) {
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

    C2Renderer::EngineCore engine_core;
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
