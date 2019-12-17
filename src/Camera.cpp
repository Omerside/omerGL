#include "Camera.h"
#include "glm/gtx/transform.hpp"



Camera::Camera()
	:mPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
	mTargetPos(glm::vec3(0.0f, 0.0f, 0.0f)),
	mUp(0.0f, 1.0f, 0.0f),
	mYaw(0.0f),
	mPitch(0.0f),
	moveSpeed(0.25f)
{
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


vec3 Camera::getPosition() {
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


void Camera::setLookAt(glm::vec3& target) {
	mTargetPos = target;
}

OrbitCamera::OrbitCamera() 
	:mRadius(10.0f)
{

}


void OrbitCamera::setRadius(float radius) {
	mRadius = glm::clamp(radius, 2.0f, 80.0f); // set radius min-max value
}

void OrbitCamera::rotateOnObject(float yaw, float pitch) {
	mYaw = glm::radians(yaw);

	mPitch = glm::radians(pitch);
	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);

	//spherical (regular numbers) to cartesian coords
	mTargetPos = glm::normalize(mTargetPos); (
		mTargetPos.x + mRadius * cosf(mPitch) * sinf(mYaw),
		mTargetPos.y + mRadius * sinf(mPitch),
		mTargetPos.z + mRadius * cosf(mPitch) * cosf(mYaw)
	);
}

FirstPersonCamera::FirstPersonCamera():
	direction(NONE) {}

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

	//setCameraPositionVectors(x, y, z);
	mTargetPos = normalize(vec3(x, y, z)) + mPosition;

}

vec3 FirstPersonCamera::calcTargetPosRotationOnCamera(float yaw, float pitch) {

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

	return normalize(vec3(x, y, z)) + mPosition;

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
}



void FirstPersonCamera::ExecuteMove(float yaw, float pitch) {
	vec3 newTarget = calcTargetPosRotationOnCamera(yaw, pitch);
	vec3 newPos = mPosition;
	

	if (direction == NONE) {
		mTargetPos = newTarget;
		return;
	} else {

		vec3 cameraDirection = normalize(mPosition - newTarget);
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
			newTarget += moveSpeed * cameraRight;

		}
		else if (direction == LEFT) {
			newPos = (mPosition -= moveSpeed * cameraRight);
			newTarget -= moveSpeed * cameraRight;
		}
		
		setCameraPositionVectors(newPos);
		mTargetPos = newTarget - cameraDirection;

	}
}

void FirstPersonCamera::ExecuteMove(float yaw, float pitch, DIRECTION directionInput) {
	vec3 newTarget = calcTargetPosRotationOnCamera(yaw, pitch);
	vec3 newPos = mPosition;


	if (directionInput == NONE) { 
		mTargetPos = newTarget;
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

	}
}

void FirstPersonCamera::move(DIRECTION d) {
	direction = d;
}