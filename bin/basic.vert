#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

/*

Calculating new vertex position in 3d space:
Vertex (clip space) = Projection matrix x View Matrix x Model Matrix x Vertex in local space

*/

void main()
{
	gl_Position = projection * view * model * vec4(pos, 1.0f);
	TexCoord = texCoord;
};