#include "Mesh.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

//Helper function for struct Bone
mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from) {
	mat4 to;

	to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
	to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
	to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
	to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

	return to;
}

mat3 aiMatrix3x3ToGlm(aiMatrix3x3t<ai_real> from) {
	mat3 to;

	to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1;
	to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2;
	to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3;
	
	return to;
}

vec3 aiVector3DToGlm(const aiVector3D* from) {
	return vec3(from->x, from->y, from->z);
}

vec3 aiVector3DToGlm(aiVector3D from) {
	return vec3(from.x, from.y, from.z);
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
	mBones.reserve(MAX_NUM_OF_BONES);
	mBonesArrOrdered.reserve(MAX_NUM_OF_BONES);

	LOG(DEBUG) << "Set arrays to " << MAX_NUM_OF_BONES << " in Mesh.";
	//mBonesArrOrdered = new Bone*[MAX_NUM_OF_BONES];
}


Mesh::Mesh(Vertex* vertices, std::vector<unsigned int> indices, std::vector<Texture> texturesIn)
{
	mBones.reserve(MAX_NUM_OF_BONES);
	bonesMap.reserve(MAX_NUM_OF_BONES);
	mBonesArrOrdered.reserve(MAX_NUM_OF_BONES);
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
	glDeleteBuffers(1, &mBBO);

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
		LOG() << "Bone name:  " << out.name;
		LOG() << "   ID:      " << out.id;
		LOG() << "   Parent:  " << out.parentId;
		std::for_each(out.childrenId.begin(), out.childrenId.end(), [](int outChild) {
			LOG() << "   Children:" << outChild;
		});
	});
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

		mNumVertices = vertexIndices.size();
		mVertices = new Vertex[mNumVertices];
		
		LOG() << "attempting to assign vertices in mesh...";
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

			mVertices[i] = (meshVertex);
		}

		// Create and initialize the buffers
		initBuffers();

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


void Mesh::TransformBone(int boneId) {
	Bone* bone = mBonesArrOrdered[boneId];
	mat4 trans = (bone->globalPose);// *bone->invBindPose);
}




/*
*Define global poses.
*This requires that Local Poses are defined.
*/

mat4 Mesh::GenerateLocalPose(BonePose pose) {


	//affine transformation begins with Scale and Rotation calculation:
	LOG(DEBUG) << "Generating local pose..";
	mat3 transMatrix = (pose.scale*toMat3(pose.rotation));
	
	LOG(DEBUG) << "Trans matrix: " << transMatrix;
	//Applied to the complete local pose:
	mat4 localPose;
	localPose[0] = vec4(transMatrix[0], 0);
	localPose[1] = vec4(transMatrix[1], 0);
	localPose[2] = vec4(transMatrix[2], 0);
	localPose[3] = vec4(pose.translation, 1);

	LOG() << "GenerateLocalPose - Local pose: " << localPose;
	return localPose;
}

//This is the one we actually call.
void Mesh::SetGlobalPoses() {
	LOG(DEBUG) << "minBoneId is " << minBoneId;
	Bone* bone = mBonesArrOrdered[minBoneId];
	LOG(DEBUG) << "Global pose is: \n" << bone->localPose;
	SetGlobalPose(minBoneId, bone->localPose);

	for (int i = 0; i < bone->childrenId.size(); i++) {
		//LOG() << "In SetGlobalPoses loop" << minBoneId;
		SetGlobalPoses(bone->childrenId[i], bone->localPose);
	}
	LOG(DEBUG) << "SetGlobalPoses :: DONE";
	//multiply our new local by the local of the parent ID
}


//This is a helper used in iterating.
void Mesh::SetGlobalPoses(int boneId, mat4 parentGlobalPose) {
	if (boneId == -1) { return; } // return of we've reached the root, though we should never get here.

	Bone* bone = mBonesArrOrdered[boneId];
	SetGlobalPose(boneId, parentGlobalPose * bone->localPose);

	for (int i = 0; i < bone->childrenId.size(); i++) {
		SetGlobalPoses(bone->childrenId[i], bone->globalPose);
	}

}

void Mesh::SetLocalPose(int boneId, mat4 pose) {
	LOG(DEBUG) << "SetLocalPose :: bone ID: " << boneId << " array size: " << mBonesArrOrdered.size();
	if (mBonesArrOrdered[boneId] == nullptr) { 
		LOG(DEBUG) << "SetLocalPose: Bone not found for ID " << boneId;
		return;
	}

	LOG(DEBUG) << "SetLocalPose :: Setting local pose of bone ID " << boneId << " to " << pose;
	LOG(DEBUG) << "SetLocalPose :: array value: " << mBonesArrOrdered[boneId];
	if(mBonesArrOrdered[boneId] == nullptr) {
		LOG(DEBUG) << "SetLocalPose :: mBonesArrOrdered at " << boneId << " is null.";
	}
	LOG(DEBUG) << "SetLocalPose :: MLOC: " << mBonesArrOrdered[boneId] << " ID: " << mBonesArrOrdered[boneId]->id;
	//LOG(DEBUG) << "SetLocalPose :: current pose: " << mBonesArrOrdered[boneId]->localPose;

	this->mBonesArrOrdered[boneId]->localPose = pose;
	LOG(DEBUG) << "SetLocalPose :: DONE";
}

void Mesh::SetGlobalPose(int boneId, mat4 pose) {
	mBonesArrOrdered[boneId]->globalPose = pose;
}

void Mesh::SetBonePose(int boneId, BonePose pose) {
	if (mBonesArrOrdered[boneId] == nullptr) {
		return;
	}

	SetLocalPose(boneId, GenerateLocalPose(pose));
	//SetGlobalPoses(boneId, GenerateGlobalPose(boneId));
}

