#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include <iostream>

using namespace glm;

class Model {
public:
	Model(ShaderProgram *shaderIn, std::string meshFile, std::string texFile, bool isSpecMap = false);
	Model(ShaderProgram *shaderIn, std::string meshFile);
	Model(ShaderProgram *shaderIn, Mesh meshIn);
	bool LoadTextures(std::string file, float shininess, vec3 specular, vec3 ambient, bool isSpecMap = false);
	void LoadTextures(Texture2D texInput);
	bool LoadObjMesh(std::string file);
	void SetScale(vec3 scaleIn);
	void Draw(vec3 pos);
	~Model();

protected:
	bool LoadTextures(std::string file, bool isSpecMap = false);
	bool LoadObjMeshes(std::string file);
	vec3 scale = vec3(1.0f);

	ShaderProgram *shader;
	std::pair<Texture2D, Texture2D> textures; //texture map, specular map
	Mesh mesh;
};

#endif

