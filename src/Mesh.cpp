#include "Mesh.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

void printMesh(Mesh *mesh) {

	const void * address = static_cast<const void*>(mesh);
	std::stringstream ss;
	ss << address;
	std::string stringAddress = ss.str();

	ofstream myfile;
	string filename = "2mesh_output_" + stringAddress + ".txt";

	myfile.open(filename, std::ios_base::app);
	myfile << "Writing mesh to a file: " << mesh << "\n" ;

	for (int i = 0; i < mesh->mVertices.size(); i++) {
		myfile << "pos: " << mesh->mVertices[i].position.x << " " << mesh->mVertices[i].position.y << " " << mesh->mVertices[i].position.z << "\n";
		myfile << "norm: " << mesh->mVertices[i].normal.x << " " << mesh->mVertices[i].normal.y << " " << mesh->mVertices[i].normal.z << "\n";
		myfile << "tex: " << mesh->mVertices[i].texCoords.x << " " << mesh->mVertices[i].texCoords.y << "\n";

	}

	myfile.close();
	return;
}


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


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> texturesIn)
{
	mVertices = vertices;
	mIndices = indices;
	//mTextures = texturesIn;
	

	//Create Texture2D objects based on the Texture vector that we receive
	LOG() << "4) Creating textures based on Texture input vector...";
	SetTextures(texturesIn);


	// Create and initialize the buffers
	//initBuffers();
	mLoaded = true;
	//printMesh(this);
	

}


Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mEBO);

}

void Mesh::SetTextures(std::vector<Texture> texturesIn) {
	for (int i = 0; i < texturesIn.size(); i++) {
		Texture2D newTex;
		newTex.loadTexture(texturesIn[i].path, true, false);
		newTex.isSpecMap = (texturesIn[i].type == "texture_specular");

		std::pair<Texture2D, Texture> tempPair(newTex, texturesIn[i]);
		textures.push_back(tempPair);
		LOG() << "4." << i << ") created texture " << texturesIn[i].path;

	}
}

void Mesh::PrintBoneHierarchy() {
	std::for_each(mBones.begin(), mBones.end(), [](Bone &out) {
		LOG() << "Bone name: " << out.name;
		LOG() << "   ID:     " << out.id;
		LOG() << "   Parent: " << out.parentId;
	});
}


vector<Bone> *Mesh::GetBoneByRef() {
	return &mBones;
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
				meshVertex.normal = tempNormals[normalIndices[i] - 1];
			}

			if (tempUVs.size() > 0) {
				meshVertex.texCoords = tempUVs[uvIndices[i] - 1];
			}

			mVertices.push_back(meshVertex);
		}

		// Create and initialize the buffers
		initBuffers();
		//printMesh(this);
		return (mLoaded = true);
	}

	// We shouldn't get here so return failure
	return false;
}


bool Mesh::loadTexture(string filePath, bool isSpecMap) {

	Texture2D newTex2D;
	Texture newTex;

	newTex2D.loadTexture(filePath, true, false);
	newTex2D.isSpecMap = isSpecMap;
	newTex.path = filePath;
	newTex.id = newTex2D.getTextureId();

	if (isSpecMap == true) {
		LOG() << "Loaded specular texture " << filePath;
		newTex.type = "texture_specular";
	} else {
		LOG() << "Loaded diffuse texture " << filePath;
		newTex.type = "texture_diffuse";
	}

	std::pair<Texture2D, Texture> tempPair(newTex2D, newTex);
	textures.push_back(tempPair);


}



void Mesh::bindTextures() {

	LOG() << "     Binding " << textures.size() << " textures.";
	for (unsigned int i = 0; i < textures.size(); i++) {
		if (textures[i].first.isSpecMap == false) {
			textures[i].first.bind(0);
			LOG() << "       Binding UV map...";
		}
		else {
			textures[i].first.bind(1);
			LOG() << "       Binding spec map...";
		}
	}
}

void Mesh::unbindTextures() {
	LOG() << "     Unbinding " << textures.size() << " textures.";
	for (unsigned int i = 0; i < textures.size(); i++) {
		if (textures[i].first.isSpecMap == false) {
			textures[i].first.unbind(0);
			LOG() << "       Binding UV map...";

		}
		else {
			textures[i].first.unbind(1);
			LOG() << "       Binding spec map...";

		}
	}
}


void Mesh::draw() {
	if (!mLoaded) { 
		LOG(ERROR) << "Mesh not loaded.";
		return; 
	}

	LOG() << "     - Binding vertex array object number " << this->mVAO;
	glBindVertexArray(this->mVAO);

	
	if (mIndices.size() > 0) {

		LOG() << "     - Drawing " << mIndices.size() << " indices.";
		glDrawElements(GL_TRIANGLES, mIndices.size(), GL_ELEMENT_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
	}
	else {
		LOG() << "     - Drawing " << mVertices.size() << " vertices.";
		glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
	}

	/*
	LOG() << "Sample vertices: ";
	for (int i = 0; i < 5; i++) {
		LOG() << "position: " << mVertices[i].position.x << mVertices[i].position.y << mVertices[i].position.z;
		LOG() << "normal: " << mVertices[i].normal.x << mVertices[i].normal.y << mVertices[i].normal.z;
		LOG() << "tex coords: " << mVertices[i].texCoords.x << mVertices[i].texCoords.y;
	}*/
	LOG() << "     Mesh drawn.";

	glBindVertexArray(0);
}



void Mesh::initBuffers() {

	glGenVertexArrays(1, &mVAO);				// Tell OpenGL to create new Vertex Array Object
	glGenBuffers(1, &mVBO);					// Generate an empty vertex buffer on the GPU
	glGenBuffers(1, &mEBO); 

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);		// "bind" or set as the current buffer we are working with
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);	// copy the data from CPU to GPU


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

	//Define position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);	// Define a layout for the first vertex buffer "0"
	glEnableVertexAttribArray(0);			// Enable the first attribute or attribute "0"

	//define normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//define texture 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(8 * sizeof(GLfloat)));

	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(11 * sizeof(GLfloat)));

	glBindVertexArray(0);
	LOG() << "	VAO: " << mVAO << " VBO: " << mVBO << " EBO: " << mEBO;
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);

	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);


	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));


	glBindVertexArray(0);
}