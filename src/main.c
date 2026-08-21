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
    GLint modelLoc = glGetUniformLocation(
        shader_program, 
        "uModel"
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

        glUseProgram(shader_program);

        glUniform2f(
            resolution_location,
            (float)width,
            (float)height
        );

        glUniform1f(
            time_location,
            (float)time
        );

        Mat4 model = mat4_rotate_x((float)glfwGetTime());


        glUniformMatrix4fv(
            modelLoc,
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