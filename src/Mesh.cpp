#include "Mesh.h"
#include <iostream>
#include <sstream>
#include <fstream>


std::vector<std::string> split(std::string s, std::string t)
{
	std::vector<std::string> res;
	while(1)
	{
		int pos = s.find(t);
		if(pos == -1)
		{
			res.push_back(s); 
			break;
		}
		res.push_back(s.substr(0, pos));
		s = s.substr(pos+1, s.size() - pos - 1);
	}
	return res;
}
Mesh::Mesh()
	:mLoaded(false)
{
}


Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
}


//-----------------------------------------------------------------------------
// Loads a Wavefront OBJ model
//
// NOTE: This is not a complete, full featured OBJ loader.  It is greatly
// simplified.
// Assumptions!
//  - OBJ file must contain only triangles
//  - We ignore materials
//  - We ignore normals
//  - only commands "v", "vt" and "f" are supported
// Made by Steve Jones from Game Institutre Inc. because Omer is too lazy to write a file reader.
//-----------------------------------------------------------------------------
bool Mesh::loadObj(const std::string& filename) {
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<vec3> tempVertices;
	std::vector<vec2> tempUVs;
	std::vector<vec3> tempNormals;


	if (filename.find(".obj") != std::string::npos)
	{
		std::ifstream fin(filename, std::ios::in);
		if (!fin)
		{
			std::cerr << "Cannot open " << filename << std::endl;
			return false;
		}

		std::cout << "Loading OBJ file " << filename << " ..." << std::endl;

		std::string lineBuffer;
		while (std::getline(fin, lineBuffer))
		{
			std::stringstream ss(lineBuffer);
			std::string cmd;
			ss >> cmd;

			if (cmd == "v")
			{
				vec3 vertex;
				int dim = 0;
				while (dim < 3 && ss >> vertex[dim]) {
					dim++;
				}

				tempVertices.push_back(vertex);
			}
			else if (cmd == "vt")
			{
				vec2 uv;
				int dim = 0;
				while (dim < 2 && ss >> uv[dim]) {
					dim++;
				}

				tempUVs.push_back(uv);
			}
			else if (cmd == "vn")
			{
				vec3 normal;
				int dim = 0;
				while (dim < 3 && ss >> normal[dim]) {
					dim++;
				}

				normal = normalize(normal);
				tempNormals.push_back(normal);
			}
			else if (cmd == "f")
			{
				std::string faceData;
				int vertexIndex, uvIndex, normalIndex; // v/vt/vn

				while (ss >> faceData) {
					std::vector<std::string> data = split(faceData, "/");


					// vertex index
					if (data[0].size() > 0) {
						sscanf_s(data[0].c_str(), "%d", &vertexIndex);
						vertexIndices.push_back(vertexIndex);
					}

					// if face vert has texture coordinates 
					if (data.size() >= 1) {
						if (data[1].size() > 0) {
							sscanf_s(data[1].c_str(), "%d", &uvIndex);
							uvIndices.push_back(uvIndex);
						}
					}

					//does face vert have a normal
					if (data.size() >= 2) {
						if (data[2].size() > 0) {
							sscanf_s(data[2].c_str(), "%d", &normalIndex);
							normalIndices.push_back(normalIndex);
						}
					}
				}
			}
		}

		// Close the file
		fin.close();


		// For each vertex of each triangle
		for (unsigned int i = 0; i < vertexIndices.size(); i++)
		{
			Vertex meshVertex;

			if (tempVertices.size() > 0) {
				meshVertex.position = tempVertices[vertexIndices[i] - 1];
			}

			if (tempNormals.size() > 0) {
				meshVertex.normal = tempNormals[normalIndices[i]];// - 1];
			}

			if (tempUVs.size() > 0) {
				meshVertex.texCoords = tempUVs[uvIndices[i] - 1];
			}

			mVertices.push_back(meshVertex);
		}

		// Create and initialize the buffers
		initBuffers();

		return (mLoaded = true);
	}

	// We shouldn't get here so return failure
	return false;
}

void Mesh::draw() {
	if (!mLoaded) { return; }

	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
	glBindVertexArray(0);
}

void Mesh::initBuffers() {

	glGenBuffers(1, &mVBO);					// Generate an empty vertex buffer on the GPU
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);		// "bind" or set as the current buffer we are working with
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);	// copy the data from CPU to GPU

	glGenVertexArrays(1, &mVAO);				// Tell OpenGL to create new Vertex Array Object
	glBindVertexArray(mVAO);					// Make it the current one

	//Define position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);	// Define a layout for the first vertex buffer "0"
	glEnableVertexAttribArray(0);			// Enable the first attribute or attribute "0"

	//define normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//define texture 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}