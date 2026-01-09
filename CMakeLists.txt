cmake_minimum_required(VERSION 3.10)

project(RG25D2-Luka-Milic-47-2023-RN LANGUAGES C CXX)

# --------------------------------------------------
# C++ standard
# --------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------
# Output folder (bin)
# --------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
# GLFW
# --------------------------------------------------
add_subdirectory(libs/glfw-3.4/glfw-3.4)

# --------------------------------------------------
# ASSIMP (precompiled)
# --------------------------------------------------
if (MSVC)
    set(ASSIMP_LIB
        ${CMAKE_SOURCE_DIR}/libs/Assimp/lib/x64/assimp-vc143-mt.lib
    )
    set(ASSIMP_DLL
        ${CMAKE_SOURCE_DIR}/libs/Assimp/bin/x64/assimp-vc143-mt.dll
    )
elseif (MINGW)
    set(ASSIMP_LIB
        ${CMAKE_SOURCE_DIR}/libs/Assimp/lib/x64/libassimp.a
    )
    set(ASSIMP_DLL
        ${CMAKE_SOURCE_DIR}/libs/Assimp/bin/x64/libassimp.dll
    )
else()
    message(FATAL_ERROR "Unsupported compiler (only MSVC and MinGW supported)")
endif()

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
        ${CMAKE_SOURCE_DIR}/libs/glfw-3.4/glfw-3.4/include
        ${CMAKE_SOURCE_DIR}/libs/glm
        ${CMAKE_SOURCE_DIR}/libs/Assimp/include
        ${CMAKE_SOURCE_DIR}/Headers
)

# --------------------------------------------------
# Link libraries
# --------------------------------------------------
target_link_libraries(${PROJECT_NAME}
        glad
        glfw
        opengl32
        ${ASSIMP_LIB}
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

# --------------------------------------------------
# Copy ASSIMP DLL after build
# --------------------------------------------------
add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${ASSIMP_DLL}
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

add_dependencies(${PROJECT_NAME} copy_resources)

# --------------------------------------------------
# Info
# --------------------------------------------------
message(STATUS "Assimp lib: ${ASSIMP_LIB}")
message(STATUS "Assimp dll: ${ASSIMP_DLL}")
message(STATUS "Binary output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
