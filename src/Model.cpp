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
	shader->SetUniform("model", model);

	int meshSize = meshes.size();
	for (int i = 0; i < meshSize; i++) {
		LOG() << "5." << i << ") Drawing mesh.";
		meshes[i].bindTextures();
		meshes[i].draw();
		meshes[i].unbindTextures();
		LOG() << "Finished drawing mesh.";
	}

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
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace );
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

	if (scene->HasMaterials()) {
		LOG() << "Note: This model contains materials. ";
		if (scene->HasTextures()) {
			LOG() << "As well as textures. ";
		}
	}
	LOG() << "Number of nodes: " << 1+(scene->mRootNode->mNumChildren);
	processNode(scene->mRootNode, scene, 0);
	LOG() << "Number of meshes processed: " << meshes.size();

	for (int i = 0; i < meshes.size(); i++) {
		
		meshes[i].initBuffers();
	}
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode *node, const aiScene *scene, int parentNodeId)
{
	static int sBoneId = 0;
	sBoneId++;

	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		LOG() << "3." << i << ") adding mesh named: " << mesh->mName.C_Str();
		

		Mesh tempMesh = processMesh(mesh, scene);
		tempMesh.mNumVertices = mesh->mNumVertices;

		//Process bones if they exist
		if (mesh->HasBones()) {
			tempMesh.mBonePose.transformation = aiMatrix4x4ToGlm(&(node->mTransformation));

			LOG() << "Number of bones in this mesh: " << mesh->mNumBones;
			for (unsigned int j = 0; j < mesh->mNumBones; j++) {

				Bone newBone;
				newBone.name = mesh->mBones[j]->mName.C_Str();
				newBone.invBindPose = aiMatrix4x4ToGlm(&(mesh->mBones[j]->mOffsetMatrix));
				newBone.vertexWeights = new float[tempMesh.mNumVertices];//(mesh->mBones[j]->mNumWeights)+1];

				//Store vertex weight information in relation to the bone we are looking at
				for (unsigned int k = 0; k < (mesh->mBones[j]->mNumWeights); k++) {
					newBone.vertexWeights[mesh->mBones[j]->mWeights->mVertexId] = mesh->mBones[j]->mWeights->mWeight;
				}

				tempMesh.mBones.push_back(newBone);
			}
			
		}


		tempMesh.mParentMesh = &meshes[parentNodeId];
		meshes.push_back(tempMesh);
		meshesMap[mesh->mName.C_Str()] = &meshes.back();
	}

	

	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	//If there are no children, we simply assign the node properties and return.
	//We also take this chance to populate a map of parent-child relationship between the nodes.
	NodeIDMap newNode;
	newNode.name = node->mName.C_Str();
	newNode.id = sBoneId;
	newNode.parentId = parentNodeId;
	pNodeIdMap[newNode.name] = newNode;
	LOG() << "NODE: " << newNode.name << " ID: " << newNode.id << " --> " << newNode.parentId;


	//LOG() << "Children count for " << node->mName.C_Str() << ": " << node->mNumChildren;
	for (int i = 0; i < node->mNumChildren; i++){
		processNode(node->mChildren[i], scene, newNode.id);
	}

}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	
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
		vertex.tangent = vector;
		// bitangent
		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		vertex.bitangent = vector;
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

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
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
