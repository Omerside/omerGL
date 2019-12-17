#version 330 core


in vec2 TexCoord;
out vec4 frag_color;

uniform sampler2D myTexture;
//uniform sampler2D myTexture2;

void main()
{

	frag_color = texture(myTexture, TexCoord); //this is where we do the sampling of our texture
	//frag_color = mix(texture(myTexture, TexCoord), texture(myTexture2, TexCoord), 0.8); //Mixing textures

};