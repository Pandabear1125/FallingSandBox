#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#include "Texture.h"

class TexAtlas
{
public:
	TexAtlas() = default;
	TexAtlas(Texture* texture, unsigned int numWidth, unsigned int numHeight);

	glm::vec4 GetQuad(unsigned int index);
	Texture* GetTexture();

	// Each of these values can be null if needed
	void SetTextureInfo(Texture* texture, unsigned int numWidth, unsigned int numHeight);
	void GetTextureInfo(Texture* texture, unsigned int& numWidth, unsigned int& numHeight);

private:
	Texture* m_Texture = nullptr;
	unsigned int m_Width = 0;
	unsigned int m_Height = 0;
	unsigned int m_CellWidth = 0;
	unsigned int m_CellHeight = 0;

};

#endif