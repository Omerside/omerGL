#version 330 core


in vec2 TexCoord;
out vec4 frag_color;

uniform vec3 lightColor;
uniform sampler2D myTexture;

void main()
{

	//Ambient
	float ambientFactor = 0.6f;
	vec3 ambient = lightColor * ambientFactor;

	//Diffuse
	vec3 diffuse = vec3(0.0f);

	//Specular
	vec3 specular = vec3(0.0f);

	//final color
	vec4 texel = texture(myTexture, TexCoord);
	frag_color = vec4(ambient + diffuse + specular, 1.0f);// * texel;
	//frag_color = vec4(lightColor, 1.0f);
};