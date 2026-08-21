#ifndef CAMERA_H
#define CAMERA_H

#include "math/mat4.h"
#include "math/vec3.h"

typedef struct {
    Vec3 position;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
} Camera;

Camera camera_create(Vec3 position);

Vec3 camera_get_forward(Camera *camera);
Vec3 camera_get_right(Camera *camera);
Vec3 camera_get_up(Camera *camera);

Mat4 camera_get_view_matrix(Camera *camera);

Mat4 mat4_look_at(Vec3 position, Vec3 target, Vec3 worldUp);

Mat4 mat4_perspective(float fov, float aspect, float near, float far);

void camera_move_forward(Camera *camera, float deltaTime);
void camera_move_backward(Camera *camera, float deltaTime);

#endif
