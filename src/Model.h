#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <stb_image/stb_image.h>
#include <unordered_map> 

using namespace glm;

//Needed for ASSIMP loading
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

//Define the unique ID and parent ID of each node
struct NodeIDMap {
	string name;
	int id;
	int parentId;
};

class Model {
public:
	Model();
	Model(ShaderProgram *shaderIn, std::string meshFile, std::string texFile, bool isSpecMap = false);
	Model(ShaderProgram *shaderIn, std::string meshFile);
	Model(ShaderProgram *shaderIn, Mesh meshIn);
	bool LoadTextures(std::string file, float shininess, vec3 specular, vec3 ambient, bool isSpecMap = false);
	void LoadTextures(Texture2D texInput);
	bool LoadObjMesh(std::string file);
	void SetScale(vec3 scaleIn);
	void Draw(vec3 pos);
	void DrawModel(vec3 pos);
	vector<Mesh> meshes;
	map<string, Mesh*> meshesMap;

	//ASSIMP FUNCTIONS
	void loadModel(string  const &path);
	//END ASSIMP FUNCTIONS


	~Model();

protected:
	bool LoadTextures(std::string file, bool isSpecMap = false);
	bool LoadObjMeshes(std::string file);
	vec3 scale = vec3(1.0f);

	// ASSIMPT FUNCTIONS AND VARS

	void processNode(aiNode *node, const aiScene *scene, int boneId);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);

	string directory;
	unordered_map<string, NodeIDMap> pNodeIdMap; //This node relationship struct is used to generate the bone hierarchy for the AnimatedModel class. name->node.
	vector<Texture> textures_loaded;
	//END ASSIMP FUNCTIONS

	ShaderProgram *shader;
	std::pair<Texture2D, Texture2D> textures; //texture map, specular map
	Mesh mMesh; //used in an older file processing function
};

#endif

