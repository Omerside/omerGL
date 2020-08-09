#include "InputController.h"

//singleton definition
InputController* InputController::inst = NULL;

//cursor properties
static MouseProperties mouseProperties;
static vec2 lastMousePos;


KeyAction CreateKeyAction(ActionType t, DIRECTION pdPress, DIRECTION pdRelease, PlayerActions paPress, PlayerActions paRelease, SystemActions sa);


void InputController::OnMouseMove(double posX, double posY) {

	// update angles based on left mouse button input 
	if (glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_LEFT) == 1) {

		//1 PIXEL = .25 OF AN ANGLE DEGREE ROTATION
		mouseProperties.gYaw += ((float)posX - lastMousePos.x) * mouseProperties.MOUSE_SENSITIVITY;
		mouseProperties.gPitch -= ((float)posY - lastMousePos.y) *  mouseProperties.MOUSE_SENSITIVITY;
	}

	// update angles based on left mouse button input 
	//Only applicable to retating camera
	if (glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_RIGHT) == 1) {

		//1 PIXEL = .25 OF AN ANGLE DEGREE ROTATION
		float dx = 0.01f * ((float)posX - lastMousePos.x);
		float dy = 0.01f * ((float)posY - lastMousePos.y);
		mouseProperties.gRadius += dx - dy;
	}

	lastMousePos.x = (float)posX;
	lastMousePos.y = (float)posY;

}


KeyAction CreateKeyAction(ActionType t, DIRECTION pdPress, DIRECTION pdRelease, PlayerActions paPress, PlayerActions paRelease, SystemActions sa) {
	KeyAction ka;
	ka.type = t;
	ka.playerDirectionOnPress = pdPress;
	ka.playerDirectionOnRelease = pdRelease;
	ka.playerActionOnPress = paPress;
	ka.playerActionOnRelease = paRelease;
	ka.SystemAction = sa;
	return ka;
}

InputController::InputController() { 
	InitKeymapping();

	//assign mouse properties
	mouseProperties.MOUSE_SENSITIVITY = 0.25F;
	mouseProperties.gYaw = 0.0f;
	mouseProperties.gPitch = 0.0f;
	mouseProperties.gRadius = 0.0f;
	lastMousePos = vec2(0.0);

};

InputController* InputController::getInstance() {
	if (inst == NULL) {
		inst = new InputController();
	}

	return(inst);
};

void InputController::InitKeymapping() {

	//Initialize all keys to performing no action at all.
	for (int i = 0; i < GLFW_KEY_LAST + 1; i++) {
		keyMapping[i] = CreateKeyAction(SYSTEM, NONE, NONE, PLAYER_NONE, PLAYER_NONE, SYSTEM_NONE);
	}

	//define some individual keys.
	keyMapping[GLFW_KEY_ESCAPE] = CreateKeyAction(SYSTEM, NONE, NONE, PLAYER_NONE, PLAYER_NONE, SYSTEM_CLOSE_WINDOW);
	keyMapping[GLFW_KEY_W] = CreateKeyAction(PLAYER, FORWARD, NONE, PLAYER_MOVE_FORWARD, PLAYER_NONE, SYSTEM_NONE);
	keyMapping[GLFW_KEY_S] = CreateKeyAction(PLAYER, BACK, NONE, PLAYER_MOVE_BACK, PLAYER_NONE, SYSTEM_NONE);
	keyMapping[GLFW_KEY_D] = CreateKeyAction(PLAYER, RIGHT, NONE, PLAYER_MOVE_RIGHT, PLAYER_NONE, SYSTEM_NONE);
	keyMapping[GLFW_KEY_A] = CreateKeyAction(PLAYER, LEFT, NONE, PLAYER_MOVE_LEFT, PLAYER_NONE, SYSTEM_NONE);
	keyMapping[GLFW_KEY_SPACE] = CreateKeyAction(PLAYER, UP, NONE, PLAYER_MOVE_UP, PLAYER_NONE, SYSTEM_NONE);
	keyMapping[GLFW_KEY_C] = CreateKeyAction(PLAYER, DOWN, NONE, PLAYER_MOVE_DOWN, PLAYER_NONE, SYSTEM_NONE);
	keyMapping[GLFW_KEY_P] = CreateKeyAction(SYSTEM, NONE, NONE, PLAYER_NONE, PLAYER_NONE, SYSTEM_POLYGON_MODE);
	keyMapping[GLFW_KEY_F] = CreateKeyAction(PLAYER, NONE, NONE, PLAYER_SWITCH_FLASHLIGHT, PLAYER_NONE, SYSTEM_NONE);
}


KeyAction InputController::OnKey(int key, int scancode, int action, int mode) {
	lastAction = action;
	if (keyMapping.find(key) == keyMapping.end()) {
		return keyMapping[0];
	} 
	return keyMapping[key];

}


MouseProperties* InputController::GetMouseProperties() {
	return &mouseProperties;
}