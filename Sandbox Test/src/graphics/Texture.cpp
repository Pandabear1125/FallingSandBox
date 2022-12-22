#include "Texture.h"

#include <iostream>

Texture::Texture(const char* filepath, bool hasAlpha, unsigned int texUnit)
	: m_ID(0), m_Width(0), m_Height(0), m_NumChannels(0), m_TexUnit(texUnit)
{
	glGenTextures(1, &m_ID);
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* data = stbi_load(filepath, &m_Width, &m_Height, &m_NumChannels, 0);
	if (data)
	{
		if (hasAlpha)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Texture Loaded: " << filepath << std::endl;
	}
	else
		std::cout << "Fatal Error: Failed to load texture: " << filepath << std::endl;

	stbi_image_free(data);


	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind() const
{
	glActiveTexture(GL_TEXTURE0 + m_TexUnit);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::Unbind() const
{
	glActiveTexture(GL_TEXTURE0 + m_TexUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Clean() const
{
	glActiveTexture(GL_TEXTURE0 + m_TexUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &m_ID);
}

unsigned int Texture::GetID()
{
	return m_ID;
}

unsigned int Texture::GetTexUnit()
{
	return m_TexUnit;
}

glm::vec2 Texture::GetDimentions()
{
	return glm::vec2((float)m_Width, (float)m_Height);
}
