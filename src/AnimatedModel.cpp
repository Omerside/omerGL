#include "AnimatedModel.h"
#include "Log.h"


AnimatedModel::AnimatedModel(ShaderProgram *shaderIn, std::string animationFile) 
{
	shader = shaderIn;

	//Load meshes into vector<Mesh> meshes (Model.h)
	loadModel(animationFile);

	//Assign bone hierarchy for animation
	AssignBonesHierarchyByNodes();

	//Load animation data including bones, poses. 
	LoadAnimationData(animationFile);

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

	aiNodeAnim** channels = animation->mChannels;

	// process each node animation channels.
	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		LOG() << "Node name is: " << channels[i]->mNodeName.C_Str();
		vec3 value = aiVector3DToGlm(&channels[i]->mPositionKeys->mValue);
	}

	/**Transform vertices:
	aiMatrix4x4 tempMatrix = node->mTransformation;// .RotationX(0.4, m);
	tempMatrix.RotationX(-1.5, tempMatrix);
	tempMesh.TransformVertices(aiMatrix4x4ToGlm(&tempMatrix));
	**/
}

void AnimatedModel::AssignBonesHierarchyByNodes()
{
	LOG() << "Assigning bone hierarchy...";
	vector<Bone>* bones;
	for (int i = 0; i < meshes.size(); i++) {
		bones = meshes[i].GetBoneByRef();
		
		//childrenIdArray uses the ID of the bone as its location in the array
		vector<int>* childrenIdArray = new vector<int>[MAX_NUM_OF_BONES];
		
		
		for (int j = 0; j < bones->size(); j++) {
			//define bone id, parent ID
			(*bones)[j].id = pNodeIdMap[((*bones)[j]).name].id;
			(*bones)[j].parentId = pNodeIdMap[((*bones)[j]).name].parentId;
			
			//Store whose child this bone is for later assignment of childrenId array
			childrenIdArray[(*bones)[j].parentId].push_back((*bones)[j].id);

			//also take the opportunity to define the bones in the skeleton, now that we know the IDs
			skel.bones[(*bones)[j].id] = &(*bones)[j];
			skel.boneCount++;
		}
		
		for (int j = 0; j < bones->size(); j++) {
			//Assign list of children to bone based on ID
			(*bones)[j].childrenId = childrenIdArray[(*bones)[j].id];
		}
		
		meshes[i].PrintBoneHierarchy();

		delete[] childrenIdArray;
		
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