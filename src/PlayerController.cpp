#include "PlayerController.h"


PlayerController* PlayerController::inst = NULL;

PlayerController::PlayerController() {
	InitPlayerCamera();
}

PlayerController* PlayerController::getInstance() {
	if (inst == NULL) {
		inst = new PlayerController();
	}

	return(inst);
}

void PlayerController::InitPlayerCamera(){
	vec3 temp = vec3(0, -0.8, -0.5);
	//TODO: Make a dynamic method for initializing position.
	playerCamera.setMoveSpeed(0.25f);
	playerCamera.setCameraPositionVectors(vec3(0.35f, 4.6f, 10.0f));
	SetLook(vec3(0.1));
	//ExecuteCameraMove(0.01, 0.01);
	

	//playerCamera
	//playerCamera.ExecuteMove(pitch, yaw);

	
}

MouseProperties PlayerController::CalcNewMouseProperties(MouseProperties in) {
	//we will need to calculate the actual initial yaw and pitch depending on our starting look-at coordinates.
	//otherwise the camera will simply snap to place.

	return in;
}

void PlayerController::ExecuteCameraMove(float yaw, float pitch) {

	playerCamera.ExecuteMove(yaw, pitch);
	//LOG(DEBUG) << "Yaw and pitch: " << playerCamera.getYawPitch().x << ", " << playerCamera.getYawPitch().y;

}

void PlayerController::MoveDirection(DIRECTION d) {
	//LOG(DEBUG) << "attempting to move player in direction " << d;
	playerCamera.move(d);
}

void PlayerController::MoveDirection(KeyAction d, int action) {

	if (action == GLFW_PRESS) {
		MoveDirection(d.playerActionOnPress);
	}
	else if (action == GLFW_RELEASE) {
		MoveDirection(d.playerActionOnRelease);
	}

}

void PlayerController::SetLook(vec3 targ) {
	
	playerCamera.setLookAt(targ);
	//playerCamera.setCameraTargetVectors(targ);
	
}

void PlayerController::SetFlashlight(int lightID) {
	flashLightID = lightID;
}

void PlayerController::SetCharacterModel(int modelID) {
	playerCharacterModelID = modelID;
}