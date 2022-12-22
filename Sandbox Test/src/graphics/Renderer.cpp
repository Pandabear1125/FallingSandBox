#include "Renderer.h"

// basic vertices that are reused/transformed
static const float QuadVertices[16] = {
	-0.5f, -0.5f, 0.0f, 0.0f, // 0
	-0.5f,  0.5f, 0.0f, 1.0f, // 1
	 0.5f, -0.5f, 1.0f, 0.0f, // 2
	 0.5f,  0.5f, 1.0f, 1.0f  // 3
};

// basic indices that connect with the base vertices ^
static const unsigned int QuadIndices[6] = {
	0, 1, 3,   3, 2, 0
};

Batch::Batch(TexAtlas* _atlas)
{
	// initialize graphic buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// bind vao
	glBindVertexArray(VAO);

	// set up buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 16 * BATCH_INITIAL_CAPACITY, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_UNSIGNED_INT) * 6 * BATCH_INITIAL_CAPACITY, NULL, GL_DYNAMIC_DRAW);

	// unbind vao
	glBindVertexArray(0);

	// configure data vectors
	vertices.resize(BATCH_INITIAL_CAPACITY * 16);
	indices.resize(BATCH_INITIAL_CAPACITY * 6);

	// store atlas pointer
	atlas = _atlas;
}

void Batch::Resize(unsigned int amount)
{
	capacity += amount;
	vertices.resize(capacity * 16);
	indices.resize(capacity * 6);
}

void Batch::Cleanup()
{
	vertices.clear();
	vertices.resize(capacity * 16);
	indices.clear();
	indices.resize(capacity * 6);

	size = 0;
}

void Batch::AddQuad(float x, float y, float width, float height, unsigned int quadIndex, float rotation, float rotOffsetX, float rotOffsetY)
{
	// create quad
	glm::vec4 texQuad = atlas->GetQuad(quadIndex);

	// Rotate vertices
	float sinX = sin(glm::radians(rotation));
	float cosX = cos(glm::radians(rotation));

	float Vertices[16] = {
		x,				y,				0.0f, 0.0f,  // 0
		x,	 			y + height, 0.0f, 1.0f,	 // 1
		x + width,	y,				1.0f, 0.0f,  // 2
		x + width,	y + height, 1.0f, 1.0f   // 3
	};

	glm::vec2 texDimens = atlas->GetTexture()->GetDimentions();

	for (unsigned int i = 0; i < 16; i+=4)
	{
		// rotation
		float origX = Vertices[i];
		float origY = Vertices[i + 1];
		Vertices[i] = cosX * (origX - x - rotOffsetX) - sinX * (origY - y - rotOffsetY) + x + rotOffsetX;
		Vertices[i + 1] = sinX * (origX - x - rotOffsetX) + cosX * (origY - y - rotOffsetY) + y + rotOffsetY;

		// texture uv
		float origU = Vertices[i + 2];
		float origV = Vertices[i + 3];
		Vertices[i + 2] = (texQuad.z * origU + texQuad.x) / texDimens.x;
		Vertices[i + 3] = (texQuad.w * origV + texQuad.y) / texDimens.y;
	}

	//// Set UV Coord - U
	//VertData[i] = (texQuad.z * QuadVertices[i] + texQuad.x) / atlas->GetTexture()->GetDimentions().x;
	//// Set UV Coord - V
	//VertData[i] = (texQuad.w * QuadVertices[i] + texQuad.y) / atlas->GetTexture()->GetDimentions().y;

	// Resize buffer if needed
	if (size >= capacity)
	{
		Resize(10);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16 * capacity, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * capacity, NULL, GL_DYNAMIC_DRAW);
	}

	// Add vertex and index data to respective vector
	for (int i = 0; i < 16; i++)
	{
		vertices[size * 16 + i] = Vertices[i];
	}

	indices[size * 6 + 0] = (0 + (4 * size));
	indices[size * 6 + 1] = (1 + (4 * size));
	indices[size * 6 + 2] = (3 + (4 * size));
	indices[size * 6 + 3] = (3 + (4 * size));
	indices[size * 6 + 4] = (2 + (4 * size));
	indices[size * 6 + 5] = (0 + (4 * size));

	// increment count
	size++;
}

