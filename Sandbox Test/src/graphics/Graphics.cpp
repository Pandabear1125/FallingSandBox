#include "Graphics.h"

namespace Graphics {

	// basic vertices that are reused/transformed
	static const float QuadVertices[16] = {
		 0.f,  0.f,  0.0f,  0.0f, // 0
		 0.f,  1.f,  0.0f,  1.0f, // 1
		 1.f,  0.f,  1.0f,  0.0f, // 2
		 1.f,  1.f,  1.0f,  1.0f  // 3
	};

	TextData tData;

	GraphicsData gData;

	void GraphicsInit(Shader* polygonShader)
	{
		// Init shader
		gData.PolygonShader = polygonShader;
		
		// Create vertex array
		glGenVertexArrays(1, &gData.VAO);

		// Create vbo
		glGenBuffers(1, &gData.VBO);

		// Bind vertex array
		glBindVertexArray(gData.VAO);

		// Bind and set up vbo
		glBindBuffer(GL_ARRAY_BUFFER, gData.VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 2, NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);
		glEnableVertexAttribArray(0);

		// Create color vbo
		glGenBuffers(1, &gData.ColorVBO);

		// bind and set up color vbo
		glBindBuffer(GL_ARRAY_BUFFER, gData.ColorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3, NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
		glEnableVertexAttribArray(1);

		// Clean up and unbind everything
		glBindVertexArray(0);
	}

	void TextInit(Shader* textShader, std::string bmpPath, std::string csvPath)
	{
		// generate buffers
		glGenVertexArrays(1, &tData.VAO);
		glGenBuffers(1, &tData.VBO);
		glGenBuffers(1, &tData.ColorVBO);
		glGenBuffers(1, &tData.EBO);

		// bind vao
		glBindVertexArray(tData.VAO);

		// configure buffers
		glBindBuffer(GL_ARRAY_BUFFER, tData.VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 16 * tData.capacity, NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, tData.ColorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3, NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribDivisor(1, 1);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tData.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_UNSIGNED_INT) * 6 * tData.capacity, NULL, GL_DYNAMIC_DRAW);

		// unbind vao
		glBindVertexArray(0);

		// assign shader
		tData.TextShader = textShader;

		// resize vectors for initial size
		tData.Vertices.resize(16 * tData.capacity);
		tData.Indices.resize(6 * tData.capacity);
		tData.CharQuads.resize(256);
		tData.CharWidths.resize(256);

		// read from font files and assemble tData values
		std::ifstream fin;
		fin.open(csvPath);
		std::string line;
		std::string output[5];
		std::string suboutput[5];

		while (std::getline(fin, line)) // read through whole file
		{
			split(line, ',', output, 5); // process line

			// assign the result to the correct tData value
			if (output[0] == "Cell Width")
			{
				tData.CellWidth = std::stoi(output[1]);
				continue;
			}
			else if (output[0] == "Cell Height")
			{
				tData.CellHeight = std::stoi(output[1]);
				continue;
			}
			else if (output[0] == "Start Char")
			{
				tData.StartingChar = std::stoi(output[1]);
				continue;
			}
			// if this line is a char width field, process again
			split(output[0], ' ', suboutput, 5);
			if (suboutput[0] == "Char" && suboutput[2] == "Base")
			{
				tData.CharWidths[std::stoi(suboutput[1])] = std::stoi(output[1]);
			}
		}


		// configure texture and atlas
		tData.Font = Texture(bmpPath.c_str(), true);

		tData.WidthInCells = (int)tData.Font.GetDimentions().x / tData.CellWidth;
		tData.HeightInCells = (int)tData.Font.GetDimentions().y / tData.CellHeight;

		TexAtlas atlas(&tData.Font, tData.WidthInCells, tData.HeightInCells);

		// pre assemble quads
		for (unsigned int i = tData.StartingChar; i < 256; i++)
		{
			tData.CharQuads[i] = (atlas.GetQuad(i - tData.StartingChar));
		}
	}

