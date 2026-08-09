#include "Configurations.h"
#include "Geometry.h"
#include "SDL_keycode.h"
#include "camera.h"
#include "mesh.h"
#include "display.h"
#include "color.h"
#include "triangle.h"
#include "texture.h"
#include "clipping.h"

#include <cstdint>
#include <vector>
#include <algorithm>


using namespace C2Renderer;

const char* OBJ_FILENAME = "assets/f117.obj";
const char* PNG_FILENAME = "assets/f117.png"; 
C2Renderer::RenderMethod render_method;
C2Renderer::CullMethod cull_method;
geom::mat4 projection_matrix;
geom::mat4 world_matrix;
geom::mat4 view_matrix;

geom::vec2 project(EngineCore& engine_core, geom::vec3 point);

const int FPS = 60;
const double FRAME_TARGET_TIME = 1000.0 / FPS;
uint64_t previous_frame_time = 0;
const float fov_factor = 128;
geom::vec3 light_direction { 0, 0, 1 }; // like sun light, shine on object
float delta_time = 0;
extern Frustum frustum;

std::vector<Triangle> triangles_to_render;

void draw_filled_triangle(EngineCore& engine_core, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_line_DDA(EngineCore& engine_core, int x0, int y0, int x1, int y1, uint32_t color);


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

void process_input(bool& is_running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) is_running = false;
                if (event.key.keysym.sym == SDLK_1) render_method = RenderMethod::RENDER_WIRE_VERTEX;
                if (event.key.keysym.sym == SDLK_2) render_method = RenderMethod::RENDER_WIRE;
                if (event.key.keysym.sym == SDLK_3) render_method = RenderMethod::RENDER_FILL_TRIANGLE;
                if (event.key.keysym.sym == SDLK_4) render_method = RenderMethod::RENDER_FILL_TRIANGLE_WIRE;
                if (event.key.keysym.sym == SDLK_5) render_method = RenderMethod::RENDER_TEXTURED;
                if (event.key.keysym.sym == SDLK_6) render_method = RenderMethod::RENDER_TEXTURED_WIRE;
                if (event.key.keysym.sym == SDLK_c) cull_method = CullMethod::CULL_BACKFACE;
                if (event.key.keysym.sym == SDLK_x) cull_method = CullMethod::CULL_NONE;
                break;
            default:
                break;
        }
    }

    const uint8_t* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_W]) {
        camera.forward_velocity = camera.direction * camera.speed * delta_time;
        camera.position = camera.position + camera.forward_velocity;
    }
    if (key_state[SDL_SCANCODE_S]) {
        camera.forward_velocity = camera.direction * camera.speed * delta_time;
        camera.position = camera.position - camera.forward_velocity;
    }
    if (key_state[SDL_SCANCODE_A]) {
        camera.yaw -= 1.5f * delta_time;
    }
    if (key_state[SDL_SCANCODE_D]) {
        camera.yaw += 1.5f * delta_time;
    }
    if (key_state[SDL_SCANCODE_SPACE]) {
        camera.position.y += camera.up_speed * delta_time;
    }
    if (key_state[SDL_SCANCODE_RSHIFT] || key_state[SDL_SCANCODE_LSHIFT]) {
        camera.position.y -= camera.up_speed * delta_time;
    }
}

