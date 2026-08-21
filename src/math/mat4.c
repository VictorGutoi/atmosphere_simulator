#include "math/mat4.h"

#include <math.h>

Mat4 mat4_identity(void) {
    Mat4 result = {0};

    result.m[0]  = 1.0f;
    result.m[5]  = 1.0f;
    result.m[10] = 1.0f;
    result.m[15] = 1.0f;

    return result;
}

Mat4 mat4_rotate_y(float angle) {
    Mat4 result = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    result.m[0]  = c;
    result.m[2]  = -s;

    result.m[8]  = s;
    result.m[10] = c;

    return result;
}

Mat4 mat4_rotate_z(float angle) {
    Mat4 result = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    result.m[0]  = c;
    result.m[1]  = -s;

    result.m[4]  = s;
    result.m[5] = c;

    return result;
}

Mat4 mat4_rotate_x(float angle) {
    Mat4 result = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    result.m[5]  = c;
    result.m[6]  = -s;

    result.m[9]  = s;
    result.m[10] = c;

    return result;
}

Mat4 mat4_translate(float x, float y, float z) {
    Mat4 result = mat4_identity();

    result.m[12] = x;
    result.m[13] = y;
    result.m[14] = z;

    return result;
}

Mat4 mat4_multiply(Mat4 a, Mat4 b) {
    Mat4 result = {0};

    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            result.m[column * 4 + row] =
                a.m[0 * 4 + row] * b.m[column * 4 + 0] +
                a.m[1 * 4 + row] * b.m[column * 4 + 1] +
                a.m[2 * 4 + row] * b.m[column * 4 + 2] +
                a.m[3 * 4 + row] * b.m[column * 4 + 3];
        }
    }

    return result;
}
