#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <iostream>
#include <map>
#include "Log.h"
#include "ControllerDefinitions.h"
#include "GLFW/glfw3.h"
#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include <unordered_map>

using namespace glm;

/*
Controller for mouse and keyboard input.
Will also contain instructions for what to do when each key is pressed
*/


class InputController
{
private:
	friend class RootController;

	//Declaring as singleton class
	static InputController* getInstance();
	static InputController* inst;
	GLFWwindow* gWindow = NULL;
	InputController();
	void InitKeymapping();

	//Record last action (press|release)
	int lastAction;


	

public:
	

	KeyAction OnKey(int key, int scancode, int action, int mode);

	//returns yaw and pitch of mouse based on new position
	
	static MouseProperties* GetMouseProperties();
	void OnMouseMove(double posX, double posY);
	void SetGlfWindow(GLFWwindow* gWindowInput) { gWindow = gWindowInput; };
	

	unordered_map<int, KeyAction> keyMapping;
	
};

#endif