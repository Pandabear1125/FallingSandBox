#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

//#include <ft2build.h>
//#include FT_FREETYPE_H

#include <map>
#include <string>
#include <vector>
#include <fstream>

#include "Shader.h"
#include "Texture.h"

//struct Character {
//	unsigned int TextureID; // ID handle of the glyph texture
//	glm::ivec2 Size; // Size of glyph
//	glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
//	unsigned int Advance; // Offset to advance to next glyph
//};

//class TTFTextRenderer
//{
//public:
//	// constructor
//	TTFTextRenderer(Shader* shader);
//
//	unsigned int getPoint();
//
//	// load a new font. clears all old font data
//	void Load(std::string font, unsigned int point);
//
//	void Draw(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.f));
//
//
//private:
//	unsigned int m_VAO, m_VBO;
//	Shader* m_Shader;
//	std::map<char, Character> m_Characters;
//	unsigned int m_Point;
//};

struct BMPFont
{
	Texture Font;

	// array of all char widths
	int Widths[128];
	char StartChar;
	// individual cell width and height in pixels
	int CellWidth;
	int CellHeight;
	// number of cells for width and height
	int XCells;
	int YCells;
};

struct Letter
{
	Letter(float vertArray[6][4]);
	Letter(
		float x1, float y1, float u1, float v1,
		float x2, float y2, float u2, float v2,
		float x3, float y3, float u3, float v3,
		float x4, float y4, float u4, float v4,
		float x5, float y5, float u5, float v5,
		float x6, float y6, float u6, float v6
	);
	float quad[6][4];
};

class BMPTextRenderer
{
public:
	BMPTextRenderer(Shader* shader);

	void Load(const char* fontBMPPath, const char* fontDataPath);

	void Draw(std::string text, float x, float y, int truncAfterDecimal = -1, float scale = 1.f, glm::vec3 color = glm::vec3(1.f));

private:
	// render vars
	unsigned int m_VAO, m_VBO, m_ColorVBO;

	Shader* m_Shader;

	BMPFont m_FontData;

	std::vector<Letter> m_LetterBuffer;
};

void truncAfterPoint(std::string &str, int decimalPoints = -1);

int split(std::string inputString, char seperator, std::string output[], int size);

#endif