	void Polygon(DrawMode mode, float *vertices, unsigned int vertexCount, float red, float green, float blue)
	{
		unsigned int triangleCount;
		// Bind vao
		glBindVertexArray(gData.VAO);

		// Fill vbo with vertex data
		glBindBuffer(GL_ARRAY_BUFFER, gData.VBO);
		if (mode == FILL)
		{	// FILL mode needs to triangulate the polygon
			// get vertex data from given polygon by triangulating it
			std::vector<float> vertexData = TriangulatePolygon(vertices, vertexCount);
			triangleCount = (unsigned int)vertexData.size() / 6;
			// resize and fill vert buffer with triangulated vertex data
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * vertexData.size(), vertexData.data(), GL_DYNAMIC_DRAW);
			// bind and turn on vertex attrib divisor
			glBindBuffer(GL_ARRAY_BUFFER, gData.ColorVBO);
			glVertexAttribDivisor(1, 1);

			// assign and fill color buffer
			float color[3] = { red, green, blue };
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3, color, GL_DYNAMIC_DRAW);
		}
		else
		{ // LINE mode only plugs in the vertex data as is
			// bind vbo and fill data
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * vertexCount * 2, vertices, GL_DYNAMIC_DRAW);
			// bind and turn on vertex attrib divisor
			glBindBuffer(GL_ARRAY_BUFFER, gData.ColorVBO);
			glVertexAttribDivisor(1, 1);

			float color[3] = { red, green, blue };
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3, color, GL_DYNAMIC_DRAW);
		}

		// Bind shader
		gData.PolygonShader->use();

		// Call draw command
		if (mode == FILL)
		{
			glDrawArrays(GL_TRIANGLES, 0, triangleCount*3);
		}
		else 
			glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
	}

	void Circle(DrawMode mode, float x, float y, float radius, float red, float green, float blue, unsigned int precision)
	{
		// Create needed array of floats with correct size
		float* vertices = new float[precision * 2];
		
		// Initialize needed variables
		unsigned int count = 0;
		float dx = 0.f;
		float dy = 0.f;
		float angle = 0.f;
		// angle increment calculated through number of degrees per side
		float angleInc = glm::radians(360.f / (float)precision);

		// Loop through all points needed to be created
		for (unsigned int i = 0; i < precision; i++)
		{
			// increase the angle
			angle += angleInc;
			// find the value of x and y
			dx = cos(angle) * radius + x;
			dy = sin(angle) * radius + y;

			// add x and y to the vertex array at the correct place
			vertices[count] = dx;
			count++;
			vertices[count] = dy;
			count++;
		}

		// use polygon to draw
		Polygon(mode, vertices, precision, red, green, blue);

		// clean up new allocated vertex array
		delete[] vertices;
	}

	void Rectangle(DrawMode mode, float x, float y, float width, float height, float rotation, float rotOffsetX, float rotOffsetY, float red, float green, float blue)
	{
		// Assemble vertex positions
		float Vertices[8] = {
			x			, y,			  // top left
			x + width, y,			  // top right
			x + width, y + height, // bottom right
			x			, y + height  // bottom left
		};


		// Rotate vertex positions
		if (rotation != 0)
		{
			// initialize sin, cos values for performace
			float sinX = sin(rotation);
			float cosX = cos(rotation);
			
			for (int i = 0; i < 8; i+=2)
			{
				float origX = Vertices[i];
				float origY = Vertices[i + 1];
				// rotate vertices
				Vertices[i] = cosX * (origX - x - rotOffsetX) - sinX * (origY - y - rotOffsetY) + x + rotOffsetX;
				Vertices[i + 1] = sinX * (origX - x - rotOffsetX) + cosX * (origY - y - rotOffsetY) + y + rotOffsetY;
			}
		}

		// use polygon to draw
		Polygon(mode, Vertices, 4, red, green, blue);
	}

	void BatchLinesPush()
	{
		gData.isBatched = true;
	}

	void Line(float x1, float y1, float x2, float y2)
	{
		if (gData.isBatched)
		{
			gData.BatchVector.push_back(x1);
			gData.BatchVector.push_back(y1);
			gData.BatchVector.push_back(x2);
			gData.BatchVector.push_back(y2);
		}
		else
		{
			// collect input points into an array
			float* vertices = new float[4];
			vertices[0] = x1;
			vertices[1] = y1;
			vertices[2] = x2;
			vertices[3] = y2;

			// bind VAO
			glBindVertexArray(gData.VAO);
			
			// bind VBO
			glBindBuffer(GL_ARRAY_BUFFER, gData.VBO);
			// fill vbo with data
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4, vertices, GL_DYNAMIC_DRAW);

			// clean up vertex array
			delete[] vertices;

			// bind Shader
			gData.PolygonShader->use();

			// draw call
			glDrawArrays(GL_LINES, 0, 2);
		}
	}

	void BatchLinesPop()
	{
		// bind VAO
		glBindVertexArray(gData.VAO);

		// bind VBO
		glBindBuffer(GL_ARRAY_BUFFER, gData.VBO);
		// fill vbo data
		glBufferData(GL_ARRAY_BUFFER, gData.BatchVector.size() * sizeof(float), gData.BatchVector.data(), GL_DYNAMIC_DRAW);

		// bind Shader
		gData.PolygonShader->use();

		// draw call
		glDrawArrays(GL_LINES, 0, (GLsizei)gData.BatchVector.size());
		gData.BatchVector.clear();

		// reset batch flag
		gData.isBatched = false;
	}

	void BatchPointsPush()
	{
		// reset Batch state
		gData.BatchSize = 0;
		gData.isBatched = true;

		glBindVertexArray(gData.VAO);
		// bind color vbo and set it to not use vertex attrib division
		// vertex attrib division doesn't seem to play nice with batched points
		glBindBuffer(GL_ARRAY_BUFFER, gData.ColorVBO);
		glVertexAttribDivisor(1, 0);

		if (gData.BatchCap == 0)
		{
			// resize batch capacity and vector size
			gData.BatchCap += 10;
			gData.BatchVector.resize(gData.BatchCap * 2);
			gData.ColorVector.resize(gData.BatchCap * 3);

			// resize vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, gData.VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * gData.BatchCap * 2, NULL, GL_DYNAMIC_DRAW);
			// resize color buffer
			glBindBuffer(GL_ARRAY_BUFFER, gData.ColorVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3 * gData.BatchCap, NULL, GL_DYNAMIC_DRAW);
		}
		
		// reset vert vector
		gData.BatchVector.clear();
		gData.BatchVector.resize(gData.BatchCap * 2);
		// reset color vector
		gData.ColorVector.clear();
		gData.ColorVector.resize(gData.BatchCap * 3);
	}

	void Point(float x, float y, unsigned int size, float red, float green, float blue)
	{
		// bind vao
		glBindVertexArray(gData.VAO);

		if (gData.isBatched)
		{
			if (gData.BatchSize >= gData.BatchCap)
			{	// resize vector/buffer
				gData.BatchCap += 10;
				// vector resize
				gData.BatchVector.resize(gData.BatchCap * 2);
				gData.ColorVector.resize(gData.BatchCap * 3);

				// vbo resize
				glBindBuffer(GL_ARRAY_BUFFER, gData.VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 2 * gData.BatchCap, NULL, GL_DYNAMIC_DRAW);
				// color vbo resize
				glBindBuffer(GL_ARRAY_BUFFER, gData.ColorVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3 * gData.BatchCap, NULL, GL_DYNAMIC_DRAW);
			}

			// verify that the buffer size is still valid
			// multiple other Graphics calls use the same vertex and color buffer, resizing it
			// check the actual size of the buffer, if it's too small than expected
			// full resize both buffers
			int size = 0;
			glBindBuffer(GL_ARRAY_BUFFER, gData.VBO);
			// get buffer size
			glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
			if (size < sizeof(GL_FLOAT) * 2 * gData.BatchCap)
			{	// resize
				glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 2 * gData.BatchCap, NULL, GL_DYNAMIC_DRAW);
				
				glBindBuffer(GL_ARRAY_BUFFER, gData.ColorVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3 * gData.BatchCap, NULL, GL_DYNAMIC_DRAW);
			}

			// add vertex data
			gData.BatchVector[gData.BatchSize * 2] = x;
			gData.BatchVector[gData.BatchSize * 2 + 1] = y;
			// add color data
			gData.ColorVector[gData.BatchSize * 3] = red;
			gData.ColorVector[gData.BatchSize * 3 + 1] = green;
			gData.ColorVector[gData.BatchSize * 3 + 2] = blue;

			gData.BatchSize++;
		}
		else
		{
			float point[2] = { x, y };
			// bind and fill vert data
			glBindBuffer(GL_ARRAY_BUFFER, gData.VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 2, point, GL_DYNAMIC_DRAW);

			float color[3] = { red, green, blue };
			// bind and fill color data
			glBindBuffer(GL_ARRAY_BUFFER, gData.ColorVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3, color, GL_DYNAMIC_DRAW);

			// set point size
			glPointSize((float)size);
			// bind shader
			gData.PolygonShader->use();
			// issue draw call
			glDrawArrays(GL_POINTS, 0, 1);
		}
	}

	void BatchPointsPop(unsigned int pointSize)
	{
		// bind vao
		glBindVertexArray(gData.VAO);

		if (gData.isBatched)
		{
			// bind vbo and fill data
			glBindBuffer(GL_ARRAY_BUFFER, gData.VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * 2 * gData.BatchSize, gData.BatchVector.data());

			// bind color vbo and fill data
			glBindBuffer(GL_ARRAY_BUFFER, gData.ColorVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * 3 * gData.BatchSize, gData.ColorVector.data());

			// set point size
			glPointSize((float)pointSize);
			// bind shader
			gData.PolygonShader->use();
			// issue draw call for BatchSize points
			glDrawArrays(GL_POINTS, 0, gData.BatchSize);
		}
		else
		{
			// Push was not called before this
			assert(false);
		}
	}

	void PrintInternal(std::string text, float x, float y, float scale, float red, float green, float blue, float rotation, int digitTruncate)
	{
		TruncateDigits(text, digitTruncate);

		float thetaSin = sin(glm::radians(rotation));
		float thetaCos = cos(glm::radians(rotation));

		float charOffsetX = 0.f;
		float charOffsetY = 0.f;

		float VertData[16];
		for (unsigned int j = 0; j < text.length(); j++)
		{
			unsigned char ch = (unsigned char)text[j];
			glm::vec4 quad = tData.CharQuads[ch];
			for (int i = 0; i < 16; i++)
			{
				switch (i % 4)
				{
				case 0:
					// Rotate
					VertData[i] = QuadVertices[i] * thetaCos - QuadVertices[i + 1] * thetaSin;
					// Translate
					VertData[i] = VertData[i] * (tData.CellWidth * scale) + x + charOffsetX;
					break;
				case 1:
					// Rotate
					VertData[i] = QuadVertices[i - 1] * thetaSin + QuadVertices[i] * thetaCos;
					// Translate
					VertData[i] = VertData[i] * (tData.CellHeight * scale) + y + charOffsetY;
					break;
				case 2:
					// Set UV Coord - U
					VertData[i] = (quad.z * QuadVertices[i] + quad.x) / tData.Font.GetDimentions().x;
					break;
				case 3:
					// Set UV Coord - V
					VertData[i] = (quad.w * QuadVertices[i] + quad.y) / tData.Font.GetDimentions().y;
					break;
				default:
					break;
				}
			}

			for (unsigned int i = 0; i < 16; i++)
			{
				tData.Vertices[tData.size * 16 + i] = VertData[i];
			}

			tData.Indices[tData.size * 6 + 0] = (0 + (4 * tData.size));
			tData.Indices[tData.size * 6 + 1] = (1 + (4 * tData.size));
			tData.Indices[tData.size * 6 + 2] = (3 + (4 * tData.size));
			tData.Indices[tData.size * 6 + 3] = (3 + (4 * tData.size));
			tData.Indices[tData.size * 6 + 4] = (2 + (4 * tData.size));
			tData.Indices[tData.size * 6 + 5] = (0 + (4 * tData.size));

			tData.size++;

			charOffsetX += (tData.CharWidths[ch] * scale) * thetaCos;
			charOffsetY += (tData.CharWidths[ch] * scale) * thetaSin;

			if (tData.size >= tData.capacity)
			{
				tData.capacity += 10;
				tData.Vertices.resize(16 * tData.capacity);
				tData.Indices.resize(6 * tData.capacity);

				glBindVertexArray(tData.VAO);
				glBindBuffer(GL_ARRAY_BUFFER, tData.VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 16 * tData.capacity, NULL, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tData.EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_UNSIGNED_INT) * 6 * tData.capacity, NULL, GL_DYNAMIC_DRAW);
				glBindVertexArray(0);
			}
		}

		float Color[3] = { red, green, blue };

		// configure buffers with data
		glBindVertexArray(tData.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, tData.VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * 16 * tData.size, tData.Vertices.data());

		glBindBuffer(GL_ARRAY_BUFFER, tData.ColorVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * 3, Color);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tData.EBO);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GL_UNSIGNED_INT) * 6 * tData.size, tData.Indices.data());

		// bind shader
		tData.TextShader->use();

		// bind texture
		tData.Font.Bind();

		// issue draw call
		glDrawElements(GL_TRIANGLES, tData.size * 6, GL_UNSIGNED_INT, 0);

		tData.size = 0;
	}

	void Print(std::string text, float x, float y, float scale, float red, float green, float blue, float rotation)
	{
		PrintInternal(text, x, y, scale, red, green, blue, rotation);
	}

	void Print(const char* text, float x, float y, float scale, float red, float green, float blue, float rotation)
	{
		PrintInternal(std::string(text), x, y, scale, red, green, blue, rotation);
	}

	void Print(float text, float x, float y, float scale, float red, float green, float blue, float rotation, int digitTruncate)
	{
		PrintInternal(std::to_string(text), x, y, scale, red, green, blue, rotation, digitTruncate);
	}

	void Print(double text, float x, float y, float scale, float red, float green, float blue, float rotation, int digitTruncate)
	{
		PrintInternal(std::to_string(text), x, y, scale, red, green, blue, rotation, digitTruncate);
	}

	void Print(int text, float x, float y, float scale, float red, float green, float blue, float rotation)
	{
		PrintInternal(std::to_string(text), x, y, scale, red, green, blue, rotation);
	}

	void Print(bool text, float x, float y, float scale, float red, float green, float blue, float rotation)
	{
		if (text)
			PrintInternal("True", x, y, scale, red, green, blue, rotation);
		else 
			PrintInternal("False", x, y, scale, red, green, blue, rotation);
	}

	std::vector<float> TriangulatePolygon(float *vertices, unsigned int vertexCount)
	{
		std::vector<float> triangleList;
		if (vertexCount < 3)
			return triangleList;

		if (vertexCount == 3)
		{
			triangleList.push_back(vertices[0]); // vertex 0
			triangleList.push_back(vertices[1]);
			triangleList.push_back(vertices[2]); // vertex 1
			triangleList.push_back(vertices[3]);
			triangleList.push_back(vertices[4]); // vertex 2
			triangleList.push_back(vertices[5]);
			return triangleList;
		}

		for (unsigned int i = 1; i <= vertexCount - 2; i++)
		{
			triangleList.push_back(vertices[0]);		// core point
			triangleList.push_back(vertices[1]);
			triangleList.push_back(vertices[i * 2]);		// next point according to the triangle index i
			triangleList.push_back(vertices[i * 2 + 1]);
			triangleList.push_back(vertices[i * 2 + 2]);	// next clockwise point from index i
			triangleList.push_back(vertices[i * 2 + 3]);
		}

		return triangleList;
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

	void TruncateDigits(std::string& text, int digits)
	{
		if (digits < 0) // if digits is negative, dont do anything
			return;

		// loop through string
		for (unsigned int i = 0; i < text.length(); i++)
		{
			if (text[i] == '.') // find the point
			{
				if (digits == 0) // if input is 0, strip the point
					text.erase(text.begin() + i, text.end());
				else
				{	// if not, strip everything after the (point + digits)
					text.erase(text.begin() + i + 1 + digits, text.end());
				}
			}
		}
	}

} // end graphics namespace