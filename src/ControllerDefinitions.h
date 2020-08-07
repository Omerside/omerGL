#ifndef CONTROLLERDEFINITIONS_H
#define CONTROLLERDEFINITIONS_H

#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include <iostream>


using namespace std;

//static GLFWwindow* gWindow = NULL;
enum DIRECTION {
	NONE,
	FORWARD,
	BACK,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

//Defines all the actions which can be taken by a player.
enum PlayerActions {
	PLAYER_MOVE_LEFT,
	PLAYER_MOVE_RIGHT,
	PLAYER_MOVE_UP,
	PLAYER_MOVE_DOWN,
	PLAYER_MOVE_FORWARD,
	PLAYER_MOVE_BACK,
	PLAYER_MOVE_STOP,
	PLAYER_NONE
};

enum EntityActions {

};

enum EntityTypes {
	ENTITY_DYNAMIC,
	ENTITY_STATIC
};

enum LightTypes {
	LIGHT_POINT,
	LIGHT_SPOT,
	LIGHT_DIRECTIONAL
};

enum LightingActions {

};

enum SystemActions {
	SYSTEM_CLOSE_WINDOW,
	SYSTEM_POLYGON_MODE,
	SYSTEM_NONE
};

enum CollisionActions {

};

enum ActionType {
	COLLISION,
	LIGHTING,
	ENTITY,
	PLAYER,
	SYSTEM
};


//Used by InputController to relay input->action mapping
struct KeyAction {
	ActionType type;
	DIRECTION playerActionOnPress;
	DIRECTION playerActionOnRelease;
	SystemActions SystemAction;
};

struct EntityProperties {
	EntityTypes type;
	int entityId;
	glm::vec3 position = glm::vec3(0);
	int textureId = -1; // default value for untextured entity
	
};

struct MouseProperties {
	float gYaw = 0.0f;
	float gPitch = 0.0f;
	float gRadius = 0.0f;
	float MOUSE_SENSITIVITY = 0.25F;
};

#endif