#ifndef CAMERA_H
#define CAMERA_H

#include "Geometry.h"

using namespace geom;


struct Camera {
    vec3 position;
    vec3 direction;
    vec3 forward_velocity;
    float yaw;
    float speed;
    float up_speed;
};

extern Camera camera;

mat4 look_at(vec3 eye, vec3 target, vec3 up);

#endif // CAMERA_H
