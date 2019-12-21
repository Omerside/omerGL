#ifndef CAMERA_H
#define CAMERA_H
#include "glm/glm.hpp"

using namespace glm;

class Camera
{
public:
	glm::mat4 getViewMatrix() const;
	virtual void rotate(float yaw, float pitch) {} // degrees, not radians
	void setCameraPositionVectors(float x, float y, float z);
	void setCameraPositionVectors(vec3 newCamPos);
	void setCameraTargetVectors(float x, float y, float z);
	void setCameraTargetVectors(vec3 newCamTarget);
	void setLookAt(glm::vec3& target);
	void setMoveSpeed(float speed);

	vec3 getPosition() const;
	vec3 getTarget();

protected:
	Camera();
	glm::vec3 mPosition;
	glm::vec3 mTargetPos;
	glm::vec3 mUp;


	//eulers angles (radians)
	float mYaw;
	float mPitch;
	float moveSpeed;


};


class OrbitCamera : public Camera {
public:
	OrbitCamera();
	virtual void rotateOnObject(float yaw, float pitch); // degrees, not radians
	void setRadius(float radius);

private:
	float mRadius;
};


class FirstPersonCamera : public Camera {
public:
	const enum DIRECTION {
		NONE,
		FORWARD,
		BACK,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	FirstPersonCamera();
	virtual void rotateOnCamera(float yaw, float pitch); // degrees, not radians
	void move(DIRECTION d);
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