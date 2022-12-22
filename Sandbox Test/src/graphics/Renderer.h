#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <unordered_map>

#include "Texture.h"
#include "Shader.h"
#include "TextureAtlas.h"

static const unsigned int BATCH_INITIAL_CAPACITY = 10;

class Batch
{
public:
	// initial constructor
	Batch(TexAtlas* atlas);

	// resize function to increase the capacity of the vectors
	void Resize(unsigned int amount);
	// final cleaning function to return vectors to initial state
	void Cleanup();

	// function to add a quad
	void AddQuad(float x, float y, float width, float height, unsigned int quadIndex, float rotation = 0.f, float rotOffsetX = 0.f, float rotOffsetY = 0.f);

	// draw call, calls cleanup
	void Draw(Shader* shader);

public:
	// stored atlas
	TexAtlas* atlas = nullptr;

private:
	// graphics data
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	// vector data
	unsigned int size = 0;									// current count of quads in vectors
	unsigned int capacity = BATCH_INITIAL_CAPACITY;	// max num of quads before seg fault
	// data vectors
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

};

/// <summary>
/// Renderer class that batches dynamic data based on texture.
/// Allows drawing of unlimited textures.
/// </summary>
class MultiRender
{
public:
	MultiRender() = default;
	MultiRender(Shader* mainShader);

	/// <summary>
	/// Add a quad to be drawn from a given atlas and index. Center origin at (x,y)
	/// </summary>
	/// <param name="x"> | x coord for center </param>
	/// <param name="y"> | y coord for center </param>
	/// <param name="width"> | width</param>
	/// <param name="height"> | height</param>
	/// <param name="atlas"> | TexAtlas pointer to atlas. Used to batch many quads together</param>
	/// <param name="frameIndex"> | index to frame of atlas to be drawn </param>
	/// <param name="rotation"> | rotation in degrees </param>
	/// <param name="rotOffsetX"> | x offset for rotation point </param>
	/// <param name="rotOffsetY"> | y offset for rotation point </param>
	void AddQuad(float x, float y, float width, float height, TexAtlas* atlas, unsigned int frameIndex, float rotation = 0.f, float rotOffsetX = 0.f, float rotOffsetY = 0.f);

	// Draw all added quads in the frame. Clears data when called
	void Draw();

private:
	std::unordered_map<TexAtlas*, Batch*> m_BatchMap;

	Shader* m_Shader = nullptr;

};



// OLD RENDERER class



class Renderer
{
public:
	Renderer(Shader* staticShader, Shader* dynamicShader, Texture* texture);

	/// <summary>
	/// Set the current texture to given newTexture reference. 
	/// </summary>
	/// <param name="newTexture">New texture reference to use</param>
	void SetCurrentTexture(Texture* newTexture);
	Texture* GetCurrentTexture();

	/// <summary>
	/// Add a dynamic quad to be drawn
	/// </summary>
	/// <param name="x">float</param>
	/// <param name="y">float</param>
	/// <param name="width">float</param>
	/// <param name="height">float</param>
	/// <param name="texQuad">vec4: (x, y, width, height)</param>
	/// <param name="rotation">float: degrees</param>
	void AddDynamic(float x, float y, float width, float height, glm::vec4 texQuad, float rotation = 0.f/*, bool animated = false, AnimationData animData = AnimationData(0, 0, 0, 0)*/);
	
	/// <summary>
	/// Add a static quad to be drawn
	/// </summary>
	/// <param name="x">float</param>
	/// <param name="y">float</param>
	/// <param name="width">float</param>
	/// <param name="height">float</param>
	/// <param name="texQuad">vec4: (x, y, width, height)</param>
	/// <param name="rotation">float: degrees</param>
	void AddStatic(float x, float y, float width, float height, glm::vec4 texQuad, float rotation = 0.f);

	// Actual draw call to render all previously added quads.
	// Dynamic quads are deleted after this call. Need to be re-added
	void Render();

private:
	// Render state
	unsigned int m_DynamicVAO, m_DynamicVBO, m_DynamicEBO;
	unsigned int m_StaticVAO, m_StaticVertVBO, m_StaticTransVBO, m_StaticEBO, m_StaticTexVBO;
	
	// Render data
	std::vector<unsigned int> m_DynamicIndices;
	std::vector<float> m_DynamicVertices;

	std::vector<glm::mat4> m_Transforms;
	std::vector<float> m_TexTransforms;

	unsigned int m_DynamicCount = 0;
	unsigned int m_DynamicCapacity = 0;

	// Shader references	
	Shader* m_ShaderStatic;
	Shader* m_ShaderDynamic;

	Texture* m_CurrentTexture;
};


#endif