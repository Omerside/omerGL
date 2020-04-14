#include "AnimatedModel.h"
#include "Log.h"


AnimatedModel::AnimatedModel(ShaderProgram *shaderIn, std::string animationFile) 
{
	shader = shaderIn;

	//Load meshes into vector<Mesh> meshes (Model.h)
	loadModel(animationFile);


	LOG(DEBUG) << ":: ENTERING PopulateSkeletalData() ::";
	//Load animation data including bones, poses. 
	PopulateSkeletalData();

	LOG(DEBUG) << ":: ENTERING LoadAnimationData(animationFile) ::";
	LoadAnimationData(animationFile);

	//Assign bone hierarchy for animation
	LOG(DEBUG) << ":: ENTERING AssignBonesHierarchyByNodes() ::";
	//AssignBonesHierarchyByNodes();
	PrintNodeHierarchy();


	LOG() << "Animated model loaded.";
	SetActiveSample(animations[0], 1);


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
	newAnimation.frameCount = animation->mDuration;
	newAnimation.fps = animation->mTicksPerSecond;
	newAnimation.isLooping = false;
	newAnimation.samples.resize(mMesh.maxBoneId);


	aiNodeAnim** nodeChannels = animation->mChannels;


	for (unsigned int i = 0; i < animation->mNumChannels; i++){
		int boneId = mMesh.GetBoneByName(nodeChannels[i]->mNodeName.C_Str()).id;
		LOG() << "AnimNode node name: " << nodeChannels[i]->mNodeName.C_Str() << " ID: " << boneId;
		Sample sample;
		sample.poses.resize(nodeChannels[i]->mNumRotationKeys);
		//We use mNumPositionKeys because, as per the assimp docs: 
		//If there are position keys, there will also be at least one scaling and one rotation key.
		for (unsigned int j = 0; j < nodeChannels[i]->mNumPositionKeys; j++) {
			sample.poses[j].rotation = quat(nodeChannels[i]->mRotationKeys[j].mValue.x,
				nodeChannels[i]->mRotationKeys[j].mValue.y,
				nodeChannels[i]->mRotationKeys[j].mValue.z,
				nodeChannels[i]->mRotationKeys[j].mValue.w);

			sample.poses[j].rotationMat = mat4(aiMatrix3x3ToGlm(nodeChannels[i]->mRotationKeys[j].mValue.GetMatrix()));
			sample.poses[j].scale = 1;
			//Need to multiply inverse bind pose by animation translation
			sample.poses[j].translation = (aiVector3DToGlm(nodeChannels[i]->mPositionKeys[j].mValue));
			sample.poses[j].translationMat = glm::translate(sample.poses[j].translation);
			sample.poses[j].transform = sample.poses[j].translationMat * sample.poses[j].rotationMat * sample.poses[j].scale;
		}

		newAnimation.samples[boneId] = (sample);
	}

	animations.push_back(newAnimation);
	/**Transform vertices:	**/
	//aiMatrix4x4 tempMatrix = node->mTransformation;// .RotationX(0.4, m);
	//tempMatrix.RotationX(-1.5, tempMatrix);

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

void  AnimatedModel::DrawModel(vec3 pos, int frame) {

	shader->SetUniformSampler("material.textureMap", 0);
	shader->SetUniformSampler("material.specularMap", 1);
	shader->SetUniform("material.ambient", vec3(1.0));
	shader->SetUniform("material.specular", vec3(1.0));
	shader->SetUniform("material.shininess", vec3(35.0));

	mat4 model = translate(mat4(), pos) * glm::scale(mat4(), scale);
	shader->SetUniform("model", model);
	shader->SetUniform("finalTransforms", finalTransforms);


	LOG() << "5) Calculating transformation";

	LOG() << "5) Drawing mesh.";
	mMesh.bindTextures();
	mMesh.draw();
	mMesh.unbindTextures();
	LOG() << "Finished drawing mesh.";
	
}

/*Requires that mBones is populated for all meshes*/
void AnimatedModel::PopulateSkeletalData() {
		
	for (int j = 0; j < mMesh.mBones.size(); j++) {
		//Store id->bone relationship in skeleton object
		//skeleton.bones[0][mMesh.mBones[j].id] = &(mMesh.mBones[j]);

		//Store id->bone relationship in bones array associated with mesh
		//These values will later be changed as we discover the bone hierarchy.
		mMesh.StoreBoneById(&(mMesh.mBones[j]), mMesh.mBones[j].id);
			
		//Store name->bone relationship in bones map associated with mesh
		mMesh.SetBoneByName(&mMesh.mBones[j], mMesh.mBones[j].name);

		//Store name->bone relationship in bones map associated with skeleton
		//skeleton.bonesMap[mMesh.mBones[j].name] = &(mMesh.mBones[j]);

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

void AnimatedModel::SetActiveSample(Clip clip, uint frame) {

	LOG() << "Setting local poses. Size of samples array: " << clip.samples.size();
	int boneId;

	for (int i = 0; i < mMesh.mBones.size(); i++) {
		
		boneId = mMesh.mBones[i].id;
		LOG() << "Bone ID: " << boneId;
		LOG() << "Checking if " << mMesh.mBones[i].name << " it has poses... ";
		if (!clip.samples[boneId].poses.empty()) {
			
			mMesh.SetLocalPose(boneId, mMesh.GenerateLocalPose(clip.samples[boneId].poses[frame]));
			
		}

	}

	LOG() << "Setting global poses";
	mMesh.SetGlobalPoses();
	SetFinalSkelTransforms();
}