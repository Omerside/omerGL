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
		MoveDirection(d.playerDirectionOnPress);
	}
	else if (action == GLFW_RELEASE) {
		MoveDirection(d.playerDirectionOnRelease);
	}
}

void PlayerController::CalcLightAction(KeyAction d, int action, vector<LightAction*> &actionQueue) {
	
	if (action == GLFW_PRESS) {
		switch (d.playerActionOnPress) {

		case PLAYER_SWITCH_FLASHLIGHT:
			LOG(INFO) << "Request to switch flashlight has been given.";

			if (SwitchFlashlight()){
				actionQueue.push_back(new LightAction(LIGHT_SET_LAYER_VISIBLE, flashLightID));

			}
			else {
				actionQueue.push_back(new LightAction(LIGHT_SET_LAYER_HIDDEN, flashLightID));
			}

			break;

		case PLAYER_MOVE_LEFT:
		case PLAYER_MOVE_RIGHT:
		case PLAYER_MOVE_UP:
		case PLAYER_MOVE_DOWN:
		case PLAYER_MOVE_FORWARD:
		case PLAYER_MOVE_BACK:
			LOG(INFO) << "Updating light pos and dir";
			if (isFlashlightOn) {
	

				actionQueue.push_back(new LightAction(LIGHT_UPDATE_POS, flashLightID, 0.0f, getPlayerCameraPosition()));
				actionQueue.push_back(new LightAction(LIGHT_UPDATE_DIR, flashLightID, 0.0f, getLook()));
			}

			break;

		default:
			LOG(ERR) << "PlayerController::CalcLightAction - Invalid action type.";
		}

	}
	else if (action == GLFW_RELEASE) {
		switch (d.playerActionOnRelease) {

		case PLAYER_SWITCH_FLASHLIGHT:

			if (SwitchFlashlight()) {
				actionQueue.push_back(new LightAction(LIGHT_SET_LAYER_VISIBLE, flashLightID));
			}
			else {
				actionQueue.push_back(new LightAction(LIGHT_SET_LAYER_HIDDEN, flashLightID));
			}

			break;

		default:
			LOG(ERR) << "PlayerController::CalcLightAction - Invalid action type.";
		}
	}
}

/*
EntityAction** PlayerController::CalcEntityAction(KeyAction d, int action) {

	if (action == GLFW_PRESS) {
		switch (d.playerDirectionOnPress) {

		case PLAYER_MOVE_LEFT:
		case PLAYER_MOVE_RIGHT:
		case PLAYER_MOVE_UP:
		case PLAYER_MOVE_DOWN:
		case PLAYER_MOVE_FORWARD:
		case PLAYER_MOVE_BACK:
			LOG(INFO) << "Move entity requested.";
			actions[0] = new EntityAction(ENTITY_UPDATE_POS, playerCharacterModelID);
			actions[0] = new EntityAction(ENTITY_UPDATE_DIR, playerCharacterModelID);

			break;

		default:
			LOG(ERR) << "PlayerController::CalcEntityAction - Invalid action type.";
		}

	}
	else if (action == GLFW_RELEASE) {
		switch (d.playerDirectionOnRelease) {

		case PLAYER_MOVE_LEFT:
		case PLAYER_MOVE_RIGHT:
		case PLAYER_MOVE_UP:
		case PLAYER_MOVE_DOWN:
		case PLAYER_MOVE_FORWARD:
		case PLAYER_MOVE_BACK:
			EntityAction* actions[1];
			actions[1] = new EntityAction(ENTITY_UPDATE_POS, playerCharacterModelID);

			break;

		default:
			LOG(ERR) << "PlayerController::CalcEntityAction - Invalid action type.";
		}
	}
}*/

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

bool PlayerController::SwitchFlashlight() {
	if (flashLightID != -1) {
		isFlashlightOn = !isFlashlightOn;
		return isFlashlightOn;
	}
	else {
		LOG(ERR) << "PlayerController::SwitchFlashlight - Cannot enable flashlight; flashlight undefined.";
		return isFlashlightOn;
	}
}