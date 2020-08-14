#ifndef MODEL_H
#define MODEL_H
#define _USE_MATH_DEFINES

#include "Mesh.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <math.h>
#include <stb_image/stb_image.h>
#include <unordered_map> 

using namespace glm;

//Needed for ASSIMP loading
unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);


//Store node information
struct Node {
	string name;
	mat4 transform;
	int id;
	int parentId;
	vector<int> childrenId;
	bool hasBone = false;
	Bone* bone;
	
};


/*
Meshes associate with bone arrays and poses incrementally. 
Mesh 1 -> Bones array 1 -> global pose 1, etc. etc.
*/
struct Skeleton
{
	int boneCount = 0; // number of bones
	vector<Mesh*> meshes;
	Bone*** bones; // [Mesh ID][bone ID] = ->bone
	vector<mat4*> globalPoses; //[Mesh ID][bone ID] = global pose
	vector<mat4*> localPoses; // [Mesh ID][bone ID] = local poses
	map<string, Bone*> bonesMap;
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
	void SetRotation(float rotationIn);
	void SetRotation(vec3 rotationIn);
	void EnableOutline(vec3 colorInput, vec3 scaleInput, vec3 outlineColorHiddenInput);
	void EnableOutline();
	void DisableOutline();
	void Draw(vec3 pos);
	void DrawModel(vec3 pos);
	void DrawOutline(vec3 pos);
	void DrawOutlineHidden(vec3 pos);
	void PrintNodeHierarchy();
	int GetNodeCount() { return nodeCount; }
	void loadModel(string  const &path);
	bool getOutline() { return hasOutline; };
	string GetDirectory() {	return directory;}

	~Model();

protected:
	

	bool LoadTextures(string file, bool isSpecMap = false);
	bool LoadObjMeshes(string file);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
	int processNode(aiNode *node, int boneId, int* sBoneId);
	Mesh processMesh(aiMesh *mesh);
	void processBones(aiNode *node);
	void SetFinalSkelTransforms();
	void SetFinalBoneTransform(int boneId);

	//baseline
	const aiScene* scene;
	vec3 scale = vec3(1.0f);
	GLfloat rotation = 0;
	string directory;
	ShaderProgram *shader;

	//Node data
	vector<Node> nodes;
	map<string, int> nodesMap; //This node relationship struct is used to generate the bone hierarchy for the AnimatedModel class. name->node location in nodes array.
	int nodeCount = 0; // quick access for nodesMap count of objects.

	//Mesh data
	aiMesh* rawMesh; // the raw mesh array derived from assimp
	Mesh mMesh; //primary mesh used for containing bones
	vector<Mesh> meshes; //deprecated
	map<string, Mesh*> meshesMap; //deprecated

	
	//Texture data
	vector<Texture> textures_loaded;
	pair<Texture2D, Texture2D> textures; //texture map, specular map

	
	//Animation data
	mat4 globalInverseTransform; //inverse of Scene->mRootNode->mTransformation;
	mat4 finalTransforms[MAX_NUM_OF_BONES]; //tansformation matrices to be sent to shader

	//Outline properties
	bool hasOutline = false;
	vec3 outlineColor;
	vec3 outlineColorHidden;
	vec3 outlineSize;
	

};

#endif

