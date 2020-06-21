#include "Model.h"
#include "Log.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <algorithm>

Model::Model(){
	
}

Model::Model(ShaderProgram *shaderIn, std::string meshFile, std::string texFile, bool isSpecMap) :
	shader(shaderIn)
{
	//constructor for .obj type meshes which need explicit properties 

	if (!LoadObjMeshes(meshFile)) {
		std::cerr << "Error loading .obj mesh for object " + meshFile << std::endl;
		return;
	}

	if (!LoadTextures(texFile, isSpecMap)) {
		std::cerr << "Error loading textures for object " + texFile << std::endl;
		return;
	}
}

Model::Model(ShaderProgram *shaderIn, std::string meshFile) :
	shader(shaderIn)
{
	LOG() << "1) Loading mesh: " << meshFile;
	loadModel(meshFile.c_str());
	return;


}

Model::Model(ShaderProgram *shaderIn, Mesh meshIn) :
	shader(shaderIn)
{
	//constructor for .obj type meshes which need explicit properties 
	mMesh = meshIn;
}



Model::~Model() {

}

bool Model::LoadTextures(std::string file, float shininess, vec3 specular, vec3 ambient, bool isSpecMap) {
	Texture2D texture;

	bool result;
	if (!isSpecMap) {
		result = texture.loadTexture(file, true, false);
		if (result) {
			texture.shininess = shininess;
			texture.specular = specular;
			texture.ambient = ambient;
			textures.first = texture;
		} else {
			return false;
		}
	} else {
		result = texture.loadTexture(file, true, false);
		if (result){
			texture.isSpecMap = true;
			textures.second = texture;
		} else {
			return false;
		}
	}


	return true;
}

bool Model::LoadTextures(std::string file, bool isSpecMap) {
	Texture2D texture;
	bool result;

	if (!isSpecMap) {
		result = texture.loadTexture(file, true, false);
		if (result) {
			textures.first = texture;

		} else {
			return false;

		}
	} else {
		result = texture.loadTexture(file, true, false);
		if (result) {
			texture.isSpecMap = false;
			textures.second = texture;

		} else {
			return false;

		}
	}

	return true;
}

void Model::LoadTextures(Texture2D texInput) {
	if (texInput.isSpecMap == false) {
		textures.first = texInput;
	}
	else {
		textures.second = texInput;
	}

}

bool Model::LoadObjMeshes(std::string file) {
	if (!mMesh.loadObj(file)) {
		return true;
	}
	else {
		return false;
	}
}

bool Model::LoadObjMesh(std::string file) {
	if (!mMesh.loadObj(file)) {
		return true;
	}
	else {
		return false;
	}
}

void  Model::Draw(vec3 pos){

	mat4 model = translate(mat4(), pos) * glm::scale(mat4(), scale);

	shader->SetUniform("model", model);
	shader->SetUniform("material.ambient", textures.first.ambient);
	shader->SetUniform("material.specular", textures.first.specular);
	shader->SetUniform("material.shininess", textures.first.shininess);
	shader->SetUniformSampler("material.textureMap", 1);
	shader->SetUniformSampler("material.specularMap", 0);

	textures.second.bind(0);
	textures.first.bind(1);
	mMesh.draw();
	textures.second.unbind(0);
	textures.first.unbind(1);

}

void  Model::DrawModel(vec3 pos) {

	shader->SetUniformSampler("material.textureMap", 0);
	shader->SetUniformSampler("material.specularMap", 1);
	shader->SetUniform("material.ambient", vec3(1.0));
	shader->SetUniform("material.specular", vec3(1.0));
	shader->SetUniform("material.shininess", vec3(35.0));

	mat4 model = translate(mat4(), pos) * glm::scale(mat4(), scale);


	/*
	int meshSize = meshes.size();
	for (int j = 0; j < mMesh.mBones.size(); j++) {
		model = (mMesh.mBones[j].globalPose) * (glm::scale(mat4(), scale));
	}*/

	shader->SetUniform("model", model);

	LOG() << "5) Drawing mesh.";
	mMesh.bindTextures();
	mMesh.draw();
	mMesh.unbindTextures();
	LOG() << "Finished drawing mesh.";
	

}


void Model::SetScale(vec3 scaleIn) {
	scale = scaleIn;
}



/**************************/
/* BEGIN ASSIMP FUNCTIONS */
/**************************/


unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
	string filename = string(path);
	//filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


void Model::loadModel(string const &path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
	LOG() << "2) imported scene";
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	LOG() << "3) Beginning processing nodes...";


	globalInverseTransform = inverse(aiMatrix4x4ToGlm(&scene->mRootNode->mTransformation));
	processBones(scene->mRootNode);
	processNode(scene->mRootNode, -1);
	
	
	
	LOG() << "Number of meshes processed: " << meshes.size();
	
	mMesh.initBuffers();	
}


