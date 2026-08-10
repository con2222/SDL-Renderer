#include "camera.h"
#include "scene.h"
#include "display.h"

namespace C2 {

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

void update_camera_and_view(SceneData& scene, FrameData& frame_data) {
    geom::vec3 up = { 0, 1, 0 };
    geom::vec3 target = { 0, 0, 1 };
    geom::mat4 camera_yaw_rotation = geom::mat4_make_rotation_y(scene.camera.yaw);
    geom::mat4 camera_pitch_rotation = geom::mat4_make_rotation_x(scene.camera.pitch);
    scene.camera.direction = (camera_yaw_rotation * camera_pitch_rotation * geom::vec4_from_vec3(target)).xyz();
    target = scene.camera.position + scene.camera.direction;

    scene.view_matrix = look_at(scene.camera.position, target, up);
}

} // C2
