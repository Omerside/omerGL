#ifndef ANIMATEDMODEL_H
#define ANIMATEDMODEL_H
#include "Model.h"

using namespace glm;

struct Sample {
	vector<BonePose> poses;	//The ID of the pose should correlate with the frame in the animation
};

struct Clip {
	string name;
	f32 fps;
	uint frameCount;
	vector<Sample> samples; //The ID of the pose should correlate with the bone ID
	bool isLooping;
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
	void PopulateSkeletalData();
	mat4 CalcLocalPose(BonePose pose);
	mat4 GetGlobalPose(int meshId, int boneId);
	void SetActiveSample(Clip clip, uint frame);

	vector<Clip> animations;
};

#endif

