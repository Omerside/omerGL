#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <iostream>
#include <map>
#include "Log.h"
#include "ControllerDefinitions.h"
#include "Camera.h"
#include "GLFW/glfw3.h"

using namespace std;

/*
Controller for mouse and keyboard input.
Will also contain instructions for what to do when each key is pressed
*/


class PlayerController
{
private:
	friend class RootController;

	//Declaring as singleton class
	static PlayerController* getInstance();
	static PlayerController* inst;
	GLFWwindow* gWindow = NULL;
	PlayerController();

	FirstPersonCamera playerCamera;
	void InitPlayerCamera();

	int flashLightID = -1;
	int playerCharacterModelID = -1;
	
	//Set a flashlight which follows the camera and points at what the player is targeting
	void SetFlashlight(int lightID);
	void SetCharacterModel(int modelID);

public:
	
	MouseProperties CalcNewMouseProperties(MouseProperties in);
	vec3 getPlayerCameraPosition() {return playerCamera.getPosition();};
	mat4 getViewMatrix() { return playerCamera.getViewMatrix(); };
	vec3 getLook() {return playerCamera.getLook(); };
	void SetGlfWindow(GLFWwindow* gWindowInput) { gWindow = gWindowInput; };
	void ExecuteCameraMove(float yaw, float pitch);

	void MoveDirection(DIRECTION d);
	void MoveDirection(KeyAction d, int action);
	void SetLook(vec3 targ);

	
};

#endif