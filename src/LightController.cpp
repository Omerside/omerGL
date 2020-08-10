#include "LightController.h"


LightController* LightController::inst = NULL;

LightController::LightController() {

	//To avoid memory reallocations that will cause pointers to splat dead, we are reserving the necessary space in advance.
	pointLights.reserve(MAX_NUM_LIGHTS);
	spotLights.reserve(MAX_NUM_LIGHTS);
	directionalLights.reserve(MAX_NUM_LIGHTS);
	lightIDMap.reserve(MAX_NUM_LIGHTS*3); // Magic number "3" because I never plan on having more than 3 light types... sue me

}

LightController::~LightController() {

	for_each(pointLights.begin(), pointLights.end(), [](PointLight* &n) { delete n; });
	for_each(spotLights.begin(), spotLights.end(), [](SpotLight* &n) { delete n; });
	for_each(directionalLights.begin(), directionalLights.end(), [](DirectionalLight* &n) { delete n; });

}

LightController* LightController::getInstance() {
	if (inst == NULL) {
		inst = new LightController();
	}

	return(inst);
}

void LightController::DrawLights() {
	for_each(pointLights.begin(), pointLights.end(), [](PointLight* &n) { n->draw(); LOG(DEBUG) << "Point light drawn."; });
	for_each(spotLights.begin(), spotLights.end(), [](SpotLight* &n) { n->draw(); LOG(DEBUG) << "Spot light drawn."; });
	for_each(directionalLights.begin(), directionalLights.end(), [](DirectionalLight* &n) { n->draw(); LOG(DEBUG) << "Directional light drawn."; });
}

void LightController::ProcessAction(LightAction actionObj) {
	if (actionObj.id == -1) {
		LOG(ERR) << "LightController::ProcessAction - unassigned light ID.";
		return;
	}
	switch (actionObj.action) {

	case LIGHT_SET_LAYER_HIDDEN:
		UpdateLightLayer(HIDDEN, actionObj.id);
		break;

	case LIGHT_SET_LAYER_VISIBLE:
		UpdateLightLayer(VISIBLE, actionObj.id);
		break;

	case LIGHT_SET_LAYER_INVISIBLE:
		UpdateLightLayer(INVISIBLE, actionObj.id);
		break;

	case LIGHT_UPDATE_POS:
		SetPosition(actionObj.id, actionObj.v);
		break;

	case LIGHT_UPDATE_DIR:
		SetDirection(actionObj.id, actionObj.v);
		break;

	case LIGHT_DELETE:
		DeleteLight(actionObj.id);
		break;

	default:
		LOG(ERR) << "LightController::ProcessAction - Unsupported command.";
	}

}

void LightController::ProcessAction(LightActionDynamic actionObj) {
	if (actionObj.id == -1) {
		LOG(ERR) << "LightController::ProcessAction - unassigned light ID.";
		return;
	}
	switch (actionObj.action) {
	case LIGHT_UPDATE_POS:
		if (actionObj.v != nullptr) {
			SetPosition(actionObj.id, *actionObj.v);
		}
		else {
			LOG(ERR) << "LightController::ProcessAction - update position requested but position value is null.";
		}
		break;

	case LIGHT_UPDATE_DIR:
		if (actionObj.v != nullptr) {
			SetDirection(actionObj.id, *actionObj.v);
		}
		else {
			LOG(ERR) << "LightController::ProcessAction - update direction requested but position value is null.";
		}
		
		break;


	default:
		LOG(ERR) << "LightController::ProcessAction - Unsupported command.";
	}

}


void LightController::UpdateLightLayer(Layer layer, int lightID) {

	switch (lightIDMap[lightID].first) {

	case LIGHT_PLAYER_FLASHLIGHT:
	case LIGHT_SPOT:
		spotLights[lightIDMap[lightID].second]->setLayer(layer);
		break;

	case LIGHT_POINT:
		pointLights[lightIDMap[lightID].second]->setLayer(layer);
		break;

	case LIGHT_DIRECTIONAL:
		directionalLights[lightIDMap[lightID].second]->setLayer(layer);
		break;

	default:
		LOG(ERR) << "LightController::UpdateLightLayer - Invalid light type.";
	}

}

