#include "PlayerController.h"


PlayerController* PlayerController::inst = NULL;

PlayerController::PlayerController() {
	//InitPlayerCamera();
	InitOrbitCamera();
}

PlayerController* PlayerController::getInstance() {
	if (inst == NULL) {
		inst = new PlayerController();
	}

	return(inst);
}

void PlayerController::InitPlayerCamera(){
	activeCameraType = FIRST_PERSON;
	//TODO: Make a dynamic method for initializing position.
	if (orbitCamera != nullptr) {
		delete orbitCamera;
	}

	if (playerCamera == nullptr){
		playerCamera = new FirstPersonCamera();
	}

	playerCamera->setMoveSpeed(0.25f);
	cameraUp = orbitCamera->getUp();
	//playerCamera->setCameraPositionVectors(vec3(0.35f, 4.6f, 10.0f));
	SetLook(vec3(0.1));

	LOG(DEBUG) << "PlayerController::InitPlayerCamera - DONE INITIALIZING FirstPersonCamera";
}

void PlayerController::InitOrbitCamera() {
	activeCameraType = ORBIT;
	//TODO: Make a dynamic method for initializing position.
	if (orbitCamera == nullptr) {
		orbitCamera = new OrbitCamera();
	}

	if (playerCamera != nullptr) {
		delete playerCamera;
	}

	orbitCamera->setMoveSpeed(0.25f);
	orbitCamera->setRadius(25.0f);
	SetLook(vec3(0.1));
	orbitCamera->setCameraPositionVectors(vec3(0.35f, 4.6f, 10.0f));
	SetCameraTargetPos(new vec3(0));
	cameraUp = orbitCamera->getUp();
	LOG(DEBUG) << "PlayerController::InitOrbitCamera - DONE INITIALIZING OrbitCamera";

	
}

MouseProperties PlayerController::CalcNewMouseProperties(MouseProperties in) {
	//we will need to calculate the actual initial yaw and pitch depending on our starting look-at coordinates.
	//otherwise the camera will simply snap to place.

	return in;
}

void PlayerController::ExecuteCameraMove(float yaw, float pitch) {
	LOG(DEBUG) << "PlayerController::ExecuteCameraMove - Beginning camera move. Type: " << activeCameraType;
	switch (activeCameraType) {

	case FIRST_PERSON:

		playerCamera->rotateOnCamera(yaw, pitch);
		break;
		
	case ORBIT:
		orbitCamera->rotateOnObject(yaw, pitch, *cameraTargetPos);
		break;

	default:
		LOG(ERR) << "PlayerController::ExecuteCameraMove - Invalid CAMERA type.";
	}
	LOG(DEBUG) << "PlayerController::ExecuteCameraMove - Finished camera move.";
}

