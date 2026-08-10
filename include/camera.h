#ifndef CAMERA_H
#define CAMERA_H

#include "Geometry.h"

namespace C2 {

struct SceneData;
struct FrameData;

using namespace geom;

void update_camera_and_view(SceneData& scene, C2::FrameData& frame_data);

struct Camera {
    vec3 position = { 0, 0, 0 };
    vec3 direction = { 0, 0, 1 };
    vec3 forward_velocity = { 0, 0, 0 };
    float yaw = 0.f;
    float pitch = 0.f;
    float speed = 5.f;
    float up_speed = 3.f;
    float sensitivity = 0.05f;
};

mat4 look_at(vec3 eye, vec3 target, vec3 up);

}; // C2

#endif // CAMERA_H
