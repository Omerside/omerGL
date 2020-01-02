#include "Model.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <algorithm>


Model::Model(ShaderProgram *shaderIn, std::string meshFile, std::string texFile, bool isSpecMap) :
	shader(shaderIn)
{
	//constructor for .obj type meshes which need explicit properties 

	if (!LoadObjMeshes(meshFile)) {
		std::cerr << "Error loading .obj mesh for object " + meshFile << std::endl;
		return;
	}

	if (!LoadTextures(texFile, isSpecMap)) {
		std::cerr << "Error loading textures for object " + texFile << std::endl;
		return;
	}
}

Model::Model(ShaderProgram *shaderIn, std::string meshFile) :
	shader(shaderIn)
{
	//constructor for .obj type meshes which need explicit properties 

	if (!LoadObjMeshes(meshFile)) {
		std::cerr << "Error loading .obj mesh for object " + meshFile << std::endl;
		return;
	}

}

Model::Model(ShaderProgram *shaderIn, Mesh meshIn) :
	shader(shaderIn)
{
	//constructor for .obj type meshes which need explicit properties 
	mesh = meshIn;
}



Model::~Model() {

}

bool Model::LoadTextures(std::string file, float shininess, vec3 specular, vec3 ambient, bool isSpecMap) {
	Texture2D texture;

	bool result;
	if (!isSpecMap) {
		result = texture.loadTexture(file, true);
		if (result) {
			texture.shininess = shininess;
			texture.specular = specular;
			texture.ambient = ambient;
			textures.first = texture;
		} else {
			return false;
		}
	} else {
		result = texture.loadTexture(file, true);
		if (result){
			texture.isSpecMap = true;
			textures.second = texture;
		} else {
			return false;
		}
	}


	return true;
}

bool Model::LoadTextures(std::string file, bool isSpecMap) {
	Texture2D texture;
	bool result;

	if (!isSpecMap) {
		result = texture.loadTexture(file, true);
		if (result) {
			textures.first = texture;

		} else {
			return false;

		}
	} else {
		result = texture.loadTexture(file, true);
		if (result) {
			texture.isSpecMap = false;
			textures.second = texture;

		} else {
			return false;

		}
	}

	return true;
}

void Model::LoadTextures(Texture2D texInput) {
	if (texInput.isSpecMap == false) {
		textures.first = texInput;
	}
	else {
		textures.second = texInput;
	}

}

bool Model::LoadObjMeshes(std::string file) {
	if (!mesh.loadObj(file)) {
		return true;
	}
	else {
		return false;
	}
}

bool Model::LoadObjMesh(std::string file) {
	if (!mesh.loadObj(file)) {
		return true;
	}
	else {
		return false;
	}
}

void  Model::Draw(vec3 pos){

	mat4 model = translate(mat4(), pos) * glm::scale(mat4(), scale);

	shader->SetUniform("model", model);
	shader->SetUniform("material.ambient", textures.first.ambient);
	shader->SetUniform("material.specular", textures.first.specular);
	shader->SetUniform("material.shininess", textures.first.shininess);
	shader->SetUniformSampler("material.textureMap", 1);
	shader->SetUniformSampler("material.specularMap", 0);

	textures.second.bind(0);
	textures.first.bind(1);
	mesh.draw();
	textures.second.unbind(0);
	textures.first.unbind(1);

}


void Model::SetScale(vec3 scaleIn) {
	scale = scaleIn;
}