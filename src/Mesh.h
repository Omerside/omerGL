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

//helper functions and const vars
mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from);
mat3 aiMatrix3x3ToGlm(aiMatrix3x3t<ai_real> from);
vec3 aiVector3DToGlm(const aiVector3D* from);
vec3 aiVector3DToGlm(aiVector3D from);
//mat4 translate(vec3 from);
//mat3 translate(vec2 from);
const int MAX_NUM_OF_BONES = 100;
const int MAX_BONE_PER_VERTEX = 4;

struct Vertex {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
};


struct Texture {
	unsigned int id;
	string type;
	string path;  // we store the path of the texture to compare with other textures
};

struct BonePose {
	quat rotation = quat(0, 0, 0, 1);
	mat4 rotationMat;
	vec3 translation;
	mat4 translationMat;
	mat4 transform;
	float scale = 1; //Not going to worry about scaling yet.
};


//This struct keeps track of bone->vertex weight relationship.
//a relationship will be stored in vector vertexWeightArr, where each vertex ID will
//relate to a set of associated bones and weights.
struct BoneVertexWeight {
	int id[MAX_BONE_PER_VERTEX];
	float weight[MAX_BONE_PER_VERTEX];
	void insert(int idInput, float weightInput){
		for (int i = 0; i < MAX_BONE_PER_VERTEX; i++) {
			if (id[i] == 0) {
				id[i] = idInput;
				weight[i] = weightInput;
				return;
			}
		}
	}
};

struct Bone {
	string name; // human-friendly name
	int id = -1; // identifying bone number
	int parentId = -1; //  ID of parent joint
	int meshId = 0; // identifying mesh number

	vector<int> childrenId;  // list of all children IDs
	vector< pair<int, float> > vertexWeights; //pairs of vertex IDs and weights

	mat4 nodeTransform;
	mat4 bindPose;
	mat4 offsetMatrix; // the job of the offset matrix it to move the vertex position from the local space of the mesh into the bone space of that particular bone
	mat4 localPose;
	mat4 globalPose;
	
	vector<BonePose> poses;
};



class Mesh
{
public:
	Mesh();
	Mesh(Vertex* vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	~Mesh();
	bool loadObj(const std::string& filename);
	bool loadTexture(string filePath, bool isSpecMap);
	void draw();
	void DrawModel();
	void bindTextures();
	void unbindTextures();
	void SetTextures(std::vector<Texture> texturesIn);
	void SetGlobalPoses(); // Iterate, generate, and set all global poses
	void PrintBoneHierarchy();
	Bone GetBoneById(int id);
	Bone GetBoneByName(string name);
	mat4 static TransformMatrix(mat4 trans, vec4 vec);
	

private:
	friend class Model;
	friend class AnimatedModel;
	
	void TransformBone(int boneId); // uses local and global poses to transform bone;
	void InsertBone(Bone boneIn);
	void StoreBoneById(Bone* bone, int id);
	void CleanUnorderedBoneArrays() {  mBones.clear(); }
	void initBuffers();
	
	void ChangeBoneId(int oldBoneId, int newBoneId);
	void SetBoneByName(int boneId, string name);
	void SetBoneByName(Bone* bone, string name);
	void SetBoneIdByName(int id, string name);//Give the bone an ID by its name. Init assignment only.


	void SetBonePose(int boneId, BonePose pose);
	void SetSkeletonPose(BonePose* pose);
	void SetLocalPose(int boneId, mat4 pose);
	void SetGlobalPose(int boneId, mat4 pose);
	void SetGlobalPoses(int boneId, mat4 parentGlobalPose); // helper function to help streamline 
	mat4 GenerateLocalPose(BonePose pose);
	
	//Baseline data (vertex information, buffers, etc.)
	Vertex* mVertices;
	vector<unsigned int> mIndices;
	uint mNumVertices;
	GLuint mVBO, mVAO, mEBO, mBBO;

	//texture data
	vector<std::pair<Texture2D, Texture>> textures;
	vector<Texture> mTextures;

	//bone data
	vector<Bone> mBones; //unordered stack of bones
	Bone* mBonesArrOrdered[MAX_NUM_OF_BONES]; // refers to bone objects by their ID
	map<string, Bone*> bonesMap; // Map array for bone access by name
	vector<BoneVertexWeight> vertexWeightArr; // [vertex ID] = {bone ID, weight}
	

	//misc data
	bool mLoaded;
	int id = -1;

	//possibly deprecated variables:
	int maxBoneId = 0;
	int minBoneId = MAX_NUM_OF_BONES;
};

#endif
