#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


char *load_file(const char *path)
{
    FILE *file = fopen(path, "rb");

    if (!file)
    {
        fprintf(stderr, "Failed to open shader: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *buffer = malloc(size + 1);

    if (!buffer)
    {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';

    fclose(file);

    return buffer;
}

typedef struct {
    float m[16];
} Mat4;

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

Mat4 mat4_identity(void)
{
    Mat4 result = {0};

    result.m[0]  = 1.0f;
    result.m[5]  = 1.0f;
    result.m[10] = 1.0f;
    result.m[15] = 1.0f;

    return result;
}

Mat4 mat4_rotate_y(float angle)
{
    Mat4 result = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    result.m[0]  = c;
    result.m[2]  = -s;

    result.m[8]  = s;
    result.m[10] = c;

    return result;
}

Mat4 mat4_rotate_z(float angle)
{
    Mat4 result = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    result.m[0]  = c;
    result.m[1]  = -s;

    result.m[4]  = s;
    result.m[5] = c;

    return result;
}

Mat4 mat4_rotate_x(float angle)
{
    Mat4 result = mat4_identity();

    float c = cosf(angle);
    float s = sinf(angle);

    result.m[5]  = c;
    result.m[6]  = -s;

    result.m[9]  = s;
    result.m[10] = c;

    return result;
}

Mat4 mat4_translate(float x, float y, float z)
{
    Mat4 result = mat4_identity();

    result.m[12] = x;
    result.m[13] = y;
    result.m[14] = z;

    return result;
}

Mat4 mat4_multiply(Mat4 a, Mat4 b)
{
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

Mat4 mat4_perspective(float fov, float aspect, float near, float far)
{
    Mat4 result = {0};

    float f = 1.0f / tanf(fov / 2.0f);

    result.m[0] = f / aspect;
    result.m[5] = f;

    result.m[10] = (far + near) / (near - far);
    result.m[11] = -1.0f;

    result.m[14] = (2.0f * far * near) / (near - far);

    return result;
}

Vec3 vec3_add(Vec3 a, Vec3 b)
{
    return (Vec3){
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

Vec3 vec3_sub(Vec3 a, Vec3 b)
{
    return (Vec3){
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

Vec3 vec3_scale(Vec3 v, float s)
{
    return (Vec3){
        v.x * s,
        v.y * s,
        v.z * s
    };
}

float vec3_dot(Vec3 a, Vec3 b)
{
    return
        a.x * b.x +
        a.y * b.y +
        a.z * b.z;
}

Vec3 vec3_cross(Vec3 a, Vec3 b)
{
    return (Vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Vec3 vec3_normalize(Vec3 v)
{
    float length = sqrtf(
        v.x * v.x +
        v.y * v.y +
        v.z * v.z
    );

    if (length == 0.0f)
        return (Vec3){0.0f, 0.0f, 0.0f};

    return (Vec3){
        v.x / length,
        v.y / length,
        v.z / length
    };
}

Mat4 mat4_look_at(Vec3 position,Vec3 target,Vec3 worldUp)
{
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

GLuint compile_shader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char info_log[512];

        glGetShaderInfoLog(
            shader,
            sizeof(info_log),
            NULL,
            info_log
        );

        fprintf(stderr, "Shader compilation failed:\n%s\n", info_log);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}


int main(void)
{
    float vertices[] = {
         0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f
    };

    Vec3 cameraPosition = {
        0.0f,
        0.0f,
        0.0f
    };

    Vec3 cameraTarget = {
        4.0f,
        0.0f,
        -5.0f
    };

    Vec3 forward = {
        0.0f,
        0.0f,
        -1.0f
    };

    Vec3 worldUp = {
        0.0f,
        1.0f,
        0.0f
    };

    Vec3 right =
        vec3_normalize(
            vec3_cross(forward, worldUp)
        );

    Vec3 up =
        vec3_cross(right, forward);

    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(
        1280,
        720,
        "Atmosphere Simulator",
        NULL,
        NULL
    );

    if (!window)
    {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    char *vertex_source = load_file("shaders/basic.vert");
    char *fragment_source = load_file("shaders/basic.frag");

    if (!vertex_source || !fragment_source)
    {
        return 1;
    }

    GLuint vertex_shader =
        compile_shader(GL_VERTEX_SHADER, vertex_source);

    GLuint fragment_shader =
        compile_shader(GL_FRAGMENT_SHADER, fragment_source);

    if (!vertex_shader || !fragment_shader)
    {
        return 1;
    }

    GLuint shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);
    
    // Fragment Shader uniforms
    GLint resolution_location = glGetUniformLocation(
        shader_program,
        "uResolution"
    );
    GLint time_location = glGetUniformLocation(
        shader_program,
        "uTime"
    );

    // Vertex Shader uniforms
    GLint model_location = glGetUniformLocation(
        shader_program, 
        "uModel"
    );

    GLint projection_location = glGetUniformLocation(
        shader_program, 
        "uProjection"
    );

    GLint view_location = glGetUniformLocation(
        shader_program, 
        "uView"
    );

    GLint success;
    glGetProgramiv(
        shader_program,
        GL_LINK_STATUS,
        &success
    );

    if (!success)
    {
        char info_log[512];

        glGetProgramInfoLog(
            shader_program,
            sizeof(info_log),
            NULL,
            info_log
        );

        fprintf(stderr, "Shader linking failed:\n%s\n", info_log);

        return 1;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    free(vertex_source);
    free(fragment_source);

    GLuint VBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    GLuint VAO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void *)0
    );

    glEnableVertexAttribArray(0);

    float aspect = (float)1280 / (float)720;
    Mat4 projection = mat4_perspective(
        60.0f * (float)M_PI / 180.0f,
        aspect,
        0.1f,
        100.0f
    );

    glUseProgram(shader_program);

    glUniformMatrix4fv(
        projection_location,
        1,
        GL_FALSE,
        projection.m
    );

    Mat4 view = mat4_look_at(
        cameraPosition,
        cameraTarget,
        worldUp
    );

    glUniformMatrix4fv(
        view_location,
        1,
        GL_FALSE,
        view.m
    );

    while (!glfwWindowShouldClose(window))
    {
        int width;
        int height;
        
        double time = glfwGetTime();

        glfwGetFramebufferSize(
            window,
            &width,
            &height
        );

        glViewport(
            0,
            0,
            width,
            height
        );

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);

        glUniform2f(
            resolution_location,
            (float)width,
            (float)height
        );

        glUniform1f(
            time_location,
            (float)time
        );

        Mat4 model = mat4_translate(.0f, .0f, -5.0f);

        glUniformMatrix4fv(
            model_location,
            1,
            GL_FALSE,
            model.m
        );

        glDrawArrays(
            GL_TRIANGLES,
            0,
            3
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}