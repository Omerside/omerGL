#ifndef CAMERA_H
#define CAMERA_H
#include "glm/glm.hpp"

class Camera
{
public:
	glm::mat4 getViewMatrix() const;
	virtual void rotate(float yaw, float pitch) {} // degrees, not radians

protected:
	Camera();
	glm::vec3 mPosition;
	glm::vec3 mTargetPos;
	glm::vec3 mUp;

	//eulers angles (radians)
	float mYaw;
	float mPitch;
};


class OrbitCamera : public Camera {
public:
	OrbitCamera();
	virtual void rotate(float yaw, float pitch); // degrees, not radians
	void setLookAt(const glm::vec3& target);
	void setRadius(float radius);

private:
	void updateCameraVectors();
	float mRadius;
};


#endif