#include "AnimatedModel.h"
#include "Log.h"


AnimatedModel::AnimatedModel(ShaderProgram *shaderIn, std::string animationFile)
{
	shader = shaderIn;

	//Load meshes into vector<Mesh> meshes (Model.h)
	loadModel(animationFile);


	LOG(DEBUG) << ":: ENTERING LoadAnimationData(animationFile) ::";
	LoadAnimationData(animationFile);

	//Assign bone hierarchy for animation

	//PrintNodeHierarchy();


	LOG() << "Animated model loaded.";
	//SetActiveSample(&animations[0], 7);
	//

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
	newAnimation.animation = animation;
	newAnimation.name = animation->mName.C_Str();
	newAnimation.frameCount = new uint[animation->mNumChannels];
	newAnimation.fps = animation->mTicksPerSecond/26;
	newAnimation.isLooping = false;
	newAnimation.samples.resize(mMesh.maxBoneId);


	aiNodeAnim** nodeChannels = animation->mChannels;
	

	LOG(DEBUG) << "processAnimNodes : Number of animations: " << animation->mNumChannels;
	for (unsigned int i = 0; i < animation->mNumChannels; i++){
		newAnimation.frameCount[i] = nodeChannels[i]->mNumPositionKeys;
		int boneId = mMesh.GetBoneByName(nodeChannels[i]->mNodeName.C_Str()).id;
		Sample sample;

		LOG(DEBUG) << "processAnimNodes : AnimNode node name: " << nodeChannels[i]->mNodeName.C_Str() << " ID: " << boneId;
		LOG(DEBUG) << "processAnimNodes :  number of poses: " << newAnimation.frameCount[i];

		sample.poses.resize(newAnimation.frameCount[i]);//(nodeChannels[i]->mNumRotationKeys);
		//We use mNumPositionKeys because, as per the assimp docs: 
		//If there are position keys, there will also be at least one scaling and one rotation key.
		for (unsigned int j = 0; j < newAnimation.frameCount[i]; j++){//nodeChannels[i]->mNumPositionKeys; j++) {
			LOG(DEBUG) << "-- number of poses: " << newAnimation.frameCount[i];
																	  //LOG() << "--- FRAME NUMBER " << j << " TRANSFORM DATA --- ";
			sample.poses[j].rotation = quat(nodeChannels[i]->mRotationKeys[j].mValue.x,
				nodeChannels[i]->mRotationKeys[j].mValue.y,
				nodeChannels[i]->mRotationKeys[j].mValue.z,
				nodeChannels[i]->mRotationKeys[j].mValue.w);

			//LOG() << "Rotation       :\n" << sample.poses[j].rotation;

			sample.poses[j].rotationMat = mat4(aiMatrix3x3ToGlm(nodeChannels[i]->mRotationKeys[j].mValue.GetMatrix()));
			//LOG() << "\n\n---Rotation Matrix:\n" << sample.poses[j].rotationMat;
			sample.poses[j].scale = 1;

			//Need to multiply inverse bind pose by animation translation
			sample.poses[j].translation = (aiVector3DToGlm(nodeChannels[i]->mPositionKeys[j].mValue));
			//LOG() << "\n\n---Translation :\n" << sample.poses[j].translation;

			sample.poses[j].translationMat = glm::translate(sample.poses[j].translation);
			//LOG() << "\n\n---Translation Matrix:\n" << sample.poses[j].translationMat;

			sample.poses[j].transform = sample.poses[j].translationMat * sample.poses[j].rotationMat * sample.poses[j].scale;
			//LOG(INFO) << "\n\n---transform Matrix:\n" << sample.poses[j].transform;
		}
		

		newAnimation.samples[boneId] = (sample);
		LOG(DEBUG) << "Finished loading animations for node name: " << nodeChannels[i]->mNodeName.C_Str() << " mloc: " << &newAnimation.samples[boneId];
	}
	
	animations.push_back(newAnimation);
	LOG(DEBUG) << "Finished loading all animation data. mloc: " << &animations.back();
	/**Transform vertices:	**/

}

void AnimatedModel::AssignBonesHierarchyByNodes()
{
	LOG() << "Assigning bone hierarchy... node map size: " << nodesMap.size();
	for (std::map<string, int>::iterator it = nodesMap.begin(); it != nodesMap.end(); ++it)
		std::cout << it->first << " => " << it->second << '\n';
	
	//nodeTransforms = new mat4[nodesMap.size()];

	vector<int>* childrenIdArray = new vector<int>[MAX_NUM_OF_BONES];
	vector<vector<Sample>> orderedSamples;
	orderedSamples.resize(animations.size());

	for_each(orderedSamples.begin(), orderedSamples.end(), [](vector<Sample> &samples) {
		samples.resize(MAX_NUM_OF_BONES);
	});

	orderedSamples.resize(mMesh.maxBoneId);		
	mMesh.PrintBoneHierarchy();
	
}

