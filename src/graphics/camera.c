#include "graphics/camera.h"

#include <math.h>

#include "math/vec3.h"
#include "math/mat4.h"

Vec3 camera_forward(Camera camera) {
    return vec3_normalize(
        vec3_sub(camera.target, camera.position)
    );
}

Vec3 camera_right(Camera camera) {
    return vec3_normalize(
        vec3_cross(camera_forward(camera), camera.world_up)
    );
}

Vec3 camera_up(Camera camera) {
    return vec3_cross(
        camera_right(camera),
        camera_forward(camera)
    );
}

Mat4 camera_view(Camera camera) {
    return mat4_look_at(
        camera.position,
        camera.target,
        camera.world_up
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