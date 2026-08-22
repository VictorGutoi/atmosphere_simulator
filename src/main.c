#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#include "math/mat4.h"
#include "math/vec3.h"
#include "graphics/camera.h"
#include "graphics/mesh.h"

Vec3 position = {0.0f, 0.0f, 0.0f};

static Camera camera;

static double lastMouseX;
static double lastMouseY;
static int firstMouse = 1;

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

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = 0;
    }

    float xOffset =
        (float)(xpos - lastMouseX);

    float yOffset =
        (float)(lastMouseY - ypos);

    lastMouseX = xpos;
    lastMouseY = ypos;

    camera.yaw +=
        xOffset * camera.mouseSensitivity;

    camera.pitch +=
        yOffset * camera.mouseSensitivity;

    if (camera.pitch > 89.0f)
        camera.pitch = 89.0f;

    if (camera.pitch < -89.0f)
        camera.pitch = -89.0f;
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

int main(void) {
    camera = camera_create(position);

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
    glfwSetCursorPosCallback(
        window,
        mouse_callback
    );

    glfwSetInputMode(
        window,
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED
    );

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    // GLAD LOADED - CALL FUNCITONS AFTER

    glEnable(GL_DEPTH_TEST);

    Mesh sphere = mesh_create_sphere(
        2.0f,
        32,
        32
    );

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
    
    // Fragment Shader uniforms
    GLint light_direction_location = glGetUniformLocation(
        shader_program,
        "uLightDirection"
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


    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    free(vertex_source);
    free(fragment_source);

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

    Mat4 view = camera_get_view_matrix(&camera);

    glUniformMatrix4fv(
        view_location,
        1,
        GL_FALSE,
        view.m
    );

    Vec3 light_direction = {
        -1.0f,
        1.0f,
        1.0f
    };

    glUniform3f(
        light_direction_location,
        light_direction.x,
        light_direction.y,
        light_direction.z
    );

    static float previousTime = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        int width;
        int height;
        
        double time = glfwGetTime();
        
        float deltaTime =
            time - previousTime;

        previousTime = time;

        // MOVEMVENT DETECION
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            Vec3 forward = camera_get_forward(&camera);

            camera.position = vec3_add(
                camera.position,
                vec3_scale(
                    forward,
                    camera.movementSpeed * deltaTime
                )
            );
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            Vec3 forward = camera_get_forward(&camera);

            camera.position = vec3_add(
                camera.position,
                vec3_scale(
                    forward,
                    -camera.movementSpeed * deltaTime
                )
            );
        }
        
        Vec3 right = camera_get_right(&camera);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                Vec3 right = camera_get_right(&camera);

                camera.position = vec3_add(
                    camera.position,
                    vec3_scale(
                        right,
                        -camera.movementSpeed * deltaTime
                    )
                );
            }

            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                Vec3 right = camera_get_right(&camera);

                camera.position = vec3_add(
                    camera.position,
                    vec3_scale(
                        right,
                        camera.movementSpeed * deltaTime
                    )
                );
            }

        //UPDATE MATRIX
        Mat4 view = camera_get_view_matrix(&camera);

        glUniformMatrix4fv(
            view_location,
            1,
            GL_FALSE,
            view.m
        );
        
        //REDNER STUFF
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program);

        Mat4 model = mat4_translate(0.0f, 0.0f, -5.0f);

        glUniformMatrix4fv(
            model_location,
            1,
            GL_FALSE,
            model.m
        );

        mesh_draw(&sphere);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    mesh_destroy(&sphere);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}