void Model::processBones(aiNode *node) {
	static bool bonesFound = false;

	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];


		LOG(DEBUG) << "3." << i << ") adding mesh named: " << mesh->mName.C_Str() << "with ID: " << i;

		mMesh = processMesh(mesh);
		mMesh.mNumVertices = mesh->mNumVertices;
		mMesh.vertexWeightArr.resize(mMesh.mNumVertices);
		mMesh.id = i;


		//Process bones if they exist
		if (mesh->HasBones()) {

			//resize bone vector to accommodate num of bones.
			mMesh.mBones.resize(mesh->mNumBones);

			for (unsigned int j = 0; j < mesh->mNumBones; j++) {

				Bone newBone;

				//Define temporary bone obj
				newBone.name = mesh->mBones[j]->mName.C_Str();
				newBone.offsetMatrix = aiMatrix4x4ToGlm(&(mesh->mBones[j]->mOffsetMatrix));
				newBone.vertexWeights.resize(mesh->mBones[j]->mNumWeights);
				newBone.rawBone = *mesh->mBones[j];

				//Store bone information in mMesh
				mMesh.mBones[j] = newBone;
				mMesh.SetBoneByName(&mMesh.mBones[j], newBone.name);
			}
			LOG(DEBUG) << "processBones: found mesh with bones - assigned to bones array in mMesh.";
			bonesFound = true;
			return;

		}
	}


	for (int i = 0; i < node->mNumChildren; i++) {
		if (bonesFound == false) {
			processBones(node->mChildren[i]);
		}
		else {
			return;
		}
	}

}



// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
// returns node ID
int Model::processNode(aiNode *node, int parentNodeId)
{
	static int sBoneId = parentNodeId;
	sBoneId++;

	Node newNode;	
	newNode.name = node->mName.C_Str();
	newNode.id = sBoneId;
	newNode.parentId = parentNodeId;
	newNode.transform = aiMatrix4x4ToGlm(&node->mTransformation);
	nodeCount++;

	Bone* bonePtr = mMesh.bonesMap[newNode.name];

	if (bonePtr > 0) {

		mMesh.SetBoneIdByName(sBoneId, newNode.name);
		mMesh.bonesMap[newNode.name]->nodeTransform = newNode.transform;
		bonePtr->parentId = parentNodeId;

		//Store bone->vertex weight relationship, now that we know the ID associated with the bone.
		for (unsigned int k = 0; k < (bonePtr->rawBone.mNumWeights); k++) {
			
			bonePtr->vertexWeights[k] = pair<int, float>(bonePtr->rawBone.mWeights[k].mVertexId, bonePtr->rawBone.mWeights[k].mWeight);
			mMesh.vertexWeightArr[bonePtr->rawBone.mWeights[k].mVertexId].insert(sBoneId, bonePtr->rawBone.mWeights[k].mWeight);
			LOG() << "for Vertex ID " << bonePtr->rawBone.mWeights[k].mVertexId << ", inserted weight " << bonePtr->rawBone.mWeights[k].mWeight << " at bone ID " << sBoneId;
		}
	}


	//Assign child node/bone values.
	for (int i = 0; i < node->mNumChildren; i++) {
		newNode.childrenId.push_back(processNode(node->mChildren[i], newNode.id));
		
		if (bonePtr > 0) {
			LOG() << "Assigning childrenId of node " << node->mName.C_Str() << " to bone " << bonePtr->name;
			bonePtr->childrenId.push_back(newNode.childrenId[newNode.childrenId.size() - 1]);
		}
	}

	nodes.push_back(newNode);
	nodesMap[newNode.name] = nodes.size()-1;
	
	return newNode.id;
}



Mesh Model::processMesh(aiMesh *mesh)
{
	
	Vertex* vertices = new Vertex[mesh->mNumVertices];
	vector<unsigned int> indices;
	vector<Texture> textures;

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;
		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		// tangent
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		//vertex.tangent = vector;
		// bitangent
		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		//vertex.bitangent = vector;
		vertices[i] = vertex;
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// process materials
	if (!scene->HasMaterials()) {
		return Mesh(vertices, indices, textures);
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	aiString matName, matString;
	material->Get(AI_MATKEY_NAME, matString);
	LOG() << "	Load material name: " << matString.C_Str();


	// 1. diffuse maps
	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());



	// 2. specular maps
	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());


	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());


	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

void Model::PrintNodeHierarchy() {
	for (int i = 0; i < nodes.size(); i++) {

			LOG() << "Node name:   " << nodes[i].name;
			if (nodes[i].hasBone) {
				LOG() << "Correlates to bone.";
			}

			LOG() << "   ID:       " << nodes[i].id;
			LOG() << "   Parent:   " << nodes[i].parentId;

			int size = nodes[i].childrenId.size();
			for (int j = 0; j < size; j++) {
				LOG() << "   Child: " << nodes[i].childrenId[j];
			}

	}
}

void Model::SetFinalSkelTransforms() {
	for (int i = 0; i < mMesh.mBones.size(); i++) {
		SetFinalBoneTransform(mMesh.mBones[i].id);
	}

}

void Model::SetFinalBoneTransform(int boneId) {
	LOG() << "Calculating transform for id " << mMesh.mBonesArrOrdered[boneId]->name;
	this->finalTransforms[boneId] = globalInverseTransform * mMesh.mBonesArrOrdered[boneId]->globalPose * mMesh.mBonesArrOrdered[boneId]->offsetMatrix;
	LOG() << "Final transform for bone  " << mMesh.mBonesArrOrdered[boneId]->name << " ID: " << mMesh.mBonesArrOrdered[boneId]->id << " is: " << this->finalTransforms[boneId];
}


// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	
	vector<Texture> textures;
	for (int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				LOG() << "		Texture " << textures_loaded[j].path.data() << " has already been loaded.";
				break;
			}
		}


		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			LOG() << "		Loading Texture named: " << texture.path;
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}

	}
	return textures;
}

