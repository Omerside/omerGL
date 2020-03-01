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
	quat rotation;
	vec3 transformation;
	float scale;
};

struct Bone
{
	mat4x3 invBindPose; // inverse bind pose
	string name; // human-friendly name
	int id; // identifying joint number
	int parentId; //  ID of parent joint
	vector<int> childrenId;  // list of all children IDs
	BonePose pose;
};



class Mesh
{
public:
	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
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

	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	int jointId;
	

private:
	friend class Model;
	void setupMesh();
	bool mLoaded;
	std::vector<std::pair<Texture2D, Texture>> textures;
	std::vector<Texture> mTextures;
	GLuint mVBO, mVAO, mEBO;
	Mesh *mParentMesh;
	vector<Bone> mBones;
	BonePose mBonePose;
};

#endif
