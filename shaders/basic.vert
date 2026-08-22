#version 410 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

out vec3 vNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    vec4 worldPosition = uModel * vec4(aPosition, 1.0);

    vNormal = mat3(uModel) * aNormal;

    gl_Position =
        uProjection *
        uView *
        worldPosition;
}