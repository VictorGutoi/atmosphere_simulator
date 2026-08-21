#ifndef CAMERA_H
#define CAMERA_H

#include "math/mat4.h"
#include "math/vec3.h"

typedef struct {
    Vec3 position;
    Vec3 target;
    Vec3 world_up;
} Camera;

Vec3 camera_forward(Camera camera);
Vec3 camera_right(Camera camera);
Vec3 camera_up(Camera camera);

Mat4 camera_view(Camera camera);

Mat4 mat4_look_at(Vec3 position, Vec3 target, Vec3 worldUp);

Mat4 mat4_perspective(float fov, float aspect, float near, float far);

#endif