void update(EngineCore& engine_core) {
    triangles_to_render.clear();

    uint64_t time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks64() - previous_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    
    delta_time = (SDL_GetTicks64() - previous_frame_time) / 1000.0;
    previous_frame_time = SDL_GetTicks64();


    //mesh.rotation.x += 0.01;
    // mesh.rotation.y += 0.01;
    // mesh.rotation.z += 0.01;
    
    /*
    mesh.scale.x += 0.5;
    mesh.scale.y = 0.5;*/
    
    
    // mesh.translation.x += 0.01;
    mesh.translation.z = 5.0;

    geom::vec3 up = { 0, 1, 0 };
    geom::vec3 target = { 0, 0, 1 };
    geom::mat4 camera_yaw_rotation = geom::mat4_make_rotation_y(camera.yaw);
    camera.direction = (camera_yaw_rotation * geom::vec4_from_vec3(target)).xyz();
    target = camera.position + camera.direction;

    view_matrix = look_at(camera.position, target, up);

    geom::mat4 scale_matrix = geom::mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    geom::mat4 translation_matrix = geom::mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    geom::mat4 rotation_matrix_x = geom::mat4_make_rotation_x(mesh.rotation.x);
    geom::mat4 rotation_matrix_y = geom::mat4_make_rotation_y(mesh.rotation.y);
    geom::mat4 rotation_matrix_z = geom::mat4_make_rotation_z(mesh.rotation.z);
    geom::mat4 rotation_matrix = rotation_matrix_z * rotation_matrix_y * rotation_matrix_x;


    world_matrix = translation_matrix * rotation_matrix * scale_matrix;

    size_t num_faces = mesh.faces.size();
    size_t num_vertices = mesh.vertices.size();

    geom::vec3 light_dir = geom::normalize(light_direction * -1.0f);

    for (size_t i = 0; i < num_faces; i++) {

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
            transformed_vertices[j] =  view_matrix * world_matrix * transformed_vertex;
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

        
       // Take colors
        uint32_t a = (mesh_face.color >> 24) & 0xFF;
        uint32_t r = (mesh_face.color >> 16) & 0xFF;
        uint32_t g = (mesh_face.color >> 8)  & 0xFF;
        uint32_t b = mesh_face.color & 0xFF;

        r = (uint32_t)(r * intensity);
        g = (uint32_t)(g * intensity);
        b = (uint32_t)(b * intensity);

        uint32_t new_color = (a << 24) | (r << 16) | (g << 8) | b; 
        mesh_face.color = new_color;

        // Create a polygon from the original transformed triangle to be clipped
        Polygon polygon = create_polygon_from_triangle(
                transformed_vertices[0].xyz(), 
                transformed_vertices[1].xyz(), 
                transformed_vertices[2].xyz(),
                face_textures[0],
                face_textures[1],
                face_textures[2]
        );

        // Return new polygon with more vertices
        clip_polygon(polygon);
        
        // Break the clipped polygon apart back into individual triangles
        Triangle triangles_after_clipping[MAX_NUM_POLY_TRIANGLES];
        int num_triangles_after_clipping = 0;
        triangles_from_polygon(polygon, triangles_after_clipping, num_triangles_after_clipping); 

        for (int t = 0; t < num_triangles_after_clipping; t++) {
            Triangle triangle_after_clipping = triangles_after_clipping[t];

            geom::vec4 projected_points[3];

            // Loop all three vertices to perform projection
            for (size_t j = 0; j < 3; j++) {
                projected_points[j] = geom::project(projection_matrix, triangle_after_clipping.points[j]); // NDC all cordinated (-1, 1)

                // 1. SCALE first (stretch -1..1 to -half_width..half_width)
                projected_points[j].x *= engine_core.window.window_width / 2.0f;
                projected_points[j].y *= engine_core.window.window_height / 2.0f;

                projected_points[j].y *= -1.f;

                // 2. SHIFT second (move the center from 0 to half_width)
                projected_points[j].x += engine_core.window.window_width / 2.0f;
                projected_points[j].y += engine_core.window.window_height / 2.0f;
            }

            float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.f;

            Triangle triangle_to_render;
            triangle_to_render.points[0] = { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w };
            triangle_to_render.points[1] = { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w };
            triangle_to_render.points[2] = { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w };
            triangle_to_render.color = mesh_face.color;
            triangle_to_render.avg_depth = avg_depth;
            triangle_to_render.texcoords[0] = triangle_after_clipping.texcoords[0];
            triangle_to_render.texcoords[1] = triangle_after_clipping.texcoords[1];
            triangle_to_render.texcoords[2] = triangle_after_clipping.texcoords[2];

            triangles_to_render.push_back(triangle_to_render); 
        }
    }
}

void render(EngineCore& engine_core) {

    size_t triangles_count = triangles_to_render.size();

    for (int i = 0; i < triangles_count; i++) {
        Triangle triangle = triangles_to_render[i];

        if (render_method == RenderMethod::RENDER_FILL_TRIANGLE || render_method == RenderMethod::RENDER_FILL_TRIANGLE_WIRE) {
            draw_filled_triangle(engine_core,
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
                triangle.color
            );
        }
        
        if (render_method == RenderMethod::RENDER_TEXTURED || render_method == RenderMethod::RENDER_TEXTURED_WIRE) {
            draw_textured_triangle(engine_core,
               triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
               triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
               triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
               mesh_texture
            );
        }
        

        if (render_method == RenderMethod::RENDER_WIRE || render_method == RenderMethod::RENDER_WIRE_VERTEX || render_method == RenderMethod::RENDER_FILL_TRIANGLE_WIRE || render_method == RenderMethod::RENDER_TEXTURED_WIRE) {
            draw_triangle(engine_core,
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                C2::Color::Gold
            );
        }

        if (render_method == C2Renderer::RenderMethod::RENDER_WIRE_VERTEX) {
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

void setup(EngineCore& engine_core) {
    triangles_to_render.reserve(15000);
    cull_method = CullMethod::CULL_BACKFACE;
    render_method = RenderMethod::RENDER_WIRE;
    
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
    projection_matrix = geom::mat4_make_perspective(fov_y, aspect_y, znear, zfar);
    frustum = create_frustum(fov_x, fov_y, znear, zfar);

    // Load hard coded texture
    /* mesh_texture = (uint32_t*)REDBRICK_TEXTURE;
    texture_height = 64;
    texture_width = 64; */

    //load_cube_mesh_data();


    load_obj_file_data(OBJ_FILENAME);
    load_png_texture_data(PNG_FILENAME);
    std::cout << "Vertices:" << mesh.vertices.size() << " " << "Faces:" << mesh.faces.size() << std::endl;
}

geom::vec2 project(EngineCore& engine_core, geom::vec3 point) {
    if (point.z != 0) {
        return geom::vec2(point.x * fov_factor / point.z,
                  point.y * fov_factor / point.z);
    } 
    return geom::vec2();
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
    upng_free(png_texture);

    return 0;
}
