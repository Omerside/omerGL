#ifndef ROOTCONTROLLER_H
#define ROOTCONTROLLER_H

#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include "Log.h"
#include "InputController.h"
#include "PlayerController.h"
#include "EntityController.h"
#include "LightController.h"
#include "ControllerDefinitions.h"


using namespace std;

/*

This is the master controller that will eventually manage every object within the game engine.
It will do this by managing interactions between controllers.

For example, if the InputController detects the 'A' key being pressed, it will report it to RootController.
The RootController will take the input and the accompanied instruction.
If the instruction is to move the player, RootController will send a request to PlayerController to movethe character object.

*/


class RootController
{
private:

	//Declaring as singleton class
	static RootController* inst;
	RootController();
	~RootController();

	//Subcontrollers
	InputController* inputCtrl;
	PlayerController* playerCtrl;
	EntityController* entityCtrl;
	LightController* lightCtrl;
	GLFWwindow* gWindow;

	//misc properties
	double lastTime;
	MouseProperties* mp;
	bool gWireframe = false;
	ShaderProgram* shader;
	

public:

	void SetShader(ShaderProgram* shaderInput);
	
	static RootController* getInstance();
	void CheckInputController(int key, int scancode, int action, int mode);
	void CheckMouseInput(double posX, double posY);

	void SetCameraLookAt(vec3 targ);
	vec3 TempGetCameraPosition() { return playerCtrl->getPlayerCameraPosition(); };
	mat4 TempGetViewMatrix() { return playerCtrl->getViewMatrix(); };
	vec3 TempGetLook() { return playerCtrl->getLook(); };
	void TempMoveCamera(DIRECTION d) {
		playerCtrl->MoveDirection(d);
	};

	void SystemDirection(KeyAction ka, int action);
	void SetGlfWindow(GLFWwindow* gWindowInput);

	//Load an entity, return entity ID.
	int LoadEntity(
		const char* daeFile,
		EntityTypes type,
		const char* texFile,
		vec3 scale,
		vec3 pos = vec3(0),
		bool activeAnimLooping = false,
		const char* activeAnim = NULL,
		bool isOutlined = false,
		vec3 outlineColor = vec3(0),
		vec3 outlineColorHidden = vec3(0)
	);

	//Load a light, return light ID.
	int LoadLight(
		LightTypes type,
		vec3 ambient,
		vec3 diffuse,
		vec3 specular,
		vec3 pos = vec3(0, 3, 0),
		vec3 direction = vec3(.5, -1, .5),
		float cosInnerConeIn = cos(radians(5.0f)),
		float cosOuterConeIn = cos(radians(10.0f)),
		float constantIn = 1.0f,
		float linearIn = 0.07f,
		float exponentIn = 0.017f
	);

	void Update();
};

#endif