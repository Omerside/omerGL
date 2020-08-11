#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
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

	CameraTypes activeCameraType;
	FirstPersonCamera* playerCamera;
	OrbitCamera* orbitCamera;

	void InitPlayerCamera();
	void InitOrbitCamera();

	bool SwitchFlashlight();
	bool isFlashlightOn = false;
	int flashLightID = -1;
	int playerCharacterModelID = -1;

	vec3* cameraPos;
	vec3* lookAt;
	mat4* viewMat;
	
	//Set a flashlight which follows the camera and points at what the player is targeting
	void SetFlashlight(int lightID);
	void SetCharacterModel(int modelID);

	//Check for actions taken by player from outside factors
	void CalcLightAction(KeyAction d, int action, vector<LightAction*> &actionQueue);

	//Append the list of persistent actions to the main action queue for the light controller
	void GetLightPersistentAction(vector<LightAction*> &actionQueue);
	vector<LightActionDynamic*> dynamicLightActionsQueue;

	//EntityAction** CalcEntityAction(KeyAction d, int action);

	void MoveDirection(DIRECTION d);
	

public:

	MouseProperties CalcNewMouseProperties(MouseProperties in);
	vec3 getPlayerCameraPosition();
	mat4 getViewMatrix();
	vec3 getLook();
	void SetGlfWindow(GLFWwindow* gWindowInput) { gWindow = gWindowInput; };
	void ExecuteCameraMove(float yaw, float pitch);


	void MoveDirection(KeyAction d, int action);
	void SetLook(vec3 targ);

	
};

#endif