#ifndef ROOTCONTROLLER_H
#define ROOTCONTROLLER_H

#include <iostream>
#include <algorithm>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"
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
const int MAX_ACTIONS_PER_CYCLE = 20;

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
	
	KeyAction mouseMoved;

	
protected:

	//shader program
	ShaderProgram* shader;

	//The model, view, and project which will be sent to the shader for drawing.
	mat4 model, view, projection;


	//Width and height of the GL window we are opening.
	int gWindowWidth, gWindowHeight;


	//Action queues for lights
	vector<LightAction*> lightActionsQueue;
	vector<LightActionDynamic*> dynamicLightActionsQueue;

	//Action queues for entities
	vector<EntityAction*> entityActionsQueue;
	vector<EntityActionDynamic*> dynamicEntityActionsQueue;

	//Position of camera in world
	vec3 cameraPos;
	
	//Camera view direction
	vec3 lookAt;

	//Camera view matrix
	mat4 viewMat;

	vec3 playerAvatarPos;

public:

	//Set shader
	void SetShader(ShaderProgram* shaderInput);

	//Load shader files to be used
	void LoadShaders(const char* vertShader = "basic.vert", const char* fragShader = "lighting_multifunc.frag");
	
	//Define singleton
	static RootController* getInstance();

	//Get keyboard/mouse inputs
	void CheckInputController(int key, int scancode, int action, int mode);
	void CheckMouseInput(double posX, double posY);

	//Process light and entity actions by iterating over their respective queue vectors and
	//passing the desired actions to their controllers. Queues are cleared once done.
	void processLightActions();
	void processEntityActions();

	//Get camera properties to be passed down.
	void SetCameraLookAt(vec3 targ);
	vec3 GetCameraPosition() { return playerCtrl->getPlayerCameraPosition(); };
	vec3 GetLook() { return playerCtrl->getLook(); };


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