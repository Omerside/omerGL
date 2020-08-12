#ifndef CAMERA_H
#define CAMERA_H
#include "WorldObject.h"
#include "ControllerDefinitions.h"
#include "glm/glm.hpp"


using namespace glm;

class Camera : protected WorldObject {
public:
	glm::mat4 getViewMatrix() const;
	virtual void rotate(float yaw, float pitch) {} // degrees, not radians
	void setCameraPositionVectors(float x, float y, float z);
	void setCameraPositionVectors(vec3 newCamPos);
	void setCameraTargetVectors(float x, float y, float z);
	void setCameraTargetVectors(vec3 newCamTarget);
	void setLookAt(glm::vec3& target);
	void setLookAt();
	void setMoveSpeed(float speed);
	const vec3& getLook() const;

	vec3 getPosition() const;
	vec3 getTarget();
	vec2 getYawPitch() {
		return vec2(mYaw, mPitch);
	};


protected:
	Camera();
	//glm::vec3 mPosition;
	glm::vec3 mTargetPos;
	glm::vec3 mUp;
	glm::vec3 mLook;


	//eulers angles (radians)
	float mYaw;
	float mPitch;
	float moveSpeed;


};


class OrbitCamera : public Camera {
public:
	OrbitCamera();
	virtual void rotateOnObject(float yaw, float pitch); // degrees, not radians
	virtual void rotateOnObject(float yaw, float pitch, vec3 &target);
	void setRadius(float radius);

private:
	float mRadius;
};


class FirstPersonCamera : public Camera {
public:


	FirstPersonCamera();
	virtual void rotateOnCamera(float yaw, float pitch); // degrees, not radians
	void move(DIRECTION d);
	void updateLookVector();
	void resetDirection();
	void ExecuteMove();
	void ExecuteMove(DIRECTION d);
	void ExecuteMove(float yaw, float pitch);
	void ExecuteMove(float yaw, float pitch, DIRECTION directionInput);
	

private: 
	vec3 calcTargetPosRotationOnCamera(float yaw, float pitch);
	DIRECTION direction;
};

#endif