#include "TextRender.h"

//TTFTextRenderer::TTFTextRenderer(Shader* shader)
//	: m_Shader(shader)
//{
//	glGenVertexArrays(1, &m_VAO);
//	glGenBuffers(1, &m_VBO);
//	glBindVertexArray(m_VAO);
//	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindVertexArray(0);
//}
//
//unsigned int TTFTextRenderer::getPoint()
//{
//	return m_Point;
//}
//
//void TTFTextRenderer::Load(std::string font, unsigned int point)
//{
//	m_Point = point;
//
//	m_Characters.clear();
//
//	FT_Library ft;
//	if (FT_Init_FreeType(&ft))
//		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
//
//	FT_Face face;
//	if (FT_New_Face(ft, font.c_str(), 0, &face))
//		std::cout << "ERROR::FREETYPE: Failed to load font | " << font << std::endl;
//
//	FT_Set_Pixel_Sizes(face, 0, point);
//
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	// then for the first 128 ASCII characters, pre-load/compile their characters and store them
//	for (GLubyte c = 0; c < 128; c++) // lol see what I did there 
//	{
//		// load character glyph 
//		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
//		{
//			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
//			continue;
//		}
//		// generate texture
//		unsigned int texture;
//		glGenTextures(1, &texture);
//		glBindTexture(GL_TEXTURE_2D, texture);
//		glTexImage2D(
//			GL_TEXTURE_2D,
//			0,
//			GL_RED,
//			face->glyph->bitmap.width,
//			face->glyph->bitmap.rows,
//			0,
//			GL_RED,
//			GL_UNSIGNED_BYTE,
//			face->glyph->bitmap.buffer
//		);
//		// set texture options
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		// now store character for later use
//		Character character = {
//			 texture,
//			 glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
//			 glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
//			 (unsigned int)face->glyph->advance.x
//		};
//		m_Characters.insert(std::pair<char, Character>(c, character));
//	}
//	glBindTexture(GL_TEXTURE_2D, 0);
//	// destroy FreeType once we're finished
//	FT_Done_Face(face);
//	FT_Done_FreeType(ft);
//}
//
//void TTFTextRenderer::Draw(std::string text, float x, float y, float scale, glm::vec3 color)
//{
//	// activate corresponding render state
//	float originX = x;
//	m_Shader->use();
//	m_Shader->setVec3("TextColor", color);
//	glActiveTexture(GL_TEXTURE0);
//	glBindVertexArray(m_VAO);
//	// iterate through all characters
//	std::string::const_iterator c;
//	for (c = text.begin(); c != text.end(); c++)
//	{
//		if (*c == 10)
//		{
//			y += m_Point;
//			x = originX;
//			continue;
//		}
//		Character ch = m_Characters[*c];
//		float xpos = x + ch.Bearing.x * scale;
//		float ypos = y + (m_Characters['H'].Bearing.y - ch.Bearing.y) * scale;
//		float w = ch.Size.x * scale;
//		float h = ch.Size.y * scale;
//
//		// update VBO for each character
//		float vertices[6][4] = {
//			{ xpos,		ypos + h,	0.0f, 1.0f },
//			{ xpos + w, ypos,			1.0f, 0.0f },
//			{ xpos,		ypos,			0.0f, 0.0f },
//			{ xpos,		ypos + h,	0.0f, 1.0f },
//			{ xpos + w, ypos + h,	1.0f, 1.0f },
//			{ xpos + w, ypos,			1.0f, 0.0f }
//		};
//
//		Letter letter(vertices);
//
//		// render glyph texture over quad
//		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
//		// update content of VBO memory
//		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(letter), &letter);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		// render quad
//		glDrawArrays(GL_TRIANGLES, 0, 6);
//		// advance cursors for next glyph (advance is 1/64 pixels)
//		x += (ch.Advance >> 6) * scale; // bitshift by 6 (2^6 = 64)
//	}
//	glBindVertexArray(0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//}