void AnimatedModel::DrawAnimationFrame() {
	if (activeAnimationID == -1) {
		LOG(DEBUG) << "activeAnimationFrame is zero - returning.";
		return;
	}
	if (deltaTime < animations[activeAnimationID].fps) {
		LOG(DEBUG) << "delta time is less than the fps - returning.";
		return;
	}
	else {
		deltaTime = 0;
	}

	if (animations[activeAnimationID].isLooping) {
		LOG(DEBUG) << "DrawAnimationFrame :: Current frame = " << animations[activeAnimationID].currentFrame;
		LOG(DEBUG) << "DrawAnimationFrame :: Max frame = " << animations[activeAnimationID].frameCount[0];

		SetActiveSample(&animations[activeAnimationID], animations[activeAnimationID].currentFrame);
		if (animations[activeAnimationID].currentFrame == animations[activeAnimationID].frameCount[0] - 1) {
			animations[activeAnimationID].currentFrame = 0;
		}
	}
	else if (animations[activeAnimationID].currentFrame == animations[activeAnimationID].frameCount[0] - 1) {
		animations[activeAnimationID].currentFrame = 0;
		activeAnimationID = -1;
		return;
	}
	else {
		LOG(DEBUG) << "Animation found but it does not loop.";
		SetActiveSample(&animations[activeAnimationID], animations[activeAnimationID].currentFrame);
	}

	animations[activeAnimationID].currentFrame++;
}

/*void AnimatedModel::DrawAnimationFrame() {
	if (!activeAnimation) return;
	if (deltaTime < activeAnimation->fps) {
		return;
	}
	else {
		deltaTime = 0;
	}
	 
	if (activeAnimation->isLooping) {
		LOG(DEBUG) << "DrawAnimationFrame :: Current frame = " << activeAnimation->currentFrame;
		LOG(DEBUG) << "DrawAnimationFrame :: Max frame = " << activeAnimation->frameCount[0];
		
		SetActiveSample(activeAnimation, activeAnimation->currentFrame);
		if (activeAnimation->currentFrame == activeAnimation->frameCount[0]-1) {
			activeAnimation->currentFrame = 0;
		}
	}
	else if (activeAnimation->currentFrame == activeAnimation->frameCount[0]-1) {
		activeAnimation->currentFrame = 0;
		activeAnimation = NULL;
		return;
	}
	else {
		SetActiveSample(activeAnimation, activeAnimation->currentFrame);
	}

	activeAnimation->currentFrame++;
}*/

void  AnimatedModel::DrawModel(vec3 pos, uint frame, double dTime) {
	deltaTime += dTime;
	LOG(DEBUG) << "Entering drawAnimationFrame for " << directory;
	DrawAnimationFrame();

	LOG(DEBUG) << "SETTING SHADER VALUES" << directory;
	shader->SetUniformSampler("material.textureMap", 0);
	shader->SetUniformSampler("material.specularMap", 1);
	shader->SetUniform("material.ambient", vec3(1.0));
	shader->SetUniform("material.specular", vec3(1.0));
	shader->SetUniform("material.shininess", vec3(35.0));

	mat4 model = translate(mat4(), pos) * glm::scale(mat4(), scale);
	model = rotate(model, rotation, vec3(0, 1, 0));
	shader->SetUniform("model", model);
	LOG(DEBUG) << "Model information:  " << model;

	//PrintFinalSkelTransforms();
	//mMesh.PrintVertexWeightArray();
	//Matrix, angle, direction
	//mat4 oldMatrix = finalTransforms[frame];
	//finalTransforms[frame] = rotate(finalTransforms[frame], (GLfloat) .5, vec3(0, 0, 1));
	shader->SetUniform("finalTransforms", finalTransforms, MAX_NUM_OF_BONES);
	

	LOG() << "5) Calculating transformation";

	LOG() << "5) Drawing mesh.";
	mMesh.bindTextures();
	mMesh.draw();
	mMesh.unbindTextures();
	LOG() << "Finished drawing mesh.";
	
	//finalTransforms[frame] = oldMatrix;
}

/*Requires that mBones is populated for all meshes*/
void AnimatedModel::PopulateSkeletalData() {
		
	for (int j = 0; j < mMesh.mBones.size(); j++) {

		mMesh.StoreBoneById(&(mMesh.mBones[j]), mMesh.mBones[j].id);
		mMesh.SetBoneByName(&mMesh.mBones[j], mMesh.mBones[j].name);

	}
}

