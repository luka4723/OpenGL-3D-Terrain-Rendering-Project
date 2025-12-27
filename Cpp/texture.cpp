#include "texture.hpp"
#include "stb_image.hpp"

Texture::Texture(const char* image, const GLenum texType, const GLenum texSlot, const GLenum pixelType, const GLenum wrap, const int filter)
{
	type = texType;
	slot = texSlot;

	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image, &width, &height, &nrChannels, 0);

    GLenum format=0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;

	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	if(filter==1)
    {
        glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
    {
        glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

	glTexParameteri(texType, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap));
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap));

	glTexImage2D(texType, 0, format, width, height, 0, format, pixelType, bytes);
	glGenerateMipmap(texType);

	stbi_image_free(bytes);
	glBindTexture(texType, 0);
}

void Texture::texUnit(const Shader &shader, const char* uniform) const {
    shader.Activate();
	const GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	glUniform1i(texUni, slot-GL_TEXTURE0);
}

void Texture::Bind() const {
    glActiveTexture(slot);
	glBindTexture(type, ID);
}

void Texture::Unbind() const {
	glBindTexture(type, 0);
}

void Texture::Delete() const {
	glDeleteTextures(1, &ID);
}

int Texture::getWidth() const {
	return this->width;
}
int Texture::getHeight() const {
	return this->height;
}

int Texture::getNrChannels() const {
	return this->nrChannels;
}

