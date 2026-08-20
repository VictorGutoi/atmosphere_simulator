cmake_minimum_required(VERSION 3.20)

project(atmosphere_simulator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(glad
    external/glad/src/gl.c
)

target_include_directories(glad
    PUBLIC
    external/glad/include
)

add_executable(atmosphere
    src/main.c
)

target_link_libraries(atmosphere
    PRIVATE
    glad
    glfw
    OpenGL::GL
)