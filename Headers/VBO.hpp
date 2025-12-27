#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>

struct particle_t;

class VBO
{
    public:
        GLuint ID{};
        VBO(const GLfloat* vertices, GLsizeiptr size);
        VBO(const particle_t* vertices, GLsizeiptr size);

        void Bind() const;

        static void Unbind();
        void Delete() const;

        void changeArray(const GLfloat *vertices,GLsizeiptr size) const;
        void changeArray(const particle_t *vertices,GLsizeiptr size) const;
};

#endif
