#include "RootController.h"
RootController* RootController::inst = NULL;

RootController::RootController() {
	lightActionsQueue.reserve(MAX_NUM_ENTITIES);
	entityActionsQueue.reserve(MAX_ACTIONS_PER_CYCLE);

	inputCtrl = inputCtrl->getInstance();
	playerCtrl = playerCtrl->getInstance();
	entityCtrl = entityCtrl->getInstance();
	lightCtrl = lightCtrl->getInstance();

	mp = inputCtrl->GetMouseProperties();
	mouseMoved.playerActionOnPress = PLAYER_MOVE_MOUSE;
	mouseMoved.playerDirectionOnPress = NONE;
	mouseMoved.SystemAction = SYSTEM_NONE;
	mouseMoved.type = PLAYER;
	
	/*
	MouseProperties newMp = (playerCtrl->CalcNewMouseProperties(*mp));
	mp->gPitch = newMp.gPitch;
	mp->gYaw = newMp.gYaw;
	*/
	lastTime = glfwGetTime();

	playerCtrl->cameraPos = &cameraPos;
	playerCtrl->viewMat = &viewMat;
	playerCtrl->lookAt = &lookAt;

	SetShader(new ShaderProgram());

	LOG(DEBUG) << "RootController::RootController - successfully initialized.";
}

RootController::~RootController() {
	delete inputCtrl;
	delete playerCtrl;
	delete entityCtrl;
	delete lightCtrl;
	delete shader;
}


RootController* RootController::getInstance() {
	if (inst == NULL) {
		inst = new RootController();
	}

	return(inst);
}

void RootController::CheckMouseInput(double posX, double posY) {
	LOG(DEBUG) << "RootController::CheckMouseInput - Checking mouse input... ";
	inputCtrl->OnMouseMove(posX, posY);
	playerCtrl->CalcLightAction(mouseMoved, GLFW_PRESS, lightActionsQueue);
	LOG(DEBUG) << "RootController::CheckMouseInput - Done ";
}

void RootController::CheckInputController(int key, int scancode, int action, int mode) {
	LOG(DEBUG) << "RootController::CheckInputController - Checking keyboard input... ";
	KeyAction ka = inputCtrl->OnKey(key, scancode, action, mode);
	playerCtrl->MoveDirection(ka, action);

	playerCtrl->CalcLightAction(ka, action, lightActionsQueue);
	SystemDirection(ka, action);
	LOG(DEBUG) << "RootController::CheckInputController - Done ";

}

void RootController::SystemDirection(KeyAction ka, int action) {
	if (ka.SystemAction == SYSTEM_CLOSE_WINDOW && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(gWindow, GL_TRUE);

	} else if (ka.SystemAction == SYSTEM_POLYGON_MODE && action == GLFW_PRESS) {
		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void RootController::Update() {
	double currentTime = glfwGetTime();
	double deltaTime = currentTime - lastTime;

	LOG(DEBUG) << "ATTEMPTING TO GET CAMERA PROPERTIES";
	view = playerCtrl->getViewMatrix();
	projection = perspective(radians(55.f), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 100.0f);
	shader->SetGlobalUniforms();

	
	cameraPos = playerCtrl->getPlayerCameraPosition();
	lookAt = playerCtrl->getLook();
	viewMat = playerCtrl->getViewMatrix();
	LOG(DEBUG) << "CAMERA PROPERTIES OBTAINED";



	//Reset a few GL parameters
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glDisable(GL_STENCIL_TEST);

	//Move camera
	LOG(DEBUG) << "ATTEMPTING TOP EXECUTE CAMERA MOVE";
	playerCtrl->ExecuteCameraMove(mp->gYaw, mp->gPitch);
	LOG(DEBUG) << "MOVE EXECUTED";

	//Draw world lights
	processLightActions();
	lightCtrl->DrawLights();

	//Draw Entities
	shader->SetUniform("material.shininess", 10.0f);
	entityCtrl->DrawEntities(&deltaTime);




	lastTime = currentTime;
}

void RootController::SetCameraLookAt(vec3 targ) {
	playerCtrl->SetLook(targ);
}

void RootController::SetGlfWindow(GLFWwindow* gWindowInput) { 
	gWindow = gWindowInput; 
	inputCtrl->SetGlfWindow(gWindow);
	playerCtrl->SetGlfWindow(gWindow);
	entityCtrl->SetGlfWindow(gWindow);
	lightCtrl->SetGlfWindow(gWindow);

	glfwGetWindowSize(gWindow, &gWindowWidth, &gWindowHeight);

};

void RootController::SetShader(ShaderProgram* shaderInput) {
	if (shader != nullptr) {
		delete shader;
	}

	shader = shaderInput;
	entityCtrl->shader = shader;
	lightCtrl->shader = shader;

	ShaderProgram::gProjection = &projection;
	ShaderProgram::gView = &view;

};

void RootController::LoadShaders(const char* vertShader, const char* fragShader) {
	//creating shader
	shader->LoadShaders(vertShader, fragShader);
	shader->Use();
}

int RootController::LoadEntity(
	const char* daeFile,
	EntityTypes type,
	const char* texFile,
	vec3 scale,
	vec3 pos,
	bool activeAnimLooping,
	const char* activeAnim,
	bool isOutlined,
	vec3 outlineColor,
	vec3 outlineColorHidden
) {
	

	int entityID = entityCtrl->LoadEntity(daeFile, texFile, type, pos);
	entityCtrl->SetEntityScale(entityID, &scale);

	if (activeAnim) {
		LOG(DEBUG) << "Found active animation. Setting ID " << entityID << " to have the active animation " << activeAnim;
		entityCtrl->SetActiveAnimation(entityID, activeAnim, &activeAnimLooping);
	}

	if (isOutlined) {
		LOG(DEBUG) << "Found outline.";
		entityCtrl->EnableEntityOutline(entityID, &outlineColor, &vec3(1.1, 1.0, 1.1), &outlineColorHidden);
	}

	LOG(DEBUG) << "Entity loaded.";
	return entityID;
}

int RootController::LoadLight(
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

	if (type == LIGHT_PLAYER_FLASHLIGHT) {
		
		int lightID = lightCtrl->InsertLight(type, ambient, diffuse, specular, pos, direction, cosInnerConeIn, cosOuterConeIn, constantIn, linearIn, exponentIn);
		if (playerCtrl->flashLightID != -1) {
			lightCtrl->DeleteLight(playerCtrl->flashLightID);
		}
			
		playerCtrl->flashLightID = lightID;
		return lightID;
	}
	else {
		return lightCtrl->InsertLight(type, ambient, diffuse, specular, pos, direction, cosInnerConeIn, cosOuterConeIn, constantIn, linearIn, exponentIn);
	}
}

void RootController::processLightActions() {
	playerCtrl->GetLightPersistentAction(lightActionsQueue);

	LOG(DEBUG) << "About to process light actions...";
	for_each(lightActionsQueue.begin(), lightActionsQueue.end(), [&](LightAction* &q) {
		lightCtrl->ProcessAction(*q);
		delete q;
	});

	lightActionsQueue.clear();

	LOG(DEBUG) << "PROCESSED ALL light actions. capacity:  " << lightActionsQueue.capacity();
}

void RootController::processEntityActions() {

}

