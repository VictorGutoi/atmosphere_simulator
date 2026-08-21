#include "graphics/camera.h"

#include <math.h>

#include "math/vec3.h"
#include "math/mat4.h"

Camera camera_create(Vec3 position)
{
    Camera camera;

    camera.position = position;

    camera.yaw = -90.0f;
    camera.pitch = 0.0f;

    camera.movementSpeed = 3.0f;
    camera.mouseSensitivity = 0.1f;

    return camera;
}


Vec3 camera_get_forward(Camera *camera)
{
    float yaw = camera->yaw * (float)M_PI / 180.0f;
    float pitch = camera->pitch * (float)M_PI / 180.0f;

    Vec3 forward;

    forward.x = cosf(yaw) * cosf(pitch);
    forward.y = sinf(pitch);
    forward.z = sinf(yaw) * cosf(pitch);

    return vec3_normalize(forward);
}

Vec3 camera_get_right(Camera *camera)
{
    Vec3 forward = camera_get_forward(camera);

    Vec3 worldUp = {
        0.0f,
        1.0f,
        0.0f
    };

    return vec3_normalize(
        vec3_cross(forward, worldUp)
    );
}

Vec3 camera_get_up(Camera *camera)
{
    Vec3 forward = camera_get_forward(camera);
    Vec3 right = camera_get_right(camera);

    return vec3_normalize(
        vec3_cross(right, forward)
    );
}

Mat4 camera_get_view_matrix(Camera *camera)
{
    Vec3 forward = camera_get_forward(camera);

    Vec3 target = vec3_add(
        camera->position,
        forward
    );

    Vec3 worldUp = {
        0.0f,
        1.0f,
        0.0f
    };

    return mat4_look_at(
        camera->position,
        target,
        worldUp
    );
}

Mat4 mat4_look_at(Vec3 position, Vec3 target, Vec3 worldUp) {
    Vec3 forward =
        vec3_normalize(
            vec3_sub(target, position)
        );

    Vec3 right =
        vec3_normalize(
            vec3_cross(forward, worldUp)
        );

    Vec3 up =
        vec3_cross(right, forward);

    Mat4 result = mat4_identity();

    result.m[0] = right.x;
    result.m[1] = right.y;
    result.m[2] = right.z;

    result.m[4] = up.x;
    result.m[5] = up.y;
    result.m[6] = up.z;

    result.m[8]  = -forward.x;
    result.m[9]  = -forward.y;
    result.m[10] = -forward.z;

    result.m[12] = -vec3_dot(right, position);
    result.m[13] = -vec3_dot(up, position);
    result.m[14] = vec3_dot(forward, position);

    return result;
}


Mat4 mat4_perspective(float fov, float aspect, float near, float far) {
    Mat4 result = {0};

    float f = 1.0f / tanf(fov / 2.0f);

    result.m[0] = f / aspect;
    result.m[5] = f;

    result.m[10] = (far + near) / (near - far);
    result.m[11] = -1.0f;

    result.m[14] = (2.0f * far * near) / (near - far);

    return result;
}

void camera_move_forward(Camera *camera, float deltaTime) {
    Vec3 forward = camera_get_forward(camera);

    camera->position = vec3_add(
        camera->position,
        vec3_scale(
            forward,
            camera->movementSpeed * deltaTime
        )
    );
}

void camera_move_backward(Camera *camera, float deltaTime) {
    Vec3 forward = camera_get_forward(camera);

    camera->position = vec3_add(
        camera->position,
        vec3_scale(
            forward,
            -camera->movementSpeed * deltaTime
        )
    );
}