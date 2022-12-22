#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Texture
{
public:
	Texture() = default;
	Texture(const char* filepath, bool hasAlpha = false, unsigned int texUnit = 0);

	void Bind() const;
	void Unbind() const;
	void Clean() const;

	unsigned int GetID();
	unsigned int GetTexUnit();

	glm::vec2 GetDimentions();

private:
	unsigned int m_ID;
	unsigned int m_TexUnit;
	int m_Width, m_Height, m_NumChannels;
};

#endif