#ifndef MESH_H
#define MESH_H

#include <glad/gl.h>

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    unsigned int indexCount;
} Mesh;

Mesh mesh_create_sphere(
    float radius,
    unsigned int latitudeSegments,
    unsigned int longitudeSegments
);

void mesh_draw(const Mesh *mesh);

void mesh_destroy(Mesh *mesh);

#endif