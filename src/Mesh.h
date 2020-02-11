#pragma once
#ifndef MESH_H
#define MESH_H

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Log.h"
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

class Mesh
{
public:
	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	~Mesh();
	bool loadObj(const std::string& filename);
	bool loadTexture(string filePath, bool isSpecMap);
	void draw();
	//void DrawModel();
	void bindTextures();
	void unbindTextures();
	//void outputVertices();
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	void initBuffers();
	

private:
	void setupMesh();
	
	bool mLoaded;
	std::vector<std::pair<Texture2D, Texture>> textures;
	std::vector<Texture> mTextures;
	GLuint mVBO, mVAO, mEBO;
};

#endif
