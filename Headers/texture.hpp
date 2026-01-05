#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>

#include"shader.hpp"

class Texture
{
public:
	GLuint ID{};
	GLenum type;
	GLenum slot;
	int width, height, nrChannels;
	unsigned char* data;
	Texture(const char* image, GLenum texType, GLenum texSlot, GLenum pixelType, GLenum wrap, int filter);

	void free() const;
	void texUnit(const Shader& shader, const char* uniform) const;
	void Bind() const;
	void Unbind() const;
	void Delete() const;
	int getWidth() const;
	int getHeight() const;
	int getNrChannels() const;
};
#endif