void Batch::Draw(Shader* shader)
{
	// bind VAO
	glBindVertexArray(VAO);
	
	// bind shader
	shader->use();

	// fill buffers with data from data vectors
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * 16 * size, vertices.data());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GL_UNSIGNED_INT) * 6 * size, indices.data());

	// bind texture
	atlas->GetTexture()->Bind();

	// issue draw call
	glDrawElements(GL_TRIANGLES, size * 6, GL_UNSIGNED_INT, 0);

	// clean up batch vectors
	Cleanup();
}

MultiRender::MultiRender(Shader* mainShader)
{
	m_Shader = mainShader;
}

void MultiRender::AddQuad(float x, float y, float width, float height, TexAtlas* atlas, unsigned int frameIndex, float rotation, float rotOffsetX, float rotOffsetY)
{
	// check if the batch connected to the atlas exists in the map
	auto it = m_BatchMap.find(atlas);
	if (it == m_BatchMap.end())
	{  // if this atlas DOES NOT exist in the map
		// create new batch
		Batch* batch = new Batch(atlas);

		// add quad to new batch
		batch->AddQuad(x, y, width, height, frameIndex, rotation, rotOffsetX, rotOffsetY);

		// add batch to map
		m_BatchMap[atlas] = batch;
	}
	else
	{  // if this atlas DOES exist
		it->second->AddQuad(x, y, width, height, frameIndex, rotation, rotOffsetX, rotOffsetY);
	}
}

void MultiRender::Draw()
{
	for (auto const& bmap : m_BatchMap)
	{
		bmap.second->Draw(m_Shader);
	}
}







