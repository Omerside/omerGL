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
	PopulateSkeletalData();

	LOG() << "DONE?>!?!";

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

	Clip newAnimation;
	newAnimation.name = animation->mName.C_Str();
	newAnimation.frameCount = animation->mDuration;
	newAnimation.fps = animation->mTicksPerSecond;
	newAnimation.isLooping = false;
	newAnimation.samples.resize(maxBoneId);

	aiNodeAnim** nodeChannels = animation->mChannels;


	for (unsigned int i = 0; i < animation->mNumChannels; i++){
		Sample sample;
		sample.poses.resize(nodeChannels[i]->mNumRotationKeys);
		

		for (unsigned int j = 0; j < nodeChannels[i]->mNumRotationKeys; j++) {
			BonePose pose;
			pose.rotation = quat(nodeChannels[i]->mRotationKeys[j].mValue.x,
				nodeChannels[i]->mRotationKeys[j].mValue.y,
				nodeChannels[i]->mRotationKeys[j].mValue.z,
				nodeChannels[i]->mRotationKeys[j].mValue.w);

			pose.scale = 1;
			

			//Need to multiply inverse bind pose by animation translation
			pose.translation = (aiVector3DToGlm(nodeChannels[i]->mPositionKeys[j].mValue));

			sample.poses[j] = pose;
		}
	
		newAnimation.samples[skeleton.bonesMap[nodeChannels[i]->mNodeName.C_Str()]->id] = (sample);
		
	}

	animations.push_back(newAnimation);
	/*
	for (unsigned int i = 0; i < animation->mNumMeshChannels; i++){

		LOG() << "Mesh animation channel name is: " << meshChannels[i]->mName.C_Str();
		
	}
	*/


	/**Transform vertices:	**/
	//aiMatrix4x4 tempMatrix = node->mTransformation;// .RotationX(0.4, m);
	//tempMatrix.RotationX(-1.5, tempMatrix);
	//tempMesh.TransformVertices(aiMatrix4x4ToGlm(&tempMatrix));

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
void AnimatedModel::PopulateSkeletalData() {
	for (int i = 0; i < meshes.size(); i++) {
		
		for (int j = 0; j < meshes[i].mBones.size(); j++) {
			//Store id->bone relationship in skeleton object
			skeleton.bones[i][meshes[i].mBones[j].id] = &(meshes[i].mBones[j]);

			//Store id->bone relationship in bones array associated with mesh
			meshes[i].StoreBoneById(&(meshes[i].mBones[j]), meshes[i].mBones[j].id);
			
			//Store name->bone relationship in bones map associated with mesh
			meshes[i].bonesMap.insert(pair<string, Bone*>(meshes[i].mBones[j].name, (meshes[i].mBonesArrOrdered[meshes[i].mBones[j].id])));

			//Store name->bone relationship in bones map associated with skeleton
			skeleton.bonesMap[meshes[i].mBones[j].name] = &(meshes[i].mBones[j]);

			//increment total number of bones
			skeleton.boneCount++;
		}

		//get min and max ID values
		if (minBoneId > meshes[i].minBoneId) { minBoneId = meshes[i].minBoneId; }
		if (maxBoneId < meshes[i].maxBoneId) { maxBoneId = meshes[i].maxBoneId; }
		maxBoneId++; // increment to avoid off-by-1 errors
	}


	//Define global poses
	skeleton.globalPoses.resize(meshes.size());

	for (int i = 0; i < meshes.size(); i++) {
		skeleton.globalPoses[i] = new mat4[maxBoneId];

		if (skeleton.bones[i][0] != NULL) {  // If the mesh has bones...
			for (int j = 0; j < meshes[i].mBones.size(); j++) {
				LOG() << "Calculating global pose for: " << meshes[i].mBones[j].name;
				skeleton.globalPoses[i][meshes[i].mBones[j].id] = GetGlobalPose(i, meshes[i].mBones[j].id);
				LOG() << "DONE Calculating global pose for: " << meshes[i].mBones[j].name;
				meshes[i].mBones[j].globalPose = &(skeleton.globalPoses[i][j]);
			}
		}
	}
}

/*Recalculate local pose
*Assumes a local pose already exists.
*/
mat4 AnimatedModel::CalcLocalPose(BonePose pose) {

	//affine transformation begins with Scale and Rotation calculation:
	mat3 transMatrix = (pose.scale*toMat3(pose.rotation));
	
	//Applued to the complete local pose:
	mat4 localPose;
	localPose[0] = vec4(transMatrix[0], 0);
	localPose[1] = vec4(transMatrix[1], 0);
	localPose[2] = vec4(transMatrix[2], 0);
	localPose[3] = vec4(pose.translation, 1);

}


/*
*Define global poses.
*This requires that Local Poses are defined.
*/
mat4 AnimatedModel::GetGlobalPose(int meshId, int boneId) {
	if (boneId == -1) { return mat4(1.0f); } // return of we've reached the root, though we should never get here.

	Bone bone = meshes[meshId].GetBoneById(boneId);

	LOG() << "In GetGlobalPose - ID: " << bone.id;
	LOG() << "In GetGlobalPose - Parent ID: " << bone.parentId;




	if (bone.parentId <= minBoneId) {
		return bone.invBindPose;
	}

	Bone parentBone = meshes[meshId].GetBoneById(bone.parentId);

	//Transform local pose with parent local pose
	mat4 newLocalPose = *(bone.localPose) * *(parentBone.localPose);

	//multiply our new local by the local of the parent ID
	return (newLocalPose * GetGlobalPose(meshId, parentBone.id));
}

void AnimatedModel::SetActiveSample(Clip clip, uint frame) {

}