//Insert a new light object to one of the 3 stacks of light, dependant on "type" parameter.
int LightController::InsertLight(
	LightTypes type,
	vec3 ambient,
	vec3 diffuse,
	vec3 specular,
	vec3 pos,
	vec3 direction,
	float cosInnerConeIn,
	float cosOuterConeIn,
	float constantIn,
	float linearIn,
	float exponentIn
) {

	if (shader == nullptr) { 
		LOG(ERR) << "LightController::InsertLight - No shader found, unable to insert new light.";
		return -1; 
	}

	int lightID = -1;

	switch (type) {

	case LIGHT_POINT:
		pointLights.push_back(new PointLight(shader, ambient, diffuse, specular, constantIn, linearIn, exponentIn));
		pointLights.back()->setPosition(pos);
		LOG(DEBUG) << "Inserted point light. ID: " << pointLights.size() - 1;
		LOG(DEBUG) << "constantIn, linearIn, exponentIn: " << constantIn << " " << linearIn << " " << exponentIn;
		lightID = pointLights.size() - 1;
		break;

	case LIGHT_SPOT:
	case LIGHT_PLAYER_FLASHLIGHT:
		spotLights.push_back(new SpotLight(shader, direction, ambient, diffuse, specular, pos, cosInnerConeIn, cosOuterConeIn, constantIn, linearIn, exponentIn));
		lightID = spotLights.size() - 1;
		LOG(DEBUG) << "Inserted spotlight.";
		break;

	case LIGHT_DIRECTIONAL:
		directionalLights.push_back(new DirectionalLight(shader, direction, ambient, diffuse, specular));	
		lightID = directionalLights.size() - 1;
		LOG(DEBUG) << "Inserted directional light.";
		break;

	default:
		LOG(ERR) << "LightController::InsertLight - Invalid light type.";
		return lightID;
	}

	lightIDMap.push_back(make_pair(type, lightID));

	return lightIDMap.size()-1;
}

//Set a light object's position in the world.
bool LightController::SetPosition(int lightID, vec3 pos) {
	
	switch (lightIDMap[lightID].first) {

	case LIGHT_PLAYER_FLASHLIGHT:
	case LIGHT_SPOT:
		spotLights[lightIDMap[lightID].second]->setPosition(pos);
		break;

	case LIGHT_POINT:
		pointLights[lightID]->setPosition(pos);
		break;

	default:
		LOG(ERR) << "LightController::SetDirection - Invalid light type.";
		return false;
	}

	return true;
}

//Set a light object's direction of projection
bool LightController::SetDirection(int lightID, vec3 direction) {
	LOG(DEBUG) << "LightController::SetDirection - Setting direction of light ID " << lightID;
	switch (lightIDMap[lightID].first) {

	case LIGHT_PLAYER_FLASHLIGHT:
	case LIGHT_SPOT:
		spotLights[lightIDMap[lightID].second]->setDirection(direction);
		break;

	case LIGHT_DIRECTIONAL:
		directionalLights[lightIDMap[lightID].second]->setDirection(direction);
		break;

	default:
		LOG(ERR) << "LightController::SetDirection - Invalid light type.";
		return false;
	}

	return true;
}

void LightController::DeleteLight(int lightID) {
	switch (lightIDMap[lightID].first) {

	case LIGHT_PLAYER_FLASHLIGHT:
	case LIGHT_SPOT:
		delete spotLights[lightIDMap[lightID].second];
		spotLights[lightIDMap[lightID].second] = nullptr;
		break;

	case LIGHT_DIRECTIONAL:
		delete directionalLights[lightIDMap[lightID].second];
		directionalLights[lightIDMap[lightID].second] = nullptr;
		break;

	case LIGHT_POINT:
		delete pointLights[lightIDMap[lightID].second];
		pointLights[lightIDMap[lightID].second] = nullptr;
		break;

	default:
		LOG(ERR) << "LightController::DeleteLight - Invalid light type.";
	}
}