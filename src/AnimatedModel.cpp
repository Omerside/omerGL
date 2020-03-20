#include "AnimatedModel.h"
#include "Log.h"


AnimatedModel::AnimatedModel(ShaderProgram *shaderIn, std::string animationFile) 
{
	shader = shaderIn;

	//Load meshes into vector<Mesh> meshes (Model.h)
	loadModel(animationFile);

	//Assign bone hierarchy for animation
	AssignBonesHierarchyByNodes();

	//populate map and array for easier processing and faster access, respectively.
	PopulateBonesArrayAndMap();

	//Load animation data including bones, poses. 
	LoadAnimationData(animationFile);

	LOG() << "Animated model loaded.";

}

AnimatedModel::~AnimatedModel() {

}


void AnimatedModel::LoadAnimationData(string const &path){
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	if (!scene->HasAnimations()) {
		LOG() << " WARNING: file " << path << " does not contain an animation. ";
		return;
	}


	LOG() << "Beginning to load animations.";

	aiAnimation** animations = scene->mAnimations;
	for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
		processAnimNodes(animations[i], scene);

	}

	LOG() << "Finished loading animations.";


}

void AnimatedModel::processAnimNodes(aiAnimation *animation, const aiScene *scene)
{

	aiNodeAnim** nodeChannels = animation->mChannels;


	for (unsigned int i = 0; i < animation->mNumChannels; i++){
		LOG() << "Node animation channel name is: " << nodeChannels[i]->mNodeName.C_Str();
		LOG() << "Name of Bone value (MAP)   : " << meshes[0].bonesMap.at(nodeChannels[i]->mNodeName.C_Str())->name;
		//LOG() << "ID of Bone value: " << meshes[0].bonesMap.at(nodeChannels[i]->mNodeName.C_Str())->id;
		//LOG() << "Bone name at this ID: " << meshes[0].mBonesAr[meshes[0].bonesMap.at(nodeChannels[i]->mNodeName.C_Str())->id]->name;

		vec3 value = aiVector3DToGlm(&nodeChannels[i]->mPositionKeys->mValue);

	}

	
	/*
	for (unsigned int i = 0; i < animation->mNumMeshChannels; i++){

		LOG() << "Mesh animation channel name is: " << meshChannels[i]->mName.C_Str();
		
	}
	*/


	/**Transform vertices:
	aiMatrix4x4 tempMatrix = node->mTransformation;// .RotationX(0.4, m);
	tempMatrix.RotationX(-1.5, tempMatrix);
	tempMesh.TransformVertices(aiMatrix4x4ToGlm(&tempMatrix));
	**/
}

void AnimatedModel::AssignBonesHierarchyByNodes()
{
	LOG() << "Assigning bone hierarchy...";
	for (int i = 0; i < meshes.size(); i++) {

		//bones = &(meshes[i].mBones);		
		//childrenIdArray uses the ID of the bone as its location in the array
		vector<int>* childrenIdArray = new vector<int>[MAX_NUM_OF_BONES];

		
		for (int j = 0; j < meshes[i].mBones.size(); j++) {
			//define bone id, parent ID
			meshes[i].mBones[j].id = pNodeIdMap[(meshes[i].mBones[j]).name].id;
			
			//LOG() << "size of bones map: " << (*bones)[j].id;

			meshes[i].mBones[j].parentId = pNodeIdMap[(meshes[i].mBones[j]).name].parentId;
			
			//Record who the parent of this bone is. This will be used below to generate a map of children for each bone.
			childrenIdArray[meshes[i].mBones[j].parentId].push_back(meshes[i].mBones[j].id);

		}
		
		for (int j = 0; j < meshes[i].mBones.size(); j++) {
			//Assign list of children to bone based on ID
			meshes[i].mBones[j].childrenId = childrenIdArray[meshes[i].mBones[j].id];
		}
		
		meshes[i].PrintBoneHierarchy();

		//delete[] childrenIdArray;

	}

}

void  AnimatedModel::DrawModel(vec3 pos, int frame) {

	shader->SetUniformSampler("material.textureMap", 0);
	shader->SetUniformSampler("material.specularMap", 1);
	shader->SetUniform("material.ambient", vec3(1.0));
	shader->SetUniform("material.specular", vec3(1.0));
	shader->SetUniform("material.shininess", vec3(35.0));

	mat4 model = translate(mat4(), pos) * glm::scale(mat4(), scale);
	shader->SetUniform("model", model);

	int meshSize = meshes.size();
	for (int i = 0; i < meshSize; i++) {
		LOG() << "5." << i << ") Calculating transformation";
		//meshes[i].mVertices

		LOG() << "5." << i << ") Drawing mesh.";
		meshes[i].bindTextures();
		meshes[i].draw();
		meshes[i].unbindTextures();
		LOG() << "Finished drawing mesh.";
	}

	
}

/*Requires that mBones is populated for all meshes*/
void AnimatedModel::PopulateBonesArrayAndMap() {
	for (int i = 0; i < meshes.size(); i++) {

		for (int j = 0; j < meshes[i].mBones.size(); j++) {

			//meshes[i].mBonesArrOrdered[meshes[i].mBones[j].id] = (meshes[i].mBones[j]);
			meshes[i].StoreBoneById(&(meshes[i].mBones[j]), meshes[i].mBones[j].id);
			meshes[i].bonesMap.insert(pair<string, Bone*>(meshes[i].mBones[j].name, (meshes[i].mBonesArrOrdered[meshes[i].mBones[j].id])));

		}

		//delete me:
		LOG() << "iterating over bones in ordered bone array size:" << meshes[i].mBones.size();
		for (int j = 0; j < meshes[i].mBones.size(); j++) {
			LOG() << "Iterator: " << meshes[i].mBones[j].id << " name: " << meshes[i].mBonesArrOrdered[meshes[i].mBones[j].id]->name << " ID: " << meshes[i].mBonesArrOrdered[meshes[i].mBones[j].id]->id;
		}
	}
}