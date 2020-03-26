#pragma once
#ifndef MESH_H
#define MESH_H

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Log.h"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <vector>
#include <string>



using namespace glm;

struct Vertex {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
	vec3 tangent;
	vec3 bitangent;

};


struct Texture {
	unsigned int id;
	string type;
	string path;  // we store the path of the texture to compare with other textures
};

struct BonePose
{
	quat rotation = quat(0, 0, 0, 1);
	vec3 translation;
	float scale = 1;
};

struct Bone
{
	float* vertexWeights;
	int id = -1; // identifying bone number
	int meshId = -1; // identifying mesh number
	int parentId; //  ID of parent joint
	vector<int> childrenId;  // list of all children IDs

	BonePose bindPose;
	mat4 invBindPose; // inverse bind pose -- CONFIRMED CORRECT
	mat4* localPose;
	mat4* globalPose;
	string name; // human-friendly name
	vector<BonePose> poses;
};

//helper functions
mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from);
vec3 aiVector3DToGlm(const aiVector3D* from);
vec3 aiVector3DToGlm(aiVector3D from);
const int MAX_NUM_OF_BONES = 100;

class Mesh
{
public:
	Mesh();
	Mesh(Vertex* vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	~Mesh();
	bool loadObj(const std::string& filename);
	bool loadTexture(string filePath, bool isSpecMap);
	void draw();
	void bindTextures();
	void unbindTextures();
	void SetTextures(std::vector<Texture> texturesIn);
	void PrintBoneHierarchy();
	Bone GetBoneById(int id);
	mat4 static TransformMatrix(mat4 trans, vec4 vec);
	

private:
	friend class Model;
	friend class AnimatedModel;
	
	void TransformSkeleton(mat4 trans);
	void InsertPose(float time, float scale, quat rotation, vec3 translation, int id);
	void InsertPose(float time, BonePose pose, int id);
	void StoreBoneById(Bone* bone, int id);
	void CleanUnorderedBoneArrays() {  mBones.clear(); }
	void initBuffers();

	Vertex* mVertices;
	uint mNumVertices;
	vector<unsigned int> mIndices;
	std::vector<std::pair<Texture2D, Texture>> textures;
	std::vector<Texture> mTextures;
	GLuint mVBO, mVAO, mEBO;
	Mesh* mParentMesh;

	vector<Bone> mBones; //unordered stack of bones
	Bone* mBonesArrOrdered[MAX_NUM_OF_BONES]; // refers to bone objects by their ID
	map<string, Bone*> bonesMap; // Map array for bone access by name
	bool mLoaded;
	int maxBoneId = 0;
	int minBoneId = MAX_NUM_OF_BONES;

	int id = -1;
};

#endif
