#include "camera.h"

Camera camera = {
    .position = { 0, 0, 0 },
    .direction = { 0, 0, 1 },
    .forward_velocity = { 0, 0, 0 },
    .yaw = 0.0,
    .pitch = 0.0,
    .speed = 5.f,
    .up_speed = 3.f
};

mat4 look_at(vec3 eye, vec3 target, vec3 up) {
    vec3 z = normalize(target - eye);
    vec3 x = normalize(cross(up, z));
    vec3 y = cross(z, x);
    
    mat4 view_matrix;
    view_matrix[0] = geom::vec4(x.x, x.y, x.z, -dot(x, eye));
    view_matrix[1] = geom::vec4(y.x, y.y, y.z, -dot(y, eye));
    view_matrix[2] = geom::vec4(z.x, z.y, z.z, -dot(z, eye));
    view_matrix[3] = geom::vec4(0, 0, 0, 1);
    return view_matrix;
}
