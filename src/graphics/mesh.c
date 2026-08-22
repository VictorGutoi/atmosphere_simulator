#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "mesh.h"

Mesh mesh_create_sphere(float radius, unsigned int latitudeSegments, unsigned int longitudeSegments)
{
    Mesh mesh = {0};

    unsigned int vertexCount = (latitudeSegments + 1) * (longitudeSegments + 1);

    unsigned int indexCount = latitudeSegments * longitudeSegments * 6;

    float *vertices = malloc(vertexCount * 3 * sizeof(float));

    unsigned int *indices = malloc(indexCount * sizeof(unsigned int));

    if (!vertices || !indices)
    {
        free(vertices);
        free(indices);

        return mesh;
    }

    unsigned int vertexIndex = 0;
    for (unsigned int lat = 0; lat <= latitudeSegments; lat++) {
        float v = (float)lat / (float)latitudeSegments;

        float theta = v * (float)M_PI;

        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);

        for (unsigned int lon = 0; lon <= longitudeSegments; lon++) {
            float u =
                (float)lon /
                (float)longitudeSegments;

            float phi =
                u * 2.0f * (float)M_PI;

            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);

            float x = radius * sinTheta * cosPhi;

            float y = radius * cosTheta;

            float z = radius * sinTheta * sinPhi;

            vertices[vertexIndex++] = x;
            vertices[vertexIndex++] = y;
            vertices[vertexIndex++] = z;
        }
    }

    unsigned int index = 0;

    unsigned int ringSize =  longitudeSegments + 1;

    for (unsigned int lat = 0; lat < latitudeSegments; lat++) {
        for (unsigned int lon = 0; lon < longitudeSegments; lon++) {
            unsigned int current = lat * ringSize + lon;

            unsigned int next = current + ringSize;

            indices[index++] = current;
            indices[index++] = next;
            indices[index++] = current + 1;

            indices[index++] = current + 1;
            indices[index++] = next;
            indices[index++] = next + 1;
        }
    }

    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ebo);

    glBindVertexArray(mesh.vao);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertexCount * 3 * sizeof(float),
        vertices,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indexCount * sizeof(unsigned int),
        indices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    free(vertices);
    free(indices);

    glBindVertexArray(0);

    mesh.indexCount = indexCount;

    return mesh;
}

void mesh_draw(const Mesh *mesh) {
    glBindVertexArray(mesh->vao);

    glDrawElements(
        GL_TRIANGLES,
        mesh->indexCount,
        GL_UNSIGNED_INT,
        0
    );

    glBindVertexArray(0);
}

void mesh_destroy(Mesh *mesh) {
    glDeleteBuffers(1, &mesh->ebo);
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteVertexArrays(1, &mesh->vao);

    mesh->ebo = 0;
    mesh->vbo = 0;
    mesh->vao = 0;
}