#version 330 core

layout (location = 0) in vec4 vertex;
layout (location = 1) in mat4 transform;
layout (location = 5) in vec4 texQuad;

layout (std140) uniform Matrices
{
	mat4 VP;
};

uniform vec2 u_texDimens;

out vec2 texCoord;

void main()
{
	gl_Position = VP * transform * vec4(vertex.xy, 0.0, 1.0);
	texCoord.x = (texQuad.z * vertex.z + texQuad.x)/u_texDimens.x;
	texCoord.y = (texQuad.w * vertex.w + texQuad.y)/u_texDimens.y;
}