void PlayerController::MoveDirection(DIRECTION d) {
	LOG(DEBUG) << "PlayerController::MoveDirection - Beginning move direction. Type: " << d;

	switch (activeCameraType) {

	case FIRST_PERSON:
		playerCamera->move(d);
		break;

	case ORBIT:
		break;

	default:
		LOG(ERR) << "PlayerController::MoveDirection - Invalid CAMERA type.";
	}

	LOG(DEBUG) << "PlayerController::MoveDirection - Done move direction.";
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
	if (isFlashlightOn) {
		actionQueue.push_back(new LightAction(LIGHT_UPDATE_DIR, flashLightID, 0.0f, *lookAt));
	}

	if (action == GLFW_PRESS) {
		switch (d.playerActionOnPress) {

		case PLAYER_SWITCH_FLASHLIGHT:
			LOG(DEBUG) << "Request to switch flashlight has been given.";

			if (SwitchFlashlight()){
				actionQueue.push_back(new LightAction(LIGHT_SET_LAYER_VISIBLE, flashLightID));
				actionQueue.push_back(new LightAction(LIGHT_UPDATE_POS, flashLightID, 0.0f, *cameraPos));
				actionQueue.push_back(new LightAction(LIGHT_UPDATE_DIR, flashLightID, 0.0f, *lookAt));


			}
			else {
				actionQueue.push_back(new LightAction(LIGHT_SET_LAYER_INVISIBLE, flashLightID));
			}

			break;

		case PLAYER_MOVE_LEFT:
		case PLAYER_MOVE_RIGHT:
		case PLAYER_MOVE_UP:
		case PLAYER_MOVE_DOWN:
		case PLAYER_MOVE_FORWARD:
		case PLAYER_MOVE_BACK:
			LOG(DEBUG) << "Updating light pos and dir";
			if (isFlashlightOn) {
				actionQueue.push_back(new LightAction(LIGHT_UPDATE_POS, flashLightID, 0.0f, *cameraPos));
				dynamicLightActionsQueue.push_back(new LightActionDynamic(LIGHT_UPDATE_POS, flashLightID, nullptr, cameraPos));
			}

			break;

		case PLAYER_MOVE_MOUSE:
			break;

		case PLAYER_MOVE_STOP:
			break;

		case PLAYER_NONE:
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
		case PLAYER_MOVE_STOP:
			for (int i = 0; i < dynamicLightActionsQueue.size(); i++) {
				switch (d.playerActionOnPress) {
				case PLAYER_MOVE_LEFT:
				case PLAYER_MOVE_RIGHT:
				case PLAYER_MOVE_UP:
				case PLAYER_MOVE_DOWN:
				case PLAYER_MOVE_FORWARD:
				case PLAYER_MOVE_BACK:
					LOG(DEBUG) << "PlayerController::CalcLightAction - Attempting to remove dynamic action...";
					dynamicLightActionsQueue.erase(dynamicLightActionsQueue.begin()+i);
					LOG(DEBUG) << "PlayerController::CalcLightAction - Action removed.";
				case PLAYER_NONE:
					break;
				default:
					LOG(ERR) << "PlayerController::CalcLightAction - Invalid action type.";
				}
			}
			
		case PLAYER_NONE:
			break;
		default:
			LOG(ERR) << "PlayerController::CalcLightAction - Invalid action type.";
		}
	}
}

void PlayerController::GetLightPersistentAction(vector<LightAction*> &actionQueue) {
	LOG(DEBUG) << "PlayerController::GetLightPersistentAction - Attempting to merge persistent and single-cycle queue values... dynamic queue size: " << dynamicLightActionsQueue.size();

	for (int i = 0; i < dynamicLightActionsQueue.size(); i++) {
		if (dynamicLightActionsQueue[i]->v == nullptr && dynamicLightActionsQueue[i]->f == nullptr) {
			actionQueue.push_back(new LightAction(dynamicLightActionsQueue[i]->action, dynamicLightActionsQueue[i]->id));// , dynamicLightActionsQueue[i]->f, dynamicLightActionsQueue[i]->v));

		} else if (dynamicLightActionsQueue[i]->v == nullptr || dynamicLightActionsQueue[i]->f == nullptr) {
			if (dynamicLightActionsQueue[i]->v == nullptr) {
				actionQueue.push_back(new LightAction(dynamicLightActionsQueue[i]->action, dynamicLightActionsQueue[i]->id, *dynamicLightActionsQueue[i]->f));
			}
			else {
				actionQueue.push_back(new LightAction(dynamicLightActionsQueue[i]->action, dynamicLightActionsQueue[i]->id, 0, *dynamicLightActionsQueue[i]->v));
			}
		}
		else {
			actionQueue.push_back(new LightAction(dynamicLightActionsQueue[i]->action, dynamicLightActionsQueue[i]->id, *dynamicLightActionsQueue[i]->f, *dynamicLightActionsQueue[i]->v));
		}
		
	}
	LOG(DEBUG) << "PlayerController::GetLightPersistentAction - PROCESSED all persistent actions.";
}

