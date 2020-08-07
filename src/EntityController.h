#ifndef ENTITYCONTROLLER_H
#define ENTITYCONTROLLER_H

#include <iostream>
#include <map>
#include "Log.h"
#include "ControllerDefinitions.h"
#include "AnimatedModel.h"
#include "Texture2D.h"
#include "GLFW/glfw3.h"
#include "glm/ext.hpp"
#include "glm/glm.hpp"

using namespace glm;

/*
Controller for mouse and keyboard input.
Will also contain instructions for what to do when each key is pressed
*/
const int MAX_NUM_ENTITIES = 300;

class EntityController
{
private:
	friend class RootController;
	static EntityController* getInstance();

	//Declaring as singleton class
	static EntityController* inst;
	GLFWwindow* gWindow = NULL;
	void SetGlfWindow(GLFWwindow* gWindowInput) { gWindow = gWindowInput; };

	//EntityController can only be instantiated by RootController.
	EntityController();
	~EntityController();

	ShaderProgram* shader;
	vector<AnimatedModel*> entities; // Location in vector = entity ID
	//AnimatedModel* entitiesOrdered[MAX_NUM_ENTITIES];
	vector<EntityProperties> entityProperties; // Location in vector = entity ID
	vector<Texture2D> textures; //Location in vector = texture ID
	
	double deltaTime;
	void DrawOutlinedEntity(int* entityID);
	void DrawEntity(int* entityID);

	
public:
	
	//creates a dynamic entity and returns the entity ID
	int LoadEntity(const char* daeFile, const char* texFile, EntityTypes type, vec3 pos);
	void DrawEntities(double* deltaTime);

	//modify entities -- true = success (where applicable)
	bool SetActiveAnimation(int entityID, const char* animName, bool* isLooping);
	void SetEntityScale(int entityID, vec3* scale);
	void EnableEntityOutline(int entityID, vec3* colorInput, vec3* scaleInput, vec3* outlineColorHiddenInput);
};

#endif