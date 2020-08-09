#include <iostream>
#include <sstream>
#include <windows.h>
#pragma comment (lib, "glew32s.lib")
#pragma comment (lib, "assimp-vc141-mtd.lib")
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Model.h"
#include "AnimatedModel.h"
#include "ShaderProgram.h"
#include "RootController.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "glm/ext.hpp"
#include "Log.h"


#define  GLM_FORCE_CTOR_INIT

using namespace glm;

structlog LOGCFG = {};

//declare controllers
RootController* rootCtrl;

//declare shader program:
ShaderProgram shaderProgram;

//define window properties
const char* APP_TITLE = "OmerGL";
int gWindowWidth = 1024;
int gWindowHeight = 768;
GLFWwindow* gWindow = NULL;

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_OnFrameBufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseMove(GLFWwindow* window, double posX, double posY);
void showFPS(GLFWwindow * window);
void initControllers(GLFWwindow* gWindow, ShaderProgram* shaderProgram);
bool initOpenGL();


int main() {
	LOGCFG.level = INFO;
	

	if (!initOpenGL()) {
		std::cerr << "GLFW init error - aborting" << std::endl;
		return -1;
	}

	//creating shader
	shaderProgram.LoadShaders("basic.vert", "lighting_multifunc.frag");

	//Define some objects
	rootCtrl->LoadEntity("floor.dae", ENTITY_STATIC, "tile_floor.jpg", vec3(4, 1, 4), vec3(1));
	rootCtrl->LoadEntity("char_running_v2.dae", ENTITY_DYNAMIC, "Character Texture.png", vec3(1, 1, 1), vec3(5, 0, 1), true, "Armature.001");
	rootCtrl->LoadEntity("char_running_v2.dae", ENTITY_DYNAMIC, "Character Texture.png", vec3(1, 1, 1), vec3(1, 0, 5), true, "Armature.001", true);
	rootCtrl->LoadEntity("char_running_v2.dae", ENTITY_DYNAMIC, "Character Texture.png", vec3(1, 1, 1), vec3(5, 0, 5), true, "Armature.001", true);


	//Define some lights
	rootCtrl->LoadLight(LIGHT_DIRECTIONAL, vec3(0.0f, 0.0f, 0.0f), vec3(2.0f, 2.0f, 2.0f), vec3(1.6f), vec3(0.0f, 0.0f, 0.0f), vec3(0, -1, 0));
	rootCtrl->LoadLight(LIGHT_POINT, vec3(10), vec3(20.0f), vec3(10.0f));
	rootCtrl->LoadLight(LIGHT_SPOT, vec3(10), vec3(20.0f), vec3(10.0f), vec3(-10, 5, -10), vec3(0, -1, 0));
	rootCtrl->LoadLight(LIGHT_SPOT, vec3(10, 10, 10), vec3(20.0f, 0, 20), vec3(10.0f, 0, 10), vec3(10, 5, 10), vec3(0, -1, 0));
	rootCtrl->LoadLight(LIGHT_PLAYER_FLASHLIGHT, vec3(10, 10, 10), vec3(20.0f, 0, 20), vec3(10.0f, 0, 10), vec3(10, 5, 10), vec3(0, -1, 0));

	shaderProgram.Use();


	mat4 model, view, projection;
	ShaderProgram::gProjection = &projection;
	ShaderProgram::gView = &view;
	
	//main game loop
	while (!glfwWindowShouldClose(gWindow)) {
		
		/*Allows the windows message pump to freeze the game loop when needed.*/
		glfwPollEvents();

		/* Is equivalent to: 
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, 1) > 0) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		*/

		showFPS(gWindow);


		
		// Reset stencil and depth tests
		glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glDisable(GL_STENCIL_TEST);
		//**

		//REPLACED BY ROOTCONTROLLER
		view = rootCtrl->TempGetViewMatrix();
		projection = perspective(radians(55.f), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 100.0f);
		shaderProgram.SetGlobalUniforms();
		

		/*
		Calculating position of camera-light
		This will be used later in a controller.
		lightPos = rootCtrl->TempGetCameraPosition();//cameraObj.getPosition();
		lightPos.y += 0.5f;
		spotLights.setPosition(vec3(lightPos));
		*/
		

		//REPLACED BY rootcontroller
		//This will be used to calculate direction of camera-light
		//spotLights.setDirection(rootCtrl->TempGetLook());//(cameraObj.getLook());

		rootCtrl->Update();
		
		// Finalize and swap buffers
		glfwSwapBuffers(gWindow);		
	}

	glfwTerminate();
	return 0;
}

bool initOpenGL() {
	if (!glfwInit()) {
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
	initControllers(gWindow, &shaderProgram);


	if (gWindow == NULL) {
		std::cerr << "Failed to initialize window" << std::endl;
		return false;
	}

	

	glfwMakeContextCurrent(gWindow);
	glfwSetKeyCallback(gWindow, glfw_onKey);
	glfwSetCursorPosCallback(gWindow, glfw_onMouseMove);
	glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW NOT OK" << std::endl;
	}

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
	glViewport(0, 0, gWindowWidth, gWindowHeight);


	//The stencil test ensures undesired pixels do not reach the depth test. This saves processing time for the scene.
	glEnable(GL_STENCIL_TEST);

	// render closer vertices last
	glEnable(GL_DEPTH_TEST); 

	// cull back-facing faces to improve performance
	glEnable(GL_CULL_FACE); 
	glFrontFace(GL_CCW);

	

	return true;
}

void initControllers(GLFWwindow* gWindowInput, ShaderProgram* shaderProgram) {
	rootCtrl = rootCtrl->getInstance();
	rootCtrl->SetGlfWindow(gWindowInput);
	rootCtrl->SetShader(shaderProgram);
	
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
	rootCtrl->CheckInputController(key, scancode, action, mode);
}

void glfw_OnFrameBufferSize(GLFWwindow* window, int width, int height) {
	gWindowWidth = width;
	gWindowHeight = height;
	glViewport(0, 0, gWindowWidth, gWindowHeight);
}
 
void glfw_onMouseMove(GLFWwindow* window, double posX, double posY) {
	rootCtrl->CheckMouseInput(posX, posY);
}

void showFPS(GLFWwindow * window) {
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); // returns number of seconds since GLFW started (double)

	elapsedSeconds = currentSeconds - previousSeconds;

	//limit text update 4 times per second
	if (elapsedSeconds > 0.25) {
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		vec3 pos = rootCtrl->TempGetCameraPosition();
		vec3 targ = rootCtrl->TempGetLook();

		std::ostringstream outs;
		outs.precision(3);
		outs << std::fixed
			<< APP_TITLE << "   "
			<< "CAM POS: " << to_string(pos) << "   "
			<< "TARGET POS: " << to_string(targ) << ""
			<< "FPS: " << to_string(fps) << " ";
		glfwSetWindowTitle(window, outs.str().c_str());

		frameCount = 0;
	}

	frameCount++;
	
}