BMPTextRenderer::BMPTextRenderer(Shader* shader)
	: m_Shader(shader)
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_ColorVBO);

	glBindVertexArray(m_VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_ColorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
	glVertexAttribDivisor(1, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void BMPTextRenderer::Load(const char* fontBMPPath, const char* fontDataPath)
{
	m_FontData.Font = Texture(fontBMPPath, true);


	std::ifstream fin;
	fin.open(fontDataPath);
	std::string line;
	std::string output[5];
	std::string suboutput[5];

	while (std::getline(fin, line))
	{
		split(line, ',', output, 5);

		if (output[0] == "Cell Width")
		{
			m_FontData.CellWidth = std::stoi(output[1]);
			continue;
		}
		else if (output[0] == "Cell Height")
		{
			m_FontData.CellHeight = std::stoi(output[1]);
			continue;
		}
		else if (output[0] == "Start Char")
		{
			m_FontData.StartChar = std::stoi(output[1]);
			continue;
		}

		split(output[0], ' ', suboutput, 5);
		if (suboutput[0] == "Char" && std::stoi(suboutput[1]) <= 127 && suboutput[2] == "Base")
		{
			m_FontData.Widths[std::stoi(suboutput[1])] = std::stoi(output[1]);
		}
	}

	m_FontData.XCells = (int)m_FontData.Font.GetDimentions().x / m_FontData.CellWidth;
	m_FontData.YCells = (int)m_FontData.Font.GetDimentions().y / m_FontData.CellHeight;

}

void BMPTextRenderer::Draw(std::string text, float x, float y, int truncAfterDecimal, float scale, glm::vec3 color)
{
	// activate corresponding render state
	float originX = x;
	m_Shader->use();
	//m_Shader->setVec3("textColor", glm::vec3(1.0));
	m_FontData.Font.Bind();

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_ColorVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color), &color);

	truncAfterPoint(text, truncAfterDecimal);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		float xpos = x;
		float ypos = y;
		float w = m_FontData.CellWidth * scale;
		float h = m_FontData.CellHeight * scale;

		switch (*c)
		{
		case '\n':
			y += m_FontData.CellHeight;
			x = originX;
			continue;
			break;
			
		default:
			break;
		}

		int texX = (*c - m_FontData.StartChar) % m_FontData.XCells + 1;
		int texY = (*c - m_FontData.StartChar) / m_FontData.XCells + 1; // both have to be width
		float texXPos = ((texX - 1) * m_FontData.CellWidth) / m_FontData.Font.GetDimentions().x;
		float texYPos = ((texY - 1) * m_FontData.CellHeight) / m_FontData.Font.GetDimentions().y;
		float texW = (m_FontData.CellWidth / m_FontData.Font.GetDimentions().x);
		float texH = (m_FontData.CellHeight / m_FontData.Font.GetDimentions().y);

		// update VBO for each character
		Letter letter(
			xpos,			ypos + h,	texXPos,				texYPos + texH,
			xpos + w,	ypos,			texXPos + texW,	texYPos,
			xpos,			ypos,			texXPos,				texYPos,
			xpos,			ypos + h,	texXPos,				texYPos + texH,
			xpos + w,	ypos + h,	texXPos + texW,	texYPos + texH,
			xpos + w,	ypos,			texXPos + texW,	texYPos
		);
		m_LetterBuffer.push_back(letter);
		// render glyph texture over quad
		//m_FontData.Font.Bind();
		//glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(letter), &letter);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//// render quad
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		x += m_FontData.Widths[*c] * scale;
		//x += 32.f;
	}
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Letter) * m_LetterBuffer.size(), &m_LetterBuffer[0], GL_DYNAMIC_DRAW);

	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(6 * m_LetterBuffer.size()));

	m_LetterBuffer.clear();
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);	
}

void truncAfterPoint(std::string &str, int decimalPoints)
{
	// Check if decimalPoints is valid
	if (decimalPoints < 0)
		return;

	// Iterate through all string chars
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == '.') // if the char is a point
		{
			// Check if the bounds are valid
			if (i + decimalPoints > str.length())
				return;

			if (decimalPoints == 0)
			{
				str.erase(str.begin() + i, str.end());
				return;
			}

			// Erase after the specified decimal points
			str.erase(str.begin() + i + decimalPoints + 1, str.end()); 
			return;
		}
	}
}

int split(std::string inputString, char seperator, std::string output[], int size)
{
	if (inputString.length() == 0)
	{
		return 0;
	}
	int count = 0;
	char word[100] = {}; // word array, init'd to an arbitrary size of 100
	int wordCount = 0;

	for (int i = 0; i < inputString.length(); i++) // loop through inputString
	{
		if (inputString[i] == seperator)
		{
			if (count + 1 > size)
			{
				return -1; // check if output array is full
			}
			output[count] = std::string(word); // add word to output array
			count++;
			for (int j = 0; j < wordCount; j++)
			{
				word[j] = 0; // reset word
			}
			wordCount = 0; // reset word length counter
		}
		else
		{
			word[wordCount] = inputString[i]; // add char to word
			wordCount++;
		}
	}

	// add the last element
	if (count + 1 <= size) // check if array is not full
	{
		output[count] = std::string(word); // add word to output array
		count++;
	}
	else
	{
		return -1; // if full
	}

	return count;
}

Letter::Letter(float vertArray[6][4])
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			vertArray[i][j] = quad[i][j];
		}
	}
}

Letter::Letter(
	float x1, float y1, float u1, float v1,
	float x2, float y2, float u2, float v2,
	float x3, float y3, float u3, float v3,
	float x4, float y4, float u4, float v4,
	float x5, float y5, float u5, float v5,
	float x6, float y6, float u6, float v6
)
{
	quad[0][0] = x1;
	quad[0][1] = y1;
	quad[0][2] = u1;
	quad[0][3] = v1;
	quad[1][0] = x2;
	quad[1][1] = y2;
	quad[1][2] = u2;
	quad[1][3] = v2;
	quad[2][0] = x3;
	quad[2][1] = y3;
	quad[2][2] = u3;
	quad[2][3] = v3;
	quad[3][0] = x4;
	quad[3][1] = y4;
	quad[3][2] = u4;
	quad[3][3] = v4;
	quad[4][0] = x5;
	quad[4][1] = y5;
	quad[4][2] = u5;
	quad[4][3] = v5;
	quad[5][0] = x6;
	quad[5][1] = y6;
	quad[5][2] = u6;
	quad[5][3] = v6;
}
