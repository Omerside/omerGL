#include <iostream>
#include <sstream>
#include <windows.h>
#pragma comment (lib, "glew32s.lib")
#pragma comment (lib, "assimp-vc141-mtd.lib")
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "RootController.h"
#include "glm/ext.hpp"
#include "Log.h"

#define  GLM_FORCE_CTOR_INIT

using namespace glm;

//Logging software
structlog LOGCFG = {};

//declare controllers
RootController* rootCtrl;

//define window properties
const char* APP_TITLE = "OmerGL";
int gWindowWidth = 1768;
int gWindowHeight = 992;
GLFWwindow* gWindow = NULL;

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_OnFrameBufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseMove(GLFWwindow* window, double posX, double posY);
void showFPS(GLFWwindow * window);
void initControllers(GLFWwindow* gWindow);
bool initOpenGL();


int main() {
	LOGCFG.level = INFO;
	

	if (!initOpenGL()) {
		std::cerr << "GLFW init error - aborting" << std::endl;
		return -1;
	}

	//creating shader
	rootCtrl->LoadShaders();

	//Define some objects
	rootCtrl->LoadEntity("floor.dae", ENTITY_STATIC, "tile_floor.jpg", vec3(4, 1, 4), vec3(1));
	rootCtrl->LoadEntity("char_running_v2.dae", ENTITY_DYNAMIC, "Character Texture.png", vec3(1, 1, 1), vec3(5, 0, 1), true, "Armature.001");
	rootCtrl->LoadEntity("char_running_v2.dae", ENTITY_DYNAMIC, "Character Texture.png", vec3(1, 1, 1), vec3(1, 0, 5), true, "Armature.001", true);
	rootCtrl->LoadEntity("char_running_v2.dae", ENTITY_PLAYER_AVATAR, "Character Texture.png", vec3(1, 1, 1), vec3(5, 0, 5), true, "Armature.001", true);


	//Define some lights
	rootCtrl->LoadLight(LIGHT_DIRECTIONAL, vec3(0.0f, 0.0f, 0.0f), vec3(2.0f, 2.0f, 2.0f), vec3(1.6f), vec3(0.0f, 0.0f, 0.0f), vec3(0, -1, 0));
	rootCtrl->LoadLight(LIGHT_POINT, vec3(10), vec3(20.0f), vec3(10.0f));
	rootCtrl->LoadLight(LIGHT_SPOT, vec3(10), vec3(20.0f), vec3(10.0f), vec3(-10, 5, -10), vec3(0, -1, 0));
	rootCtrl->LoadLight(LIGHT_SPOT, vec3(10, 10, 10), vec3(20.0f, 0, 20), vec3(10.0f, 0, 10), vec3(10, 5, 10), vec3(0, -1, 0));
	rootCtrl->LoadLight(LIGHT_PLAYER_FLASHLIGHT, vec3(10, 10, 10), vec3(20.0f, 20, 20), vec3(10.0f, 0, 10), vec3(10, 5, 10), vec3(0, -1, 0));


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

		//Update window info
		showFPS(gWindow);


		//Update... the world :O
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
	initControllers(gWindow);


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

void initControllers(GLFWwindow* gWindowInput) {
	rootCtrl = rootCtrl->getInstance();
	rootCtrl->SetGlfWindow(gWindowInput);
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

		vec3 pos = rootCtrl->GetCameraPosition();
		vec3 targ = rootCtrl->GetLook();

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