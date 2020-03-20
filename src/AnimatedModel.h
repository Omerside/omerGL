#ifndef ANIMATEDMODEL_H
#define ANIMATEDMODEL_H
#include "Model.h"

using namespace glm;


struct Skeleton
{
	int boneCount = 0; // number of bones
	Bone* bones[MAX_NUM_OF_BONES]; // array of joints
};

struct SkeletonPose {
	vector<BonePose*> localPoses;
	vector<mat4*> globalPoses;
};

class AnimatedModel : public Model {
public:
	AnimatedModel(ShaderProgram *shaderIn, std::string animationFile);
	~AnimatedModel();
	void LoadAnimationData(string const &path);
	void  DrawModel(vec3 pos, int frame);


private:
	void processAnimNodes(aiAnimation *node, const aiScene *scene);
	void AssignBonesHierarchyByNodes();
	void PopulateBonesArrayAndMap(); // Populate mBonesAr based on the IDs mentioned in mBones

	
	Skeleton skel;
	SkeletonPose skelPose;
};

#endif