/*Recalculate local pose
*Assumes a local pose already exists.
*/
mat4 AnimatedModel::CalcLocalPose(BonePose pose) {

	//affine transformation begins with Scale and Rotation calculation:
	mat3 transMatrix = (pose.scale*toMat3(pose.rotation));
	
	//Applied to the complete local pose:
	mat4 localPose;
	localPose[0] = vec4(transMatrix[0], 0);
	localPose[1] = vec4(transMatrix[1], 0);
	localPose[2] = vec4(transMatrix[2], 0);
	localPose[3] = vec4(pose.translation, 1);

	return localPose;
}


/*
*Define global poses.
*This requires that Local Poses are defined.
*/
mat4 AnimatedModel::GetGlobalPose(int meshId, int boneId) {
	if (boneId == -1) { return mat4(1.0f); } // return of we've reached the root, though we should never get here.

	Bone bone = mMesh.GetBoneById(boneId);


	if (bone.parentId <= mMesh.minBoneId) {
		return bone.nodeTransform;
	}

	Bone parentBone = mMesh.GetBoneById(bone.parentId);

	//Transform local pose with parent local pose
	mat4 newLocalPose = (bone.localPose) * (parentBone.localPose);

	//multiply our new local by the local of the parent ID
	return (newLocalPose * GetGlobalPose(meshId, parentBone.id));
}

void AnimatedModel::SetActiveSample(Clip* clip, uint frame) {

	LOG(DEBUG) << "Setting local poses. Size of samples array: " << clip->samples.size() << " - FRAME NUM: " << frame;
	LOG(DEBUG) << "Clip data: " << clip->name;
	int boneId;

	for (int i = 0; i < mMesh.mBones.size(); i++) {
		
		boneId = mMesh.mBones[i].id;

		LOG(DEBUG) << "Checking if " << mMesh.mBones[i].name << " it has poses...  Bone ID: " << boneId;
		if (!clip->samples[boneId].poses.empty()) {
			LOG(DEBUG) << "setting local pose (size: " << clip->samples[boneId].poses[frame].transform << ")";
			 
			mMesh.SetLocalPose(boneId, clip->samples[boneId].poses[frame].transform);

		} else {
			LOG(DEBUG) << "Pose not found - setting local pose to node transform.";
			mMesh.SetLocalPose(boneId, mMesh.mBones[i].nodeTransform);
		}

	}

	LOG(DEBUG) << "Setting global poses";
	mMesh.SetGlobalPoses();
	SetFinalSkelTransforms();

}

void AnimatedModel::SetActiveSample(Clip clip, uint frame) {

	LOG(DEBUG) << "Setting local poses. Size of samples array: " << clip.samples.size();
	LOG(DEBUG) << "Clip data: " << clip.name;
	int boneId;

	for (int i = 0; i < mMesh.mBones.size(); i++) {

		boneId = mMesh.mBones[i].id;

		LOG(DEBUG) << "Checking if " << mMesh.mBones[i].name << " it has poses...  Bone ID: " << boneId;
		if (!clip.samples[boneId].poses.empty()) {
			LOG(DEBUG) << "setting local pose (size: " << clip.samples[boneId].poses[frame].transform << ")";

			mMesh.SetLocalPose(boneId, clip.samples[boneId].poses[frame].transform);

		}
		else {
			LOG(DEBUG) << "Pose not found - setting local pose to node transform.";
			mMesh.SetLocalPose(boneId, mMesh.mBones[i].nodeTransform);
		}

	}

	LOG(DEBUG) << "Setting global poses";
	mMesh.SetGlobalPoses();
	SetFinalSkelTransforms();

}

bool AnimatedModel::SetActiveAnimation(string animName, bool isLooping) {
	for (int i = 0; i < animations.size(); i++) {
		if (animations[i].name == animName) {
			LOG(DEBUG) << "SetActiveAnimation :: assigning " << animName << " as active animation for ";
			animations[i].isLooping = isLooping;
			animations[i].currentFrame = 0;
			//activeAnimation = &animations[i];
			activeAnimationID = i;
			LOG(DEBUG) << "SetActiveAnimation :: SET " << animations[activeAnimationID].name << " as active animation.";
		}
	}
}

void AnimatedModel::UnsetActiveAnimation() {
	animations[activeAnimationID].currentFrame = 0;
	activeAnimationID = -1;
	//activeAnimation->currentFrame = 0;
	//activeAnimation = NULL;
}

void AnimatedModel::PrintFinalSkelTransforms() {
	LOG() << "Printing final bone transformation: ";
	for (int i = 0; i < finalTransforms->length(); i++) {
		LOG() << finalTransforms[i];
	}
}