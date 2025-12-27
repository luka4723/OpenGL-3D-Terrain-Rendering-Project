#include "VBO.hpp"

VBO::VBO(const GLfloat* vertices, const GLsizeiptr size)
{
    glGenBuffers(1,&ID);
    glBindBuffer(GL_ARRAY_BUFFER,ID);
    glBufferData(GL_ARRAY_BUFFER,size,vertices,GL_STATIC_DRAW);
}
VBO::VBO(const particle_t* vertices, const GLsizeiptr size)
{
    glGenBuffers(1,&ID);
    glBindBuffer(GL_ARRAY_BUFFER,ID);
    glBufferData(GL_ARRAY_BUFFER,size,vertices,GL_STATIC_DRAW);
}

void VBO::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER,ID);
}
void VBO::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
void VBO::Delete() const {
    glDeleteBuffers(1,&ID);
}
void VBO::changeArray(const GLfloat* vertices, const GLsizeiptr size) const {
    glBindBuffer(GL_ARRAY_BUFFER,ID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size*sizeof(GLfloat), vertices);
}

void VBO::changeArray(const particle_t* vertices, const GLsizeiptr size) const {
    glBindBuffer(GL_ARRAY_BUFFER,ID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
}
