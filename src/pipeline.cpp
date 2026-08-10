#include "pipeline.h"
#include "color.h"
#include "mesh.h"
#include "display.h"
#include "scene.h"
#include "clipping.h"


namespace C2 {

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

}