void Mesh::SetSkeletonPose(BonePose* pose) {
	for (int i = 0; i < mBones.size(); i++) {
		//SetLocalPose(mBones[i].id, GenerateLocalPose(pose));
	}
	SetGlobalPoses();
}

mat4 Mesh::TransformMatrix(mat4 mat, vec4 vec) {
	mat[0] = mat[0] * vec;
	mat[1] = mat[1] * vec;
	mat[2] = mat[2] * vec;
	mat[3] = mat[3] * vec;
	return (mat);
}

void Mesh::SetBoneIdByName(int id, string name) {
	bonesMap[name]->id = id;
	StoreBoneById(this->bonesMap[name], id);
}

void Mesh::StoreBoneById(Bone* bone, int id) {
	LOG(DEBUG) << "StoreBoneById :: Storing bone name " << bone->name << " by ID " << id;
	mBonesArrOrdered[id] = bone;
	if (maxBoneId <= id) { maxBoneId = (id + 1); LOG(DEBUG) << "New MAX bone ID is " << (id + 1); }
	if (minBoneId > id) { minBoneId = id; }

	LOG(DEBUG) << "StoreBoneById :: MLOC: << " << &mBonesArrOrdered[id] << " Ref ID: " << mBonesArrOrdered[id]->id << " - maxBone ID: " << maxBoneId << " - minBone ID: " << minBoneId;
}

Bone Mesh::GetBoneById(int id) {
	if (mBonesArrOrdered[id] == nullptr) { return *(new Bone()); }//Return empty bone
	return *(mBonesArrOrdered[id]);
	
}

Bone Mesh::GetBoneByName(string name) {
	if (bonesMap[name] <= 0) { 
		LOG(WARN) << "No bone found by the name " << name;
		return *(new Bone()); 
	}

	return *bonesMap[name];

}

void Mesh::InsertBone(Bone boneIn) {
	Bone bone = boneIn;
	this->mBones.push_back(bone);

	if (!bone.name.empty()) {
		this->SetBoneByName(&mBones.back(), bone.name);
	}

	if (bone.id > 0) {
		this->SetBoneIdByName(bone.id, bone.name);
	}
}

void Mesh::SetBoneByName(int boneId, string name) {
	if (mBonesArrOrdered[boneId] == nullptr) {
		LOG(WARN) << "SetBoneByName: bone ID " << boneId << " does not refer to a bone in mBonesArrOrdered. ";
		return;
	}

	this->bonesMap.insert(pair<string, Bone*>(mBonesArrOrdered[boneId]->name, mBonesArrOrdered[boneId]));
}

void Mesh::SetBoneByName(Bone* bone, string name) {
	if (bone == nullptr) {
		LOG(WARN) << "SetBoneByName: cannot set reference to empty bone.";
		return;
	}

	this->bonesMap.insert(pair<string, Bone*>(bone->name, bone));
}

void Mesh::DrawModel() {

}



void Mesh::draw() {
	if (!mLoaded) { 
		LOG(ERR) << "Mesh not loaded.";
		return; 
	}
	

	LOG(DEBUG) << "     - Binding vertex array object number " << this->mVAO;
	glBindVertexArray(this->mVAO);

	
	if (mIndices.size() > 0) {

		LOG(DEBUG) << "     - Drawing " << mNumVertices << " indices.";
		glDrawArrays(GL_TRIANGLES, 0, mNumVertices);
		//glDrawElements(GL_TRIANGLES, mNumVertices, GL_ELEMENT_ARRAY_BUFFER, 0);
		
	}
	else {
		LOG(DEBUG) << "     - Drawing " << mNumVertices << " vertices.";
		glDrawArrays(GL_TRIANGLES, 0, mNumVertices);
	}

	LOG() << "     Mesh drawn.";

	glBindVertexArray(0);
}



void Mesh::initBuffers() {
	
	glGenVertexArrays(1, &mVAO);				// Tell OpenGL to create new Vertex Array Object
	glGenBuffers(1, &mVBO);					// Generate an empty vertex buffer on the GPU
	glGenBuffers(1, &mEBO); 
	glGenBuffers(1, &mBBO);

	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);		// "bind" or set as the current buffer we are working with
	glBufferData(GL_ARRAY_BUFFER, mNumVertices * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);	// copy the data from CPU to GPU

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumVertices * sizeof(unsigned int), &mIndices[0], GL_STATIC_DRAW);

	
	//Define position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);	// Define a layout for the first vertex buffer "0"
	glEnableVertexAttribArray(0);			// Enable the first attribute or attribute "0"

	//define normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//define texture 
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);


	glBindBuffer(GL_ARRAY_BUFFER, mBBO);
	glBufferData(GL_ARRAY_BUFFER, mNumVertices * sizeof(BoneVertexWeight), &vertexWeightArr[0], GL_STATIC_DRAW);


	// bone ID array
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(BoneVertexWeight), (0));
	glEnableVertexAttribArray(3);

	// vertex weight array
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(BoneVertexWeight), (GLvoid*)(4 * sizeof(GLint)));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
	
}

void Mesh::PrintVertexWeightArray() {
	LOG() << "Printing vertex weight array values";
	for (int i = 0; i < vertexWeightArr.size(); i++) {
		LOG() << "Vertex ID:       " << i;
		LOG() << "Vertex Location: " << mVertices[i].position;
		for (int j = 0; j < MAX_BONE_PER_VERTEX; j++) {
			LOG() << "Bone ID: " << vertexWeightArr[i].id[j];
			LOG() << "Weight: " << vertexWeightArr[i].weight[j];
			
		}
	}
}