Renderer::Renderer(Shader* staticShader, Shader* dynamicShader, Texture* texture)
	: m_ShaderStatic(staticShader), m_ShaderDynamic(dynamicShader), m_CurrentTexture(texture)
{
	//// STATIC DATA:
	// 
	// Generate all needed buffers
	glGenVertexArrays(1, &m_StaticVAO);
	glGenBuffers(1, &m_StaticVertVBO);
	glGenBuffers(1, &m_StaticTransVBO);
	glGenBuffers(1, &m_StaticEBO);
	glGenBuffers(1, &m_StaticTexVBO);

	// Bind VAO
	glBindVertexArray(m_StaticVAO);
	
	// Bind and set up vertex VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_StaticVertVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);
	glEnableVertexAttribArray(0);

	// Bind and set up transform VBO
	// Transform vbo is a 4x4 float matrix
	glBindBuffer(GL_ARRAY_BUFFER, m_StaticTransVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 16, (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 16, (void*)(4 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 16, (void*)(8 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 16, (void*)(12 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(4);
	glVertexAttribDivisor(4, 1);

	// Build and set up UV coord VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_StaticTexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 2, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);
	glEnableVertexAttribArray(5);
	glVertexAttribDivisor(5, 1);

	// Bind and set up EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_StaticEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QuadIndices), QuadIndices, GL_STATIC_DRAW);

	// Unbind everything
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	////DYNAMIC DATA:
	//
	// Generate all needed buffers
	glGenVertexArrays(1, &m_DynamicVAO);
	glGenBuffers(1, &m_DynamicVBO);
	glGenBuffers(1, &m_DynamicEBO);

	// Bind VAO
	glBindVertexArray(m_DynamicVAO);

	// Bind and set up VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_DynamicVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_DynamicVertices.size(), m_DynamicVertices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void*)0);
	glEnableVertexAttribArray(0);

	// Bind and set up EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_DynamicEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_DynamicIndices.size(), m_DynamicIndices.data(), GL_DYNAMIC_DRAW);

	// Unbind everything
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Renderer::SetCurrentTexture(Texture* newTexture)
{
	m_CurrentTexture = newTexture;
}

Texture* Renderer::GetCurrentTexture()
{
	return m_CurrentTexture;
}

void Renderer::AddDynamic(float x, float y, float width, float height, glm::vec4 texQuad, float rotation)
{
	// Rotate vertices
	float thetaSin = sin(glm::radians(rotation));
	float thetaCos = cos(glm::radians(rotation));

	float VertData[16];
	for (int i = 0; i < 16; i++)
	{
		switch (i % 4)
		{
		case 0:
			// Rotate
			VertData[i] = QuadVertices[i] * thetaCos - QuadVertices[i + 1] * thetaSin;
			// Translate
			VertData[i] = VertData[i] * width + x;
			break;
		case 1:
			// Rotate
			VertData[i] = QuadVertices[i-1] * thetaSin + QuadVertices[i] * thetaCos;
			// Translate
			VertData[i] = VertData[i] * height + y;
			break;
		case 2:
			// Set UV Coord - U
			VertData[i] = (texQuad.z * QuadVertices[i] + texQuad.x) / m_CurrentTexture->GetDimentions().x;
			break;
		case 3:
			// Set UV Coord - V
			VertData[i] = (texQuad.w * QuadVertices[i] + texQuad.y) / m_CurrentTexture->GetDimentions().y;
			break;
		default:
			break;
		}
	}

	// Resize buffer if needed
	if (m_DynamicCount >= m_DynamicCapacity)
	{
		m_DynamicCapacity += 10;
		m_DynamicVertices.resize(m_DynamicCapacity * 16);
		m_DynamicIndices.resize(m_DynamicCapacity * 6);
		glBindVertexArray(m_DynamicVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_DynamicVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16 * m_DynamicCapacity, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_DynamicEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * m_DynamicCapacity, NULL, GL_DYNAMIC_DRAW);
	}

	// Add vertex and index data to respective vector
	for (int i = 0; i < 16; i++)
	{
		m_DynamicVertices[m_DynamicCount * 16 + i] = VertData[i];
	}

	m_DynamicIndices[m_DynamicCount * 6 + 0] = (0 + (4 * m_DynamicCount));
	m_DynamicIndices[m_DynamicCount * 6 + 1] = (1 + (4 * m_DynamicCount));
	m_DynamicIndices[m_DynamicCount * 6 + 2] = (3 + (4 * m_DynamicCount));
	m_DynamicIndices[m_DynamicCount * 6 + 3] = (3 + (4 * m_DynamicCount));
	m_DynamicIndices[m_DynamicCount * 6 + 4] = (2 + (4 * m_DynamicCount));
	m_DynamicIndices[m_DynamicCount * 6 + 5] = (0 + (4 * m_DynamicCount));

	// increment count
	m_DynamicCount++;
}

void Renderer::AddStatic(float x, float y, float width, float height, glm::vec4 texQuad, float rotation)
{
	// Bind VAO
	glBindVertexArray(m_StaticVAO);

	// Create and set up transform matrix
	// rotate -> scale -> translate
	glm::mat4 transform = glm::mat4(1.f);
	transform = glm::translate(transform, glm::vec3(x, y, 0.f));
	transform = glm::scale(transform, glm::vec3(width, height, 1.f));
	transform = glm::rotate(transform, rotation, glm::vec3(0.f, 0.f, 1.f));
	// Add transform to vector
	m_Transforms.push_back(transform);

	// Add texture data to vector
	m_TexTransforms.push_back(texQuad.x); // x
	m_TexTransforms.push_back(texQuad.y); // y
	m_TexTransforms.push_back(texQuad.z); // width
	m_TexTransforms.push_back(texQuad.w); // height

	// Bind and add data to transform VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_StaticTransVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * m_Transforms.size(), m_Transforms.data(), GL_DYNAMIC_DRAW);

	// Bind and add data to texture vbo
	glBindBuffer(GL_ARRAY_BUFFER, m_StaticTexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_TexTransforms.size(), m_TexTransforms.data(), GL_DYNAMIC_DRAW);

	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Render()
{
	// Bind Texture
	m_CurrentTexture->Bind();

	//// STATIC:
	// Bind VAO
	glBindVertexArray(m_StaticVAO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	
	// Bind Shader
	m_ShaderStatic->use();

	m_ShaderStatic->setVec2("u_texDimens", m_CurrentTexture->GetDimentions());
	
	// Draw
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)m_Transforms.size());

	//// DYNAMIC:
	// Bind VAO
	glBindVertexArray(m_DynamicVAO);

	// Set buffer data
	glBindBuffer(GL_ARRAY_BUFFER, m_DynamicVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_DynamicVertices.size(), m_DynamicVertices.data());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_DynamicEBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * m_DynamicIndices.size(), m_DynamicIndices.data());

	// Bind Shader
	m_ShaderDynamic->use();

	// Draw
	glDrawElements(GL_TRIANGLES, (GLsizei)m_DynamicIndices.size(), GL_UNSIGNED_INT, 0);

	// Reset count 
	m_DynamicCount = 0;

	// Clear vector data
	m_DynamicIndices.clear();
	m_DynamicIndices.resize(m_DynamicCapacity * 6);
	m_DynamicVertices.clear();
	m_DynamicVertices.resize(m_DynamicCapacity * 16);
}

