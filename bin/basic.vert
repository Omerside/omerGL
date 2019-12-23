#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

/*

Calculating new vertex position in 3d space:
Vertex (clip space) = Projection matrix x View Matrix x Model Matrix x Vertex in local space

*/

void main()
{
	

	//To get world space, we multiply our view space by our model vector
	vec4 worldPos = vec4(model * vec4(pos, 1.0f));
	
	
	FragPos = vec3(worldPos);

	//recalculate normals in case of scaling skewing incoming values
	Normal = mat3(transpose(inverse(model))) * normal;

	gl_Position = projection * view * worldPos;
	TexCoord = texCoord;
};