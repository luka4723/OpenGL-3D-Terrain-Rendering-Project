cmake_minimum_required(VERSION 3.10)
project(RG25D2-Luka-Milic-47-2023-RN)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- GLAD ----d
add_library(glad STATIC libs/glad/src/glad.c)
target_include_directories(glad PUBLIC libs/glad/include)

# ---- GLFW ----
add_subdirectory(libs/glfw-3.4/glfw-3.4)

# ---- Dodavanje CPP fajlova ručno ----
add_executable(RG25D2-Luka-Milic-47-2023-RN
        main.cpp
        Cpp/camera.cpp
        Cpp/shader.cpp
        Cpp/VAO.cpp
        Cpp/VBO.cpp
        Cpp/EBO.cpp
        Cpp/helper.cpp
        Cpp/texture.cpp
        Cpp/stb_image.cpp
)

# ---- Include directories ----
target_include_directories(RG25D2-Luka-Milic-47-2023-RN PUBLIC
        ${CMAKE_SOURCE_DIR}/libs/glad/include
        ${CMAKE_SOURCE_DIR}/libs/glfw-3.4/glfw-3.4/include
        ${CMAKE_SOURCE_DIR}/libs/glm
        ${CMAKE_SOURCE_DIR}/Headers
)

# ---- Link libraries ----
target_link_libraries(RG25D2-Luka-Milic-47-2023-RN
        glad
        glfw
        opengl32
)

# ---- Kopiranje resursa i shader-a kao custom target ----
add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resources
        ${CMAKE_BINARY_DIR}/Resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Glsls
        ${CMAKE_BINARY_DIR}/Glsls
)

# ---- Neka executable zavisi od copy_resources ----
add_dependencies(RG25D2-Luka-Milic-47-2023-RN copy_resources)
