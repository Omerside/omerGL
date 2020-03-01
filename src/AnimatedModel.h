#ifndef ANIMATEDMODEL_H
#define ANIMATEDMODEL_H
#include "Model.h"

using namespace glm;

struct Skeleton
{
	int jointCount; // number of joints
	vector<Bone*> bones; // array of joints
};

struct SkeletonPose {
	Skeleton* pSkeleton; // pointer to skeleton
	vector<BonePose*> localPoses;
};

class AnimatedModel : public Model {
public:
	AnimatedModel(ShaderProgram *shaderIn, std::string animationFile);
	~AnimatedModel();
	void LoadAnimationData(string const &path);


private:
	void processAnimNodes(aiAnimation *node, const aiScene *scene);
	void AssignBonesHierarchyByNodes();
	Skeleton skel;
	SkeletonPose skelPose;
};

#endif

