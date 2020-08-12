#include "EntityController.h"


EntityController* EntityController::inst = NULL;

EntityController::EntityController() {

	//To avoid memory reallocations that will cause pointers to splat dead, we are reserving the necessary space in advance.
	entities.reserve(MAX_NUM_ENTITIES);
	entityProperties.reserve(MAX_NUM_ENTITIES);
	textures.reserve(MAX_NUM_ENTITIES);

}

EntityController::~EntityController() {
	for_each(entities.begin(), entities.end(), [](AnimatedModel* &n) { delete n; });
}

EntityController* EntityController::getInstance() {
	if (inst == NULL) {
		inst = new EntityController();
	}

	return(inst);
}


int EntityController::LoadEntity(const char* daeFile, const char* texFile, EntityTypes type, vec3 pos) {
	EntityProperties properties;
	properties.type = type;
	properties.position = pos;


	entities.push_back(new AnimatedModel(shader, daeFile));
	properties.entityId = entities.size() - 1;

	if (texFile) {

		Texture2D texture;
		texture.loadTexture(texFile, true);
		textures.push_back(texture);
		properties.textureId = textures.size() - 1;
	}

	entityProperties.push_back(properties);
	return properties.entityId;
}

void EntityController::DrawEntities(double* deltaTimeInput) {
	LOG(DEBUG) << "Setting deltatime...";
	this->deltaTime = *deltaTimeInput;
	LOG(DEBUG) << "DrawEntities : Attempting to draw entities...";

	for (int i = 0; i < entities.size(); i++){

		if (entities[i]->getOutline()) {
			LOG(DEBUG) << "ID: " << i << " Drawing entity with back";
			DrawOutlinedEntity(&i);
		}
		else {
			LOG(DEBUG) << "ID: " << i << " Drawing entity with no back";
			DrawEntity(&i);
		}
	}
}

void EntityController::DrawOutlinedEntity(int* entityID) {

	
	int textureID = entityProperties[*entityID].textureId;
	if (textureID == -1) {
		LOG() << "no texture found for entity ID " << &entityID;
	}

	//Enable stencil testing.
	glEnable(GL_STENCIL_TEST);

	//Whatever is drawn next will (GL_)ALWAYS have a stencil value of 1 - meaning it will be drawn.
	glStencilFunc(GL_ALWAYS, 1, 0xFF);

	LOG(DEBUG) << "Drawing entity with back - ID is " << *entityID;
	LOG(DEBUG) << "DeltaTime: " << deltaTime;
	LOG(DEBUG) << "position: " << entityProperties[*entityID].position;
	textures[textureID].bind(0);
	entities[*entityID]->DrawModel(entityProperties[*entityID].position, -1, deltaTime);
	textures[textureID].unbind(0);
	LOG(DEBUG) << "Drawing entity with back DONE" << *entityID;
	//Whatever is drawn next will only only be drawn if it has a value of 0 in the stencil buffer 
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	entities[*entityID]->DrawOutline(entityProperties[*entityID].position);

	//We disable depth testing too, as otherwise the outline will fail depth test and not appear in front of other objects.
	glDisable(GL_DEPTH_TEST);

	entities[*entityID]->DrawOutlineHidden(entityProperties[*entityID].position);

	//Disable stencil testing and enable depth testing to resume normal drawing.
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
}

void EntityController::DrawEntity(int* entityID) {
	

	int textureID = entityProperties[*entityID].textureId;
	if (textureID == -1) {
		LOG(DEBUG) << "no texture found for entity ID " << &entityID;
	}


	textures[textureID].bind(0);
	entities[*entityID]->DrawModel(entityProperties[*entityID].position, -1, deltaTime);
	textures[textureID].unbind(0);
}

bool EntityController::SetActiveAnimation(int entityID, const char* animName, bool* isLooping) {
	LOG(DEBUG) << "Setting active animation for entity ID " << entityID << " animation name: " << animName;
	LOG(DEBUG) << "Directory: " << entities[entityID]->GetDirectory();
	return (entities[entityID]->SetActiveAnimation(animName, *isLooping));
}

void EntityController::SetEntityScale(int entityID, vec3* scale){
	LOG() << "Setting entity ID " << entityID << " scale to " << *scale;
	entities[entityID]->SetScale(*scale);
}

void EntityController::EnableEntityOutline(int entityID, vec3* colorInput, vec3* scaleInput, vec3* outlineColorHiddenInput) {
	LOG() << "enabling outline of entity ID " << entityID;
	entities[entityID]->EnableOutline(*colorInput, *scaleInput, *outlineColorHiddenInput);
}


void EntityController::ProcessAction(EntityAction actionObj) {
	if (actionObj.id == -1) {
		LOG(ERR) << "EntityController::ProcessAction - unassigned entity ID.";
		return;
	}
	switch (actionObj.action) {

	case LIGHT_SET_LAYER_HIDDEN:
		//UpdateLightLayer(HIDDEN, actionObj.id);
		break;

	case LIGHT_SET_LAYER_VISIBLE:
		//UpdateLightLayer(VISIBLE, actionObj.id);
		break;

	case LIGHT_SET_LAYER_INVISIBLE:
		//UpdateLightLayer(INVISIBLE, actionObj.id);
		break;

	case ENTITY_UPDATE_POS:
		SetEntityPosition(actionObj.id, actionObj.v);
		break;

	case ENTITY_UPDATE_DIR:
		SetEntityDirection(actionObj.id, actionObj.v);
		break;

	case ENTITY_DELETE:
		//DeleteLight(actionObj.id);
		break;

	default:
		LOG(ERR) << "LightController::ProcessAction - Unsupported command.";
	}

}


void EntityController::SetEntityPosition(int id, vec3 pos) {
	entityProperties[id].position += pos;
}

void EntityController::SetEntityDirection(int id, vec3 dir) {

}

vec3 EntityController::GetEntityPosition(int id) {
	return entityProperties[id].position;
}