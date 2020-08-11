#include "Camera.h"
#include "Log.h"
#include "glm/gtx/transform.hpp"



Camera::Camera() :
	mTargetPos(glm::vec3(0.0f, 0.0f, 0.0f)),
	mUp(0.0f, 1.0f, 0.0f),
	mYaw(0.0f),
	mPitch(0.0f),
	moveSpeed(0.25f)
{
	setLookAt(mTargetPos);
}

void Camera::setCameraPositionVectors(float x, float y, float z) {
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;

}

void Camera::setCameraPositionVectors(vec3 newCamPos) {
	mPosition = newCamPos;

}

void Camera::setCameraTargetVectors(float x, float y, float z) {
	mTargetPos.x = x;
	mTargetPos.y = y;
	mTargetPos.z = z;
}

void Camera::setCameraTargetVectors(vec3 newCamTarget) {
	mTargetPos = newCamTarget;

}


vec3 Camera::getPosition() const{
	return mPosition;
}

void Camera::setMoveSpeed(float speed) {
	moveSpeed = speed;
}

vec3 Camera::getTarget() {
	return mTargetPos;
}

glm::mat4 Camera::getViewMatrix() const {
	return glm::lookAt(mPosition, mTargetPos, mUp);
}


void Camera::setLookAt(vec3& target) {

	mTargetPos = target;
	mLook = normalize(getPosition() - mTargetPos);
}

void Camera::setLookAt() {
	mLook = normalize(getPosition() - mTargetPos);
}


 const vec3& Camera::getLook() const {
	return mLook;
}

OrbitCamera::OrbitCamera() 
	:mRadius(10.0f)
{

}


void OrbitCamera::setRadius(float radius) {
	mRadius = glm::clamp(radius, 2.0f, 80.0f); // set radius min-max value
}

void OrbitCamera::rotateOnObject(float yaw, float pitch) {

	LOG(INFO) << "OrbitCamera::rotateOnObject - Rotating on object, yaw/pitch: " << yaw << ", " << pitch;
	mYaw = glm::radians(yaw);

	//mPitch = glm::radians(pitch);
	mPitch = glm::clamp(radians(pitch), -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);

	float x, y, z;
	z = mRadius * cosf(mPitch) * cosf(mYaw);
	y = mRadius * sinf(mPitch);
	x = mRadius * cosf(mPitch) * sinf(mYaw);

	setCameraPositionVectors(x, y, z);

	//spherical (regular numbers) to cartesian coords
	mTargetPos = glm::normalize(mTargetPos); (
		mTargetPos.x - x,
		mTargetPos.y - y,
		mTargetPos.z - z
	);

	//mLook = normalize(mTargetPos);
}

FirstPersonCamera::FirstPersonCamera():
	direction(NONE) {

}


void FirstPersonCamera::updateLookVector() {

	// Calculate the view direction vector based on yaw and pitch angles (roll not considered)
// radius is 1 for normalized length
	glm::vec3 look;
	look.z = cosf(mPitch) * sinf(mYaw);
	look.y = sinf(mPitch);
	look.x = cosf(mPitch) * cosf(mYaw);

	mLook = normalize(look);
	vec3 mRight = glm::normalize(glm::cross(mLook, vec3(0.0f, 1.0f, 0.0f)));
}


void FirstPersonCamera::rotateOnCamera(float yaw, float pitch) {

	if (pitch > 89.0f) {
		pitch = 89.0f;
	} else if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	mYaw = glm::radians(yaw);
	mPitch = glm::radians(pitch);
	float x, y, z;

	x = cos(mPitch)*cos(mYaw);
	y = sin(mPitch);
	z = cos(mPitch)*sin(mYaw);

	setCameraPositionVectors(x, y, z);
	mTargetPos = normalize(vec3(x, y, z)) + mPosition;
	//updateLookVector();
}

vec3 FirstPersonCamera::calcTargetPosRotationOnCamera(float yaw, float pitch) {
	if (yaw == 0.0f && pitch == 0.0f) {
		return mTargetPos;
	}

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	else if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	mYaw = glm::radians(yaw);
	mPitch = glm::radians(pitch);
	float x, y, z;

	x = cos(mPitch)*cos(mYaw);
	y = sin(mPitch);
	z = cos(mPitch)*sin(mYaw);

	LOG(DEBUG) << "Rotation on camera is: " << (normalize(vec3(x, y, z))) + mPosition;
	return (normalize(vec3(x, y, z))) + mPosition;

}


void FirstPersonCamera::resetDirection() {
	direction = NONE;

}

