#ifndef ANIMATEDMODEL_H
#define ANIMATEDMODEL_H
#include "Model.h"

using namespace glm;

//Defines pose of a bone at a given frame in the animation.
struct Sample {
	vector<BonePose> poses;	//The ID of the pose should correlate with the frame in the animation
};

//Defines collection of poses for all bones which are part of an animation.
struct Clip {
	aiAnimation* animation = new aiAnimation();
	string name;
	f32 fps;
	uint* frameCount;
	vector<Sample> samples; //The ID of the sample should correlate with the bone ID
	bool isLooping;
};

class AnimatedModel : public Model {
public:
	AnimatedModel(ShaderProgram *shaderIn, std::string animationFile);
	~AnimatedModel();
	void LoadAnimationData(string const &path);
	void DrawModel(vec3 pos, int frame);
	void PrintFinalSkelTransforms();

private:
	void processAnimNodes(aiAnimation *node, const aiScene *scene);
	void AssignBonesHierarchyByNodes();
	void PopulateSkeletalData();
	mat4 CalcLocalPose(BonePose pose);
	mat4 GetGlobalPose(int meshId, int boneId);
	void SetActiveSample(Clip clip, uint frame);

	vector<Clip> animations;
};

#endif

