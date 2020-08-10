#ifndef LIGHTCONTROLLER_H
#define LIGHTCONTROLLER_H

#include <iostream>
#include <map>
#include <algorithm>
#include "Log.h"
#include "ControllerDefinitions.h"
#include "Light.h"
#include "GLFW/glfw3.h"
#include "glm/ext.hpp"
#include "glm/glm.hpp"

using namespace glm;

/*
Controller for mouse and keyboard input.
Will also contain instructions for what to do when each key is pressed
*/
const int MAX_NUM_LIGHTS = 50;

class LightController
{
private:
	friend class RootController;
	static LightController* getInstance();

	//Declaring as singleton class
	static LightController* inst;
	GLFWwindow* gWindow = NULL;
	void SetGlfWindow(GLFWwindow* gWindowInput) { gWindow = gWindowInput; };
	ShaderProgram* shader;

	//LightController can only be instantiated by RootController.
	LightController();
	~LightController();

	//view matrix and camera position:
	vec3* cameraPos;
	mat4* viewMat;

	//Stacks of light
	vector<PointLight*> pointLights;
	vector<SpotLight*> spotLights;
	vector<DirectionalLight*> directionalLights;


	/* lightIDMap
	Stack of pairs connecting a light ID unique across all 3 light types with an ID
	specific to the stack the light exists on.

	The location of the data on this vector correlates with the unique ID which can be used
	to refer to the specific object.
	*/
	vector<pair<LightTypes, int>> lightIDMap;
	//vector<pair<LightTypes, int>> dynamicLightIDMap;



	//create a light object, returns light ID
	int InsertLight(
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



	//Control light position, direction
	bool SetPosition(int lightID, vec3 pos);
	bool SetDirection(int lightID, vec3 direction);

	//Draw lights currently stored in LightController
	void DrawLights();

	//Delete light object
	void DeleteLight(int id);

	//Change the visibilty layer of a light object
	void UpdateLightLayer(Layer layer, int lightID);

	//Process an action passed by the root controller.
	void ProcessAction(LightAction actionObj);
	void ProcessAction(LightActionDynamic actionObj);

	
public:
	
};

#endif