void FirstPersonCamera::ExecuteMove() {
	if (direction == NONE) { return; }

	vec3 newTarget = mTargetPos;
	vec3 newPos = mPosition;
	vec3 cameraDirection = normalize(mPosition - mTargetPos);
	vec3 cameraRight = normalize(cross(mUp, cameraDirection));
	vec3 cameraUp = normalize(cross(cameraDirection, cameraRight));


	if (direction == FORWARD) {
		newPos = (mPosition -= moveSpeed * (cameraDirection));

	}
	else if (direction == BACK) {
		newPos = (mPosition += moveSpeed * (cameraDirection));

	}

	mTargetPos = newTarget;
	setCameraPositionVectors(newPos);
	updateLookVector();
}

void FirstPersonCamera::ExecuteMove(DIRECTION d) {
	if (d == NONE) { return; }

	vec3 newTarget = mTargetPos;
	vec3 newPos = mPosition;
	vec3 cameraDirection = normalize(mPosition - mTargetPos);
	vec3 cameraRight = normalize(cross(mUp, cameraDirection));
	vec3 cameraUp = normalize(cross(cameraDirection, cameraRight));

	

	if (d == FORWARD) {
		newPos = (mPosition -= moveSpeed*(cameraDirection));
		newTarget = mTargetPos + cameraDirection;

	} else if (d == BACK) {
		newPos = (mPosition += moveSpeed*(cameraDirection));
		newTarget = mTargetPos + cameraDirection;
	}
	else if (direction == RIGHT) {
		newPos = (mPosition += moveSpeed * cameraRight);
		newTarget += moveSpeed * cameraRight;

	}
	else if (direction == LEFT) {
		newPos = (mPosition -= moveSpeed * cameraRight);
		newTarget -= moveSpeed * cameraRight;
	}


	mTargetPos = newTarget;
	setCameraPositionVectors(newPos);
	updateLookVector();
}



void FirstPersonCamera::ExecuteMove(float yaw, float pitch) {
	if (direction == NONE && yaw == 0.0f && pitch == 0.0f) {
		return;
	}


	mTargetPos = calcTargetPosRotationOnCamera(yaw, pitch);
	vec3 newPos = mPosition;


	// = getPosition();// = calcTargetPosRotationOnCamera(yaw, pitch);//mPosition;
	vec3 cameraDirection = normalize(mPosition - mTargetPos);
	vec3 cameraRight = normalize(cross(mUp, cameraDirection));
	vec3 cameraUp = normalize(cross(cameraDirection, cameraRight));
	

	if (direction == FORWARD) {
		newPos = (mPosition -= moveSpeed * (cameraDirection));

	}
	else if (direction == BACK) {
		newPos = (mPosition += moveSpeed * (cameraDirection));

	}
	else if (direction == RIGHT) {
		newPos = (mPosition += moveSpeed * cameraRight);
		mTargetPos += moveSpeed * cameraRight;

	}
	else if (direction == LEFT) {
		newPos = (mPosition -= moveSpeed * cameraRight);
		mTargetPos -= moveSpeed * cameraRight;
	} 
	else if (direction == UP) {
		newPos = (mPosition += moveSpeed * cameraUp);
		mTargetPos += moveSpeed * cameraUp;
	
	}


	setCameraPositionVectors(newPos);
	setLookAt(mTargetPos);
	updateLookVector();

}

void FirstPersonCamera::ExecuteMove(float yaw, float pitch, DIRECTION directionInput) {
	vec3 newTarget = calcTargetPosRotationOnCamera(yaw, pitch);
	vec3 newPos = mPosition;


	if (directionInput == NONE) { 
		mTargetPos = newTarget;
		updateLookVector();
		return; 
	} else {

		vec3 cameraDirection = normalize(mPosition - newTarget);
		vec3 cameraRight = normalize(cross(mUp, cameraDirection));
		vec3 cameraUp = normalize(cross(cameraDirection, cameraRight));


		if (directionInput == FORWARD) {
			newPos = (mPosition -= moveSpeed * (cameraDirection));

		}
		else if (directionInput == BACK) {
			newPos = (mPosition += moveSpeed * (cameraDirection));

		}
		else if (direction == RIGHT) {
			newPos = (mPosition += moveSpeed * cameraRight);
			newTarget += moveSpeed * cameraRight;

		}
		else if (direction == LEFT) {
			newPos = (mPosition -= moveSpeed * cameraRight);
			newTarget -= moveSpeed * cameraRight;
		}

		setCameraPositionVectors(newPos);
		mTargetPos = newTarget + cameraDirection;
		updateLookVector();

	}
}

void FirstPersonCamera::move(DIRECTION d) {
	direction = d;
}