void PlayerController::GetEntityPersistentAction(vector<EntityAction*> &actionQueue) {
	LOG(DEBUG) << "PlayerController::GetLightPersistentAction - Attempting to merge persistent and single-cycle queue values... dynamic queue size: " << dynamicLightActionsQueue.size();

	for (int i = 0; i < dynamicEntityActionsQueue.size(); i++) {
		if (dynamicEntityActionsQueue[i]->v == nullptr && dynamicEntityActionsQueue[i]->f == nullptr) {
			actionQueue.push_back(new EntityAction(dynamicEntityActionsQueue[i]->action, dynamicEntityActionsQueue[i]->id));

		}
		else if (dynamicEntityActionsQueue[i]->v == nullptr || dynamicEntityActionsQueue[i]->f == nullptr) {
			if (dynamicEntityActionsQueue[i]->v == nullptr) {
				actionQueue.push_back(new EntityAction(dynamicEntityActionsQueue[i]->action, dynamicEntityActionsQueue[i]->id, *dynamicEntityActionsQueue[i]->f));
			}
			else {
				actionQueue.push_back(new EntityAction(dynamicEntityActionsQueue[i]->action, dynamicEntityActionsQueue[i]->id, 0, *dynamicEntityActionsQueue[i]->v));
			}
		}
		else {
			actionQueue.push_back(new EntityAction(dynamicEntityActionsQueue[i]->action, dynamicEntityActionsQueue[i]->id, *dynamicEntityActionsQueue[i]->f, *dynamicEntityActionsQueue[i]->v));
		}

	}
	LOG(DEBUG) << "PlayerController::GetLightPersistentAction - PROCESSED all persistent actions.";
}

