#ifndef VAO_H
#define VAO_H

#include <glad/glad.h>
#include "VBO.hpp"

class VAO
{
    public:
        GLuint ID;
        VAO();

        static void LinkAttrib(const VBO &VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, const void* offset);
        void Bind() const;

        static void Unbind();
        void Delete() const;
};

#endif
