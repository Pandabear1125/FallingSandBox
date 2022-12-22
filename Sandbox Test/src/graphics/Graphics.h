#ifndef SIMPLE_GRAPHICS_H
#define SIMPLE_GRAPHICS_H

#include "Shader.h"
#include "TextureAtlas.h"
#include <vector>

namespace Graphics {

	enum DrawMode
	{
		FILL,
		LINE
	};

	struct GraphicsData
	{
		unsigned int VAO = 0;
		unsigned int VBO = 0;
		unsigned int ColorVBO = 0;

		std::vector<float> BatchVector;
		std::vector<float> ColorVector;
		bool isBatched = false;
		unsigned int BatchCap = 0;
		unsigned int BatchSize = 0;
		
		Shader* PolygonShader = nullptr;
	};

	struct TextData
	{
		// graphics data
		unsigned int VAO = 0;
		unsigned int VBO = 0;
		unsigned int EBO = 0;
		unsigned int ColorVBO = 0;

		Shader* TextShader = nullptr;

		std::vector<float> Vertices;
		std::vector<unsigned int> Indices;
		unsigned int size = 0;
		unsigned int capacity = 10;

		// bmp font data
		Texture Font;
		int CellWidth = -1;
		int CellHeight = -1;
		unsigned char StartingChar = 0;
		int FontHeight = -1;
		int FontWidth = -1;
		int WidthInCells = -1;
		int HeightInCells = -1;

		std::vector<unsigned int> CharWidths;
		std::vector<glm::vec4> CharQuads;
	};

	// Initialize Graphics rendering data
	void GraphicsInit(Shader* polygonShader);

	// Initialize Text rendering data
	void TextInit(Shader* textShader, std::string bmpPath, std::string csvPath);

	// Draw a simple, convex polygon using "vertices" for vertex data.
	// If "mode" == Graphics::FILL, "vertices" is triangulated and draws a filled polygon. DATA MUST BE A CONVEX, SIMPLE POLYGON for defined behavior.
	// If "mode" == Graphics::LINE, "vertices" is drawn as is and draws the outline of the polygon. Does not have to be convex, simple.
	void Polygon(DrawMode mode, float *vertices, unsigned int vertexCount, float red = 1.f, float green = 1.f, float blue = 1.f);

	// Draw an approximated circle at "x, y" with a certain "radius" and "r, g, b" color.
	// Circle is just an approximated uniform array of points around "x, y".
	// "precision" controls how many points are used, less points are chunkier but more performant.
	// Graphics::FILL for "mode" draws a filled circle, Graphics::LINE draws an outlined circle.
	void Circle(DrawMode mode, float x, float y, float radius, float red = 1.f, float green = 1.f, float blue = 1.f, unsigned int precision = 16);

	// Draw a rectangle from "x, y" to "x + width, y + height" at a certain "r, g, b" color.
	// "rotation" given in radians determines rotation around "x, y" plus "rotOffsetX, rotOffsetY".
	// Graphics::FILL for "mode" draws a filled rectangle, Graphics::LINE draws an outlined rectangle.
	void Rectangle(DrawMode mode, float x, float y, float width, float height, float rotation = 0.f, float rotOffsetX = 0.f, float rotOffsetY = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);

	// Enable line batching.
	void BatchLinesPush();

	// Draw a line from (x1, y1) to (x2, y2).
	void Line(float x1, float y1, float x2, float y2);

	// Disable line batching and issue the draw call for all Graphics::Line() calls after the last Graphics::BatchLinesPush().
	void BatchLinesPop();

	// Enable point batching.
	void BatchPointsPush();

	// Draw a point of a certain "size" radius in pixels at "x, y" of a certain "r, g, b" color.
	// "size" is unused if this point is being batched.
	void Point(float x, float y, unsigned int size = 1, float red = 1.f, float green = 1.f, float blue = 1.f);

	// Disable point batching and issue the draw call for all Graphics::Point() calls after the last Graphics::BatchPointsPush().
	// "pointSize" is the pixel size in radius.
	void BatchPointsPop(unsigned int pointSize = 1);

	// Internal initial overloaded print function. 
	void PrintInternal(std::string text, float x, float y, float scale = 1.f, float red = 1.f, float green = 1.f, float blue = 1.f, float rotation = 0.f, int digitTruncate = -1);
	
	// Draws "text" to "x, y" scaled to a certain "scale" with a certain "r, g, b" color with a specific "rotation" around "x, y".
	void Print(std::string text, float x, float y, float scale = 1.f, float red = 1.f, float green = 1.f, float blue = 1.f, float rotation = 0.f);
	// Draws "text" to "x, y" scaled to a certain "scale" with a certain "r, g, b" color with a specific "rotation" around "x, y".
	void Print(const char* text, float x, float y, float scale = 1.f, float red = 1.f, float green = 1.f, float blue = 1.f, float rotation = 0.f);
	// Draws "text" to "x, y" scaled to a certain "scale" with a certain "r, g, b" color with a specific "rotation" around "x, y".
	// "digitTruncate" controls whether the float/double should be cut off after "digitTruncate" digits after the point.
	void Print(float text, float x, float y, float scale = 1.f, float red = 1.f, float green = 1.f, float blue = 1.f, float rotation = 0.f, int digitTruncate = -1);
	// Draws "text" to "x, y" scaled to a certain "scale" with a certain "r, g, b" color with a specific "rotation" around "x, y".
	// "digitTruncate" controls whether the float/double should be cut off after "digitTruncate" digits after the point.
	void Print(double text, float x, float y, float scale = 1.f, float red = 1.f, float green = 1.f, float blue = 1.f, float rotation = 0.f, int digitTruncate = -1);
	// Draws "text" to "x, y" scaled to a certain "scale" with a certain "r, g, b" color with a specific "rotation" around "x, y".
	void Print(int text, float x, float y, float scale = 1.f, float red = 1.f, float green = 1.f, float blue = 1.f, float rotation = 0.f);
	// Draws "text" to "x, y" scaled to a certain "scale" with a certain "r, g, b" color with a specific "rotation" around "x, y".
	void Print(bool text, float x, float y, float scale = 1.f, float red = 1.f, float green = 1.f, float blue = 1.f, float rotation = 0.f);

	/// <summary>
	/// Attempts to triangulate a SIMPLE, CONVEX polygon. It will return unknown values if polygon does not fit these rules
	/// </summary>
	/// <param name="vertices"> | simple float array of pairs of x,y coords</param>
	/// <param name="vertexCount"> | count of VERTICES (pairs of values), not values in the array</param>
	/// <returns> | float vector of 3 pairs of coords to make triangles</returns>
	std::vector<float> TriangulatePolygon(float* vertices, unsigned int vertexCount);

	int split(std::string inputString, char seperator, std::string output[], int size);

	void TruncateDigits(std::string& text, int digits);

}	// namespace Graphics

#endif