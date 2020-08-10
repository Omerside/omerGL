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
	PLAYER_MOVE_MOUSE,
	PLAYER_SWITCH_FLASHLIGHT,
	PLAYER_NONE
};

//Defines types of entities.
enum EntityTypes {
	ENTITY_DYNAMIC, //Entity capable of world movement and animation
	ENTITY_STATIC //Static entity incapable of movement or animation
};

//Define actions which can be performed by an entity
enum EntityActions {
	ENTITY_SET_LAYER,
	ENTITY_UPDATE_POS,
	ENTITY_UPDATE_DIR
};


//Defines types of light objects
enum LightTypes {
	LIGHT_POINT,
	LIGHT_SPOT,
	LIGHT_DIRECTIONAL,
	LIGHT_PLAYER_FLASHLIGHT
};

//Defines list of actions which can be performed by a light type
enum LightActions {
	LIGHT_SET_LAYER_INVISIBLE,
	LIGHT_SET_LAYER_HIDDEN,
	LIGHT_SET_LAYER_VISIBLE,
	LIGHT_INSERT_DIRECTIONAL,
	LIGHT_INSERT_SPOT,
	LIGHT_INSERT_POINT,
	LIGHT_DELETE,
	LIGHT_UPDATE_POS,
	LIGHT_UPDATE_DIR,
	LIGHT_UPDATE_AMBIENT,
	LIGHT_UPDATE_DIFFUSE,
	LIGHT_UPDATE_SPECULAR,
	LIGHT_UPDATE_INNER_CONE,
	LIGHT_UPDATE_OUTER_CONE,
	LIGHT_UPDATE_CONSTANT,
	LIGHT_UPDATE_LINEAR,
	LIGHT_UPDATE_EXPONENT
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

enum Layer {
	INVISIBLE, // Object which has no observable properties
	HIDDEN, // Object which should not be seen by a player but can be seen by designer,
	VISIBLE // Object visible to all.
};


//Used by InputController to relay input->action mapping
struct KeyAction {
	ActionType type;
	DIRECTION playerDirectionOnPress;
	DIRECTION playerDirectionOnRelease;
	PlayerActions playerActionOnPress;
	PlayerActions playerActionOnRelease;
	SystemActions SystemAction;
	KeyAction() {};
	KeyAction(ActionType t, PlayerActions paop, PlayerActions paor, SystemActions sa) :
		type(t),
		playerActionOnPress(paop),
		playerActionOnRelease(paor),
		SystemAction(sa){}
};

//LightAction is passed to the LightController. The type of action dictates which
//of the variables will be necessary. This action will happen once and then self-delete.
struct LightAction {
	LightActions action;
	const int id;
	float f;
	glm::vec3 v;
	LightAction(LightActions actionIn, int idIn, float fIn = 0.0f, glm::vec3 vIn = glm::vec3(0)) : id(idIn) {
		action = actionIn;
		f = fIn;
		v = vIn;
	}
};

//LightAction is passed to the LightController. The type of action dictates which
//of the variables will be necessary. This action will happen until explicitly deleted.
struct LightActionDynamic {
	const LightActions action;
	const int id;
	const float* f;
	const glm::vec3* v;
	LightActionDynamic(LightActions actionIn, int idIn, float* fIn = nullptr, glm::vec3* vIn = nullptr) :
		id(idIn),
		action(actionIn),
		f(fIn),
		v(vIn) {};
};


struct EntityAction {
	EntityActions action;
	int id;
	float f;
	glm::vec3 v;
	EntityAction(EntityActions actionIn, int idIn, float fIn = 0.0f, glm::vec3 vIn = glm::vec3(0)) {
		action = actionIn;
		id = idIn;
		f = fIn;
		v = vIn;
	}
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