cmake_minimum_required(VERSION 3.10)
project(RG25D2-Luka-Milic-47-2023-RN)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- GLAD ----
add_library(glad STATIC libs/glad/src/glad.c)
target_include_directories(glad PUBLIC libs/glad/include)

# ---- GLFW ----
add_subdirectory(libs/glfw-3.4/glfw-3.4)

# ---- EXECUTABLE ----
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

# ---- INCLUDE DIRECTORIES ----
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/glad/include
        ${CMAKE_SOURCE_DIR}/libs/glfw-3.4/glfw-3.4/include
        ${CMAKE_SOURCE_DIR}/libs/glm
        ${CMAKE_SOURCE_DIR}/libs/Assimp/include
        ${CMAKE_SOURCE_DIR}/Headers
)

# ---- LINK LIBRARIES ----
target_link_libraries(${PROJECT_NAME}
        glad
        glfw
        opengl32
        # DODAT x64 U PUTANJU (prema tvojoj slici)
        "${CMAKE_SOURCE_DIR}/libs/Assimp/lib/x64/assimp-vc143-mt.lib"
)

# ---- RESOURCES & DLL COPY ----
add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resources
        ${CMAKE_BINARY_DIR}/Resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Glsls
        ${CMAKE_BINARY_DIR}/Glsls

        # Kopiranje DLL-a (proveri da li je i on u bin/x64 ili samo bin)
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/libs/Assimp/bin/x64/assimp-vc143-mt.dll
        ${CMAKE_BINARY_DIR}/assimp-vc143-mt.dll
)

add_dependencies(${PROJECT_NAME} copy_resources)