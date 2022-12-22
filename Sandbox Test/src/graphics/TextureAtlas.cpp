#include "TextureAtlas.h"

TexAtlas::TexAtlas(Texture* texture, unsigned int numWidth, unsigned int numHeight)
	: m_Texture(texture), m_Width(numWidth), m_Height(numHeight)
{
	m_CellWidth = (unsigned int)((float)texture->GetDimentions().x / (float)m_Width);
	m_CellHeight = (unsigned int)((float)texture->GetDimentions().y / (float)m_Height);
}

glm::vec4 TexAtlas::GetQuad(unsigned int index)
{
	// Get texture index in the form of a coordinate
	glm::vec2 coord = glm::vec2(
		index % m_Width,
		index / m_Width
	);

	// Record texture dimentions for later use
	glm::vec2 texDimen = m_Texture->GetDimentions();

	// Calculate the resulting quad
	glm::vec4 quad = glm::vec4(
		m_CellWidth * coord.x,		// starting x value
		m_CellHeight * coord.y,		// starting y value
		m_CellWidth, m_CellHeight  // actual texture width
	);
	return quad;
}

Texture* TexAtlas::GetTexture()
{
	return m_Texture;
}

void TexAtlas::SetTextureInfo(Texture* texture, unsigned int numWidth, unsigned int numHeight)
{
	if (texture)
		m_Texture = texture;
	
	if (numWidth)
		m_Width = numWidth;
	
	if (numHeight)
		m_Height = numHeight;
}

void TexAtlas::GetTextureInfo(Texture* texture, unsigned int& numWidth, unsigned int& numHeight)
{
	if (texture)
		texture = m_Texture;

	if (numWidth)
		numWidth = (unsigned int)m_Texture->GetDimentions().x;

	if (numHeight)
		numHeight = (unsigned int)m_Texture->GetDimentions().y;
}
