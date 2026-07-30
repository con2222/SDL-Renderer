#include "Geometry.h"
#include "display.h"
#include "mesh.h"
#include "color.h"
#include "triangle.h"

#include <cstdint>
#include <vector>
#include <utility>

const char* OBJ_FILENAME = "assets/cube.obj"; 

using namespace C2Renderer;

geom::vec2 project(EngineCore& engine_core, geom::vec3 point);

const int FPS = 240;
const double FRAME_TARGET_TIME = 1000.0 / FPS;
uint64_t previous_frame_time = 0;
const float fov_factor = 128;
geom::vec3 camera_position { 0, 0, 0 };

std::vector<Triangle> triangles_to_render;

void draw_filled_triangle(EngineCore& engine_core, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_line_DDA(EngineCore& engine_core, int x0, int y0, int x1, int y1, uint32_t color);
void fill_flat_bottom_triangle(EngineCore& engine_core, int x0, int y0, int x1, int y1, int Mx, int My, uint32_t color);
void fill_flat_top_triangle(EngineCore& engine_core, int x1, int y1, int Mx, int My, int x2, int y2, uint32_t color);


void fill_flat_bottom_triangle(EngineCore& engine_core, int x0, int y0, int x1, int y1, int Mx, int My, uint32_t color) {

    float inv_slope1 = static_cast<float>((x1 - x0)) / (y1 - y0);
    float inv_slope2 = static_cast<float>((Mx - x0)) / (My - y0);

    float current_x1 = x0;
    float current_x2 = x0;

    for (int y = y0; y <= y1; y++) {
        draw_line_DDA(engine_core, geom::round_float_to_int(current_x1), y, geom::round_float_to_int(current_x2), y, color);
        current_x1 += inv_slope1;
        current_x2 += inv_slope2;
    }
}

void fill_flat_top_triangle(EngineCore& engine_core, int x1, int y1, int Mx, int My, int x2, int y2, uint32_t color) {
    float inv_slope1 = static_cast<float>((x2 - x1)) / (y2 - y1);
    float inv_slope2 = static_cast<float>((x2 - Mx)) / (y2 - My);

    float current_x1 = x1;
    float current_x2 = Mx;

    for (int y = y1; y <= y2; y++) {
        draw_line_DDA(engine_core, geom::round_float_to_int(current_x1), y, geom::round_float_to_int(current_x2), y, color);
        current_x1 += inv_slope1;
        current_x2 += inv_slope2;
    }
}



// DDA
void draw_line_DDA(EngineCore& engine_core, int x0, int y0, int x1, int y1, uint32_t color) {
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;

    int side_length = abs(delta_x) > abs(delta_y) ? abs(delta_x) : abs(delta_y);

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

    draw_filled_triangle(engine_core, x0, y0, x1, y1, x2, y2, color);
}

void draw_filled_triangle(EngineCore& engine_core, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
    }
    if (y1 > y2) {
        std::swap(y1, y2);
        std::swap(x1, x2);
    }
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
    }

    if (y2 == y0) return;

    int My = y1;
    int Mx = ((x2 - x0) * (y1 - y0) / static_cast<float>((y2 - y0))) + x0;

    if (y1 > y0) {
        fill_flat_bottom_triangle(engine_core, x0, y0, x1, y1, Mx, My, color);
    }

    if (y2 > y1) {
        fill_flat_top_triangle(engine_core, x1, y1, Mx, My, x2, y2, color);
    }
}

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
    triangles_to_render.clear();

    uint64_t time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks64() - previous_frame_time);
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }
    previous_frame_time = SDL_GetTicks64();


    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;

    size_t num_faces = mesh.faces.size();
    size_t num_vertices = mesh.vertices.size();

    for (size_t i = 0; i < num_faces; i++) {
        Face mesh_face = mesh.faces[i];

        geom::vec3 face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a];
        face_vertices[1] = mesh.vertices[mesh_face.b];
        face_vertices[2] = mesh.vertices[mesh_face.c];

        Triangle projected_triangle;

        geom::vec3 transformed_vertices[3];

        // Apply transformations
        for (size_t j = 0; j < 3; j++) {
            geom::vec3 transformed_vertex = geom::vec3_rotate_x(face_vertices[j], mesh.rotation.x);
            transformed_vertex = geom::vec3_rotate_z(transformed_vertex, mesh.rotation.z);
            transformed_vertex = geom::vec3_rotate_y(transformed_vertex, mesh.rotation.y);

            transformed_vertex.z += 2.;
        
            transformed_vertices[j] =  transformed_vertex;
        }

        // Clockwise
        geom::vec3 vector_a = transformed_vertices[0]; /*   A   */
        geom::vec3 vector_b = transformed_vertices[1]; /*  / \  */
        geom::vec3 vector_c = transformed_vertices[2]; /* C---B */

        geom::vec3 vector_ab = vector_b - vector_a;
        geom::vec3 vector_ac = vector_c - vector_a;
        geom::vec3 normal = geom::cross(vector_ab, vector_ac); // left handed coordinate system
        geom::vec3 camera_ray = camera_position - vector_a;
        float product = geom::dot(normal, camera_ray);
        if (product < 0.0f) {
            continue;
        }

        // Loop all three vertices to perform projection
        for (size_t j = 0; j < 3; j++) {
            geom::vec2 project_point = project(engine_core, transformed_vertices[j]);

            project_point.x += engine_core.window.window_width / 2.0f;
            project_point.y += engine_core.window.window_height / 2.0f;

            projected_triangle.points[j] = project_point;
        }
        triangles_to_render.push_back(projected_triangle); 
    }
}

void render(EngineCore& engine_core) {

    size_t triangles_count = triangles_to_render.size();

    /*
    for (int i = 0; i < triangles_count; i++) {
        Triangle triangle = triangles_to_render[i];
        draw_rectangle(engine_core, triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFFFF00);
        draw_rectangle(engine_core, triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFFFF00);
        draw_rectangle(engine_core, triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFFFF00);
        

        draw_triangle(engine_core,
            triangle.points[0].x, triangle.points[0].y,
            triangle.points[1].x, triangle.points[1].y,
            triangle.points[2].x, triangle.points[2].y,
            C2::Color::Cyan
        );
    }*/

    draw_triangle(engine_core, 300, 100, 50, 400, 500, 700, 0xFF00FF00);

    render_color_buffer(engine_core);
    clear_color_buffer(engine_core, 0xFF000000); 
    SDL_RenderPresent(engine_core.renderer);
}

void setup(EngineCore& engine_core) {
    engine_core.color_buffer = new uint32_t[engine_core.window.window_width * engine_core.window.window_height]; 
    engine_core.color_buffer_texture = SDL_CreateTexture(engine_core.renderer, SDL_PIXELFORMAT_ARGB8888, 
        SDL_TEXTUREACCESS_STREAMING, engine_core.window.window_width, engine_core.window.window_height);

    // load_cube_mesh_data();
    load_obj_file_data(OBJ_FILENAME);
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

    return 0;
}
