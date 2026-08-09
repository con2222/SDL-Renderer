#ifndef CAMERA_H
#define CAMERA_H

#include "Geometry.h"

using namespace geom;


struct Camera {
    vec3 position = { 0, 0, 0 };
    vec3 direction = { 0, 0, 1 };
    vec3 forward_velocity = { 0, 0, 0 };
    float yaw = 0.f;
    float pitch = 0.f;
    float speed = 5.f;
    float up_speed = 3.f;
};

mat4 look_at(vec3 eye, vec3 target, vec3 up);

#endif // CAMERA_H
