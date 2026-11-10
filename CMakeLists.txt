cmake_minimum_required(VERSION 3.14)

project(terrain_rendering LANGUAGES C CXX)

# --------------------------------------------------
# C++ standard
# --------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------
# Output folder (bin)
# --------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Visual Studio would put the executable in bin/Debug or bin/Release, away from the copied resources
foreach (config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} config)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config} ${CMAKE_BINARY_DIR}/bin)
endforeach ()

# --------------------------------------------------
# GLAD
# --------------------------------------------------
add_library(glad STATIC
        libs/glad/src/glad.c
)
target_include_directories(glad PUBLIC
        libs/glad/include
)

# --------------------------------------------------
# GLFW (downloaded at configure time)
# --------------------------------------------------
include(FetchContent)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Build GLFW with Wayland support (Linux)")
FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
)

# --------------------------------------------------
# GLM (downloaded at configure time, header-only)
# --------------------------------------------------
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glfw glm)

# --------------------------------------------------
# ASSIMP (installed one if CMake finds it, otherwise built from source with only the OBJ importer)
# --------------------------------------------------
find_package(assimp CONFIG QUIET)
if (NOT assimp_FOUND)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
    set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
    FetchContent_Declare(assimp
            GIT_REPOSITORY https://github.com/assimp/assimp.git
            GIT_TAG v5.4.3
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(assimp)
endif()

find_package(OpenGL REQUIRED)

# --------------------------------------------------
# Executable
# --------------------------------------------------
add_executable(${PROJECT_NAME}
        main.cpp
        Cpp/camera.cpp
        Cpp/shader.cpp
        Cpp/VAO.cpp
        Cpp/VBO.cpp
        Cpp/EBO.cpp
        Cpp/helper.cpp
        Cpp/texture.cpp
        Cpp/stb_image.cpp
        Cpp/mesh.cpp
        Cpp/model.cpp
        Headers/mesh.hpp
        Headers/model.hpp
)

# --------------------------------------------------
# Include directories
# --------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/glad/include
        ${CMAKE_SOURCE_DIR}/Headers
)

# --------------------------------------------------
# Link libraries
# --------------------------------------------------
target_link_libraries(${PROJECT_NAME}
        glad
        glfw
        glm::glm
        OpenGL::GL
        assimp::assimp
)

# --------------------------------------------------
# Copy resources (textures, shaders, etc.)
# --------------------------------------------------
add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/Resources
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/Glsls
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Glsls
)

add_dependencies(${PROJECT_NAME} copy_resources)

# --------------------------------------------------
# Info
# --------------------------------------------------
message(STATUS "Binary output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
