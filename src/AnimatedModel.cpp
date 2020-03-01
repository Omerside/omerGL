#include "AnimatedModel.h"
#include "Log.h"


AnimatedModel::AnimatedModel(ShaderProgram *shaderIn, std::string animationFile) {
	shader = shaderIn;

	//Load meshes into vector<Mesh> meshes (Model.h)
	loadModel(animationFile);

	//Assign bone hierarchy for animation
	AssignBonesHierarchyByNodes();

	//Load animation data including bones, poses. 
	//LoadAnimationData(animationFile);

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

	aiAnimation** animations = scene->mAnimations;
	for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
		processAnimNodes(animations[i], scene);
	}

	LOG() << "Finished loading animations.";
}

void AnimatedModel::processAnimNodes(aiAnimation *animation, const aiScene *scene)
{

	
	// process each node animation channels.
	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		LOG() << "Node name is: " << animation->mChannels[i]->mNodeName.C_Str();

	}

	// process each mesh animation channels.
	for (unsigned int i = 0; i < animation->mNumMeshChannels; i++)
	{
		LOG() << "Mesh name is: " << animation->mMeshChannels[i]->mName.C_Str();

	}

}

void AnimatedModel::AssignBonesHierarchyByNodes()
{
	LOG() << "Assigning bone hierarchy...";
	vector<Bone>* bones;
	for (int i = 0; i < meshes.size(); i++) {
		bones = meshes[i].GetBoneByRef();
		int id, parentId;
		string name;

		for (int j = 0; j < bones->size(); j++) {
			(*bones)[j].id = pNodeIdMap[((*bones)[j]).name].id;
			(*bones)[j].parentId = pNodeIdMap[((*bones)[j]).name].parentId;
			//name = out.name;pNodeIdMap
		}

		meshes[i].PrintBoneHierarchy();

	}

}