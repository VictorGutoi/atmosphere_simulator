#ifndef MAT4_H
#define MAT4_H

typedef struct {
    float m[16];
} Mat4;

Mat4 mat4_identity(void);

Mat4 mat4_rotate_y(float angle);

Mat4 mat4_rotate_z(float angle);

Mat4 mat4_rotate_x(float angle);

Mat4 mat4_translate(float x, float y, float z);

Mat4 mat4_multiply(Mat4 a, Mat4 b);

#endif
