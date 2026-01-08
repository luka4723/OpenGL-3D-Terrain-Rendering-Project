cmake_minimum_required(VERSION 3.10)
project(RG25D2-Luka-Milic-47-2023-RN)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- GLAD ----
add_library(glad STATIC libs/glad/src/glad.c)
target_include_directories(glad PUBLIC libs/glad/include)

# ---- GLFW ----
add_subdirectory(libs/glfw-3.4/glfw-3.4)

# ---- ASSIMP ----
# Proveravamo compiler i postavljamo lib i dll
if(MSVC)
    # MSVC: linkujemo .lib i DLL
    set(ASSIMP_LIB "${CMAKE_SOURCE_DIR}/libs/Assimp/lib/x64/assimp-vc143-mt.lib")
    set(ASSIMP_DLL "${CMAKE_SOURCE_DIR}/libs/Assimp/bin/x64/assimp-vc143-mt.dll")
elseif(MINGW)
    # MinGW: linkujemo .a i DLL
    set(ASSIMP_LIB "${CMAKE_SOURCE_DIR}/libs/Assimp/lib/x64/libassimp.a")
    set(ASSIMP_DLL "${CMAKE_SOURCE_DIR}/libs/Assimp/bin/x64/libassimp.dll")
else()
    message(FATAL_ERROR "Unsupported compiler. Only MSVC and MinGW are supported.")
endif()

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
        ${ASSIMP_LIB}
)

# ---- OUTPUT DIRECTORIES ----
# Sve exe i DLL-ove stavimo u bin folder
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# ---- RESOURCES & DLL COPY ----
add_custom_target(copy_resources ALL
        # Kreiraj bin folder ako ne postoji
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
        # Kopiraj Resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Resources
        ${CMAKE_BINARY_DIR}/bin/Resources
        # Kopiraj Glsls
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Glsls
        ${CMAKE_BINARY_DIR}/bin/Glsls
        # Kopiraj DLL u bin folder
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${ASSIMP_DLL}
        ${CMAKE_BINARY_DIR}/bin/
)

add_dependencies(${PROJECT_NAME} copy_resources)

message(STATUS "Assimp lib: ${ASSIMP_LIB}")
message(STATUS "Assimp dll: ${ASSIMP_DLL}")
message(STATUS "Executable and DLLs will go to: ${CMAKE_BINARY_DIR}/bin")
