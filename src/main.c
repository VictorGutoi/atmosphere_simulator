#include <stdio.h>

#include <OpenGL/gl3.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

int main(void)
{
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

    printf("OpenGL window created successfully!\n");

    while (!glfwWindowShouldClose(window))
    {
        // Clear the screen.
        glClear(GL_COLOR_BUFFER_BIT);

        // Display what we rendered.
        glfwSwapBuffers(window);

        // Process keyboard/window events.
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}