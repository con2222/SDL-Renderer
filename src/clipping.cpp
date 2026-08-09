#include "clipping.h"

#include "Geometry.h"
#include "triangle.h"

Frustum frustum;

Frustum create_frustum(float fov_x, float fov_y, float z_near, float z_far) {
        Frustum f;
        
        float cos_half_fov_x = std::cos(fov_x / 2.0f);
        float sin_half_fov_x = std::sin(fov_x / 2.0f);

        float cos_half_fov_y = std::cos(fov_y / 2.0f);
        float sin_half_fov_y = std::sin(fov_y / 2.0f);

        // Left plane
        f.left.point = {0.0f, 0.0f, 0.0f};
        f.left.normal = {cos_half_fov_x, 0.0f, sin_half_fov_x};

        // Right plane
        f.right.point = {0.0f, 0.0f, 0.0f};
        f.right.normal = {-cos_half_fov_x, 0.0f, sin_half_fov_x};

        // Top plane
        f.top.point = {0.0f, 0.0f, 0.0f};
        f.top.normal = {0.0f, -cos_half_fov_y, sin_half_fov_y};

        // Bot plane
        f.bottom.point = {0.0f, 0.0f, 0.0f};
        f.bottom.normal = {0.0f, cos_half_fov_y, sin_half_fov_y};

        // Near plane
        f.near.point = {0.0f, 0.0f, z_near};
        f.near.normal = {0.0f, 0.0f, 1.0f};

        // Far plane
        f.far.point = {0.0f, 0.0f, z_far};
        f.far.normal = {0.0f, 0.0f, -1.0f};

        return f;
}

float float_lerp(float a, float b, float t) {
    return a + t * (b - a);
}

Polygon create_polygon_from_triangle(geom::vec3 v0, geom::vec3 v1, geom::vec3 v2, geom::vec2 t0, geom::vec2 t1, geom::vec2 t2) {
    Polygon polygon;
    polygon.vertices[0] = v0;
    polygon.vertices[1] = v1;
    polygon.vertices[2] = v2;
    polygon.texcoords[0] = t0;
    polygon.texcoords[1] = t1;
    polygon.texcoords[2] = t2;
    polygon.num_vertices = 3;
    return polygon;
}

void clip_polygon_againts_plain(Polygon& polygon, const Plane& plane) {
    geom::vec3 plane_point = plane.point;
    geom::vec3 plane_normal = plane.normal;
    
    // Part of the final polygon
    geom::vec3 inside_vertices[MAX_NUM_POLY_VERTICES];
    geom::vec2 inside_texcoords[MAX_NUM_POLY_VERTICES];
    int num_inside_vertices = 0;

    geom::vec3* current_vertex = &polygon.vertices[0];
    geom::vec2* current_texcoord = &polygon.texcoords[0];

    geom::vec3* previous_vertex = &polygon.vertices[polygon.num_vertices - 1];
    geom::vec2* previous_texcoord = &polygon.texcoords[polygon.num_vertices - 1];


    float current_dot = 0;
    float previous_dot = geom::dot((*previous_vertex - plane_point), plane_normal);

    while (current_vertex != &polygon.vertices[polygon.num_vertices]) {
        current_dot = geom::dot((*current_vertex - plane_point), plane_normal);
        if (current_dot * previous_dot < 0) {
            // TODO Calculate interpolation intersection
            float t = previous_dot / (previous_dot - current_dot);
            geom::vec3 intersection_point = *previous_vertex + t * (*current_vertex - *previous_vertex);
            geom::vec2 intersection_texcoord = *previous_texcoord + t * (*current_texcoord - *previous_texcoord);

            inside_vertices[num_inside_vertices] = intersection_point;
            inside_texcoords[num_inside_vertices] = intersection_texcoord;
            num_inside_vertices++;
        }
        if (current_dot >= 0) {
            inside_vertices[num_inside_vertices] = *current_vertex;
            inside_texcoords[num_inside_vertices] = *current_texcoord;
            num_inside_vertices++;
        }
        previous_dot = current_dot;
        previous_vertex = current_vertex;
        previous_texcoord =  current_texcoord;
        current_vertex++;
        current_texcoord++;
    }

    for (size_t i = 0; i < num_inside_vertices; i++) {
        polygon.vertices[i] = inside_vertices[i];
        polygon.texcoords[i] = inside_texcoords[i];
    }
    polygon.num_vertices = num_inside_vertices;
}

void clip_polygon(Polygon& polygon) {
    clip_polygon_againts_plain(polygon, frustum.left);
    clip_polygon_againts_plain(polygon, frustum.right);
    clip_polygon_againts_plain(polygon, frustum.top);
    clip_polygon_againts_plain(polygon, frustum.bottom); 
    clip_polygon_againts_plain(polygon, frustum.near);
    clip_polygon_againts_plain(polygon, frustum.far);
}

void triangles_from_polygon(Polygon& polygon, Triangle triangles[], int& num_triangles) {
    if (polygon.num_vertices < 3) {
        num_triangles = 0;
        return;
    }

    for (int i = 0; i < polygon.num_vertices - 2; i++) {
        int index0 = 0;
        int index1 = i + 1;
        int index2 = i + 2;

        triangles[i].points[0] = geom::vec4_from_vec3(polygon.vertices[index0]);
        triangles[i].points[1] = geom::vec4_from_vec3(polygon.vertices[index1]);
        triangles[i].points[2] = geom::vec4_from_vec3(polygon.vertices[index2]);
        
        // From geom::vec2 to Texel
        triangles[i].texcoords[0] = { (float)polygon.texcoords[index0].x, (float)polygon.texcoords[index0].y };
        triangles[i].texcoords[1] = { (float)polygon.texcoords[index1].x, (float)polygon.texcoords[index1].y };
        triangles[i].texcoords[2] = { (float)polygon.texcoords[index2].x, (float)polygon.texcoords[index2].y };
    }
    num_triangles = polygon.num_vertices - 2;
}