void PlayerController::CalcEntityAction(KeyAction d, int action, vector<EntityAction*> &actionQueue) {
	vec3 direction = (*cameraTargetPos - *cameraPos); 
	vec3 cameraRight;

	if (playerCharacterModelID != -1) {
		direction = normalize(vec3(direction.x, 0, direction.z));
		actionQueue.push_back(new EntityAction(ENTITY_UPDATE_DIR, playerCharacterModelID, 0.0f, direction));
		LOG(DEBUG) << "PlayerController::CalcEntityAction - Setting direction to " << direction;
	}

	if (action == GLFW_PRESS) {
		
		switch (d.playerActionOnPress) {

		
		case PLAYER_MOVE_RIGHT:
			cameraRight = normalize(cross(cameraUp, direction))*moveSpeed;
			actionQueue.push_back(new EntityAction(ENTITY_UPDATE_POS, playerCharacterModelID, 0.0f, vec3(-cameraRight.x, 0, -cameraRight.z)));
			dynamicEntityActionsQueue.push_back(new EntityActionDynamic(ENTITY_UPDATE_POS, playerCharacterModelID, nullptr, new vec3(-cameraRight.x, 0, -cameraRight.z)));
			break;

		case PLAYER_MOVE_LEFT:
			cameraRight = normalize(cross(cameraUp, direction))*moveSpeed;
			actionQueue.push_back(new EntityAction(ENTITY_UPDATE_POS, playerCharacterModelID, 0.0f, vec3(cameraRight.x, 0, cameraRight.z)));
			dynamicEntityActionsQueue.push_back(new EntityActionDynamic(ENTITY_UPDATE_POS, playerCharacterModelID, nullptr, new vec3(cameraRight.x, 0, cameraRight.z)));
			break;

		case PLAYER_MOVE_UP:
			actionQueue.push_back(new EntityAction(ENTITY_UPDATE_POS, playerCharacterModelID, 0.0f, cameraUp * moveSpeed));
			dynamicEntityActionsQueue.push_back(new EntityActionDynamic(ENTITY_UPDATE_POS, playerCharacterModelID, nullptr, &cameraUp));
			break;

		case PLAYER_MOVE_DOWN:
			actionQueue.push_back(new EntityAction(ENTITY_UPDATE_POS, playerCharacterModelID, 0.0f, -cameraUp * moveSpeed));
			dynamicEntityActionsQueue.push_back(new EntityActionDynamic(ENTITY_UPDATE_POS, playerCharacterModelID, nullptr, &(-cameraUp * moveSpeed)));
			break;

		case PLAYER_MOVE_FORWARD:
			direction = normalize(vec3(direction.x, 0, direction.z))*moveSpeed;
			actionQueue.push_back(new EntityAction(ENTITY_UPDATE_POS, playerCharacterModelID, 0.0f, vec3(direction.x, 0, direction.z)));
			dynamicEntityActionsQueue.push_back(new EntityActionDynamic(ENTITY_UPDATE_POS, playerCharacterModelID, nullptr, new vec3(direction.x, 0, direction.z)));
			break;

		case PLAYER_MOVE_BACK:
			direction = normalize(vec3(direction.x, 0, direction.z))*moveSpeed;
			actionQueue.push_back(new EntityAction(ENTITY_UPDATE_POS, playerCharacterModelID, 0.0f, vec3(-direction.x, 0, -direction.z)));
			dynamicEntityActionsQueue.push_back(new EntityActionDynamic(ENTITY_UPDATE_POS, playerCharacterModelID, nullptr, new vec3(-direction.x, 0, -direction.z)));
			break;

		case PLAYER_MOVE_MOUSE:
			break;

		case PLAYER_MOVE_STOP:
			break;

		case PLAYER_NONE:
			break;

		default:
			LOG(ERR) << "PlayerController::CalcLightAction - Invalid action type.";
		}

	}
	else if (action == GLFW_RELEASE) {
		switch (d.playerActionOnRelease) {

		case PLAYER_MOVE_STOP:
			for (int i = 0; i < dynamicEntityActionsQueue.size(); i++) {
				switch (d.playerActionOnPress) {
				case PLAYER_MOVE_LEFT:
				case PLAYER_MOVE_RIGHT:
				case PLAYER_MOVE_UP:
				case PLAYER_MOVE_DOWN:
				case PLAYER_MOVE_FORWARD:
				case PLAYER_MOVE_BACK:
					LOG(DEBUG) << "PlayerController::CalcLightAction - Attempting to remove dynamic action...";
					dynamicEntityActionsQueue.erase(dynamicEntityActionsQueue.begin() + i);
					LOG(DEBUG) << "PlayerController::CalcLightAction - Action removed.";
				case PLAYER_NONE:
					break;
				default:
					LOG(ERR) << "PlayerController::CalcLightAction - Invalid action type.";
				}
			}

		case PLAYER_NONE:
			break;
		default:
			LOG(ERR) << "PlayerController::CalcLightAction - Invalid action type.";
		}
	}
}
void PlayerController::SetLook(vec3 targ) {
	
	switch (activeCameraType) {

	case FIRST_PERSON:
		playerCamera->setLookAt(targ);
		break;

	case ORBIT:
		orbitCamera->setLookAt(targ);
		break;

	default:
		LOG(ERR) << "PlayerController::SetLook - Invalid CAMERA type.";
	}
}

void PlayerController::SetCameraTargetPos(vec3* targetPos) {
	cameraTargetPos = targetPos;
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

vec3 PlayerController::getPlayerCameraPosition() {
	switch (activeCameraType) {

	case FIRST_PERSON:
		return playerCamera->getPosition();
		break;

	case ORBIT:
		return orbitCamera->getPosition();
		break;

	default:
		LOG(ERR) << "PlayerController::getPlayerCameraPosition - Invalid CAMERA type.";
	}

	return vec3(0);
}

mat4 PlayerController::getViewMatrix() {
	switch (activeCameraType) {

	case FIRST_PERSON:
		return playerCamera->getViewMatrix();
		break;

	case ORBIT:
		return orbitCamera->getViewMatrix();
		break;

	default:
		LOG(ERR) << "PlayerController::getViewMatrix - Invalid CAMERA type.";
	}

	return mat4(0);
}

vec3 PlayerController::getLook() {
	switch (activeCameraType) {

	case FIRST_PERSON:
		return playerCamera->getLook();
		break;

	case ORBIT:
		LOG(DEBUG) << "PlayerController::getLook - returning " << orbitCamera->getLook();
		return orbitCamera->getLook();
		break;

	default:
		LOG(ERR) << "PlayerController::getLook - Invalid CAMERA type.";
	}

	return vec3(0);
}