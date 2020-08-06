#include "RootController.h"
RootController* RootController::inst = NULL;

RootController::RootController() {
	inputCtrl = inputCtrl->getInstance();
	playerCtrl = playerCtrl->getInstance();
	entityCtrl = entityCtrl->getInstance();

	mp = inputCtrl->GetMouseProperties();
	
	/*
	MouseProperties newMp = (playerCtrl->CalcNewMouseProperties(*mp));
	mp->gPitch = newMp.gPitch;
	mp->gYaw = newMp.gYaw;
	*/
	lastTime = glfwGetTime();
}

RootController::~RootController() {
	delete inputCtrl;
	delete playerCtrl;
	delete entityCtrl;
}

void RootController::CheckMouseInput(double posX, double posY) {
	inputCtrl->OnMouseMove(posX, posY);
}


RootController* RootController::getInstance() {
	if (inst == NULL) {
		inst = new RootController();
	}

	return(inst);
}


void RootController::CheckInputController(int key, int scancode, int action, int mode) {
	KeyAction ka = inputCtrl->OnKey(key, scancode, action, mode);
	playerCtrl->MoveDirection(ka, action);
	SystemDirection(ka, action);

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
	
	LOG(DEBUG) << "Executing Camera move";
	playerCtrl->ExecuteCameraMove(mp->gYaw, mp->gPitch);

	LOG(DEBUG) << "Drawing entity";
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
};

void RootController::SetShader(ShaderProgram* shaderInput) {
	shader = shaderInput;
	entityCtrl->shader = shader;
};

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
		LOG(INFO) << "Found active animation. Setting ID " << entityID << " to have the active animation " << activeAnim;
		entityCtrl->SetActiveAnimation(entityID, activeAnim, &activeAnimLooping);
	}

	if (isOutlined) {
		LOG(DEBUG) << "Found outline.";
		entityCtrl->EnableEntityOutline(entityID, &outlineColor, &vec3(1.1, 1.0, 1.1), &outlineColorHidden);
	}

	LOG(INFO) << "Entity loaded.";
	return entityID;
}