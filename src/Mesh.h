#pragma once
#ifndef MESH_H
//REMINDER: NO ifndef MESH_H or #define MESH_H
#define MESH_H

#include <vector>
#include <string>

#include "GL/glew.h"
#include "glm/glm.hpp"

using namespace glm;

struct Vertex {
	vec3 position;
	vec2 texCoords;
};

class Mesh
{
public:
	Mesh();
	~Mesh();
	bool loadObj(const std::string& filename);
	void draw();

private:
	void initBuffers();
	bool mLoaded;
	std::vector<Vertex> mVertices;
	GLuint mVBO, mVAO;
};

#endif
