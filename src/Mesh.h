#pragma once
#ifndef MESH_H
#define MESH_H

#include "GL/glew.h"
#include "glm/glm.hpp"
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
	mat4 transformation;
	float scale = 1;
};

struct Bone
{
	float* vertexWeights;
	mat4 invBindPose; // inverse bind pose
	string name; // human-friendly name
	int id; // identifying joint number
	int parentId; //  ID of parent joint
	vector<int> childrenId;  // list of all children IDs
	BonePose pose;
};

//helper functions
mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from);
vec3 aiVector3DToGlm(const aiVector3D* from);

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
	void initBuffers();
	void SetTextures(std::vector<Texture> texturesIn);
	void PrintBoneHierarchy();
	vector<Bone>* GetBoneByRef();

	Vertex* mVertices;
	uint mNumVertices;

	std::vector<unsigned int> mIndices;
	int jointId;
	

private:
	friend class Model;

	bool mLoaded;
	void TransformVertices(mat4 trans);
	std::vector<std::pair<Texture2D, Texture>> textures;
	std::vector<Texture> mTextures;
	GLuint mVBO, mVAO, mEBO;
	Mesh *mParentMesh;
	vector<Bone> mBones;
	BonePose mBonePose;
};

#endif
