#version 330 core


in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
out vec4 frag_color;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D myTexture;

void main()
{

	//Ambient
	float ambientFactor = 0.2f;
	vec3 ambient = lightColor * ambientFactor;

	//Diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - FragPos);
	float NdotL = max(dot(normal, lightDirection), 0.0f);
	vec3 diffuse = lightColor * NdotL * 1.0f;

	//Specular (AKA Phong)
	float specularFactor = 0.8f;
	float shininess = 100.0f;
	vec3 viewDirection = normalize(viewPos - FragPos);

	//Blinn-phong components:
	vec3 halfDirection = normalize(lightDirection + viewDirection);
	float NdotH = max(dot(normal, halfDirection), 0.0f);
	vec3 specular =  lightColor * specularFactor * pow(NdotH, shininess);
	

	//Phong components:
	//vec3 reflectDirection = reflect(-lightDirection, normal);
	//float RdotV = max(dot(reflectDirection, viewDirection), 0.0f);
	//vec3 specular =  lightColor * specularFactor * pow(RdotV, shininess);
	

	//final color
	vec4 texel = texture(myTexture, TexCoord);
	frag_color = vec4(ambient + diffuse + specular, 1.0f) * texel;
	//frag_color = vec4(lightColor, 1.0f);
};