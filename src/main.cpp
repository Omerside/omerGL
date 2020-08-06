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

bool gWireframe = false;
float gCubeAngle = 0.0f;


//cameraObj cameraObj;
//FirstPersonCamera cameraObj;
float gYaw = 0.0f;
float gPitch = 0.0f;
float gRadius = 0.0f;
const float MOUSE_SENSITIVITY = 0.25F;

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

	//model positions
	vec3 modelPos[] = {
		vec3(-2.5f, 1.0f, 0.0f),
		vec3(2.5f, -1.0f, 0.0f),
		vec3(3.0f, 0.0f, 6.0f),
		vec3(0.0f, 0.0f, 0.0f)
	};

	//model scaling
	vec3 modelScale[] = {
		vec3(1.0f, 1.0f, 1.0f),
		vec3(1.0f, 1.0f, 1.0f),
		vec3(1.0f, 1.0f, 1.0f),
		vec3(10.0f, 0.5f, 10.0f)
	};

	
	//Load meshes/textures
	const int numModels = 4;
	/*
	Mesh mesh[numModels];
	Mesh robet;
	//robet.loadObj("nanosuit.obj");

	
	Texture2D texture[numModels];
	Texture2D specularMap;
	
	mesh[0].loadObj("crate.obj");
	mesh[1].loadObj("woodCrate.obj");
	mesh[2].loadObj("robot.obj");
	mesh[3].loadObj("floor.obj");
	*/
	

	//specularMap.loadTexture("container2_specular.png", true);
	//specularMap.isSpecMap = true;
	

	//light position
	vec3 lightPos = vec3(0.0f);//cameraObj.getPosition();
	vec3 lightColor(1.0f, 1.0f, 1.0f);
	/*
	texture[0].loadTexture("crate.jpg", true);
	texture[1].loadTexture("woodcrate_diffuse.jpg", true);
	texture[2].loadTexture("robot_diffuse.jpg", true);
	texture[3].loadTexture("tile_floor.jpg", true);
	*/
	Texture2D nanosuitTex;
	Texture2D floorTex;
	nanosuitTex.loadTexture("Character Texture.png", true);
	floorTex.loadTexture("tile_floor.jpg",true);

	AnimatedModel nanosuit(&shaderProgram, "char_running_v2.dae");

	//AnimatedModel floor(&shaderProgram, "floor.dae");

	rootCtrl->LoadEntity("floor.dae", ENTITY_STATIC, "tile_floor.jpg", vec3(4, 1, 4), vec3(1));
	rootCtrl->LoadEntity("char_running_v2.dae", ENTITY_DYNAMIC, "Character Texture.png", vec3(1, 1, 1), vec3(1, 0, 4), TRUE, "Armature.001");
	rootCtrl->LoadEntity("char_running_v2.dae", ENTITY_DYNAMIC, "Character Texture.png", vec3(1, 1, 1), vec3(4, 0, 1), TRUE, "Armature.001");
	//rootCtrl->LoadEntity("char_running_v2.dae", ENTITY_DYNAMIC, "Character Texture.png", vec3(1, 1, 1), vec3(1, 0, 1), true, "Armature.001");

	//floor.SetScale(vec3(4, 1, 4));

	//LOG(DEBUG) << "DONE";
	nanosuit.SetActiveAnimation("Armature.001", true);
	nanosuit.SetScale(vec3(1, 1, 1));
	//nanosuit.EnableOutline(vec3(0.0f,0.0f,0.0f), vec3(1.1, 1, 1.1), vec3(1.0f, 0.0f, 0.0f));

	LOGCFG.level = ERR;
	//AnimatedModel bendingRod(&shaderProgram, "turnstick.dae");
	//AnimatedModel nanosuitAnimated(&shaderProgram, "Character Running.obj", "Character Running.dae");

	double lastTime = glfwGetTime();
	float angle = 0.0f;

	//REPLACED IN ROOTCONTROLLER
	//cameraObj.setMoveSpeed(0.25f);

	PointLight pointLights(&shaderProgram, vec3(100.0f), vec3(100.0f), vec3(1.0f), 1.0f, 0.07f, 0.017f);
	//SpotLight spotLights(&shaderProgram, cameraObj.getLook(), vec3(10.50f), vec3(30.0f), vec3(30.0f), vec3(lightPos), cos(radians(5.0f)), cos(radians(10.0f)), 1.0f, 0.07f, 0.017f);
	SpotLight spotLights(&shaderProgram, rootCtrl->TempGetLook(), vec3(10.50f), vec3(30.0f), vec3(30.0f), vec3(lightPos), cos(radians(5.0f)), cos(radians(10.0f)), 1.0f, 0.07f, 0.017f);

	
	SpotLight spotLights2(&shaderProgram, vec3(0.0f, -1.0f, 0.0f), vec3(50.0f), vec3(100.0f), vec3(100.0f), vec3(0.0f, 10.0f, 0.0f), cos(radians(5.0f)), cos(radians(10.0f)), 1.0f, 0.07f, 0.017f);
	DirectionalLight dirLight(&shaderProgram, vec3(0.0f, -0.9f, -0.17f), vec3(2.0f, 2.0f, 2.0f), vec3(0.6f), vec3(0.0f, 0.0f, 0.0f));

	pointLights.setPosition(vec3(-8.0f, 1.0f, -5.0f));

	//main loop

	shaderProgram.Use();
	vec3 initPos = vec3(0.35f, 4.6f, 16.0f);

	//REPLACED IN ROOTCONTROLLER
	//cameraObj.setCameraPositionVectors(initPos);

	
	//rootCtrl->SetCameraLookAt(vec3(0));

	//REPLACED IN ROOTCONTROLLER
	//cameraObj.setCameraTargetVectors(initLookAt);
	mat4 model, view, projection;
	ShaderProgram::gProjection = &projection;
	ShaderProgram::gView = &view;
	

	//LOG() << "ENTERING MAIN LOOP";

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

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		showFPS(gWindow);


		
		// Reset stencil and depth tests
		glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glDisable(GL_STENCIL_TEST);
		//**
		//Calculate camera face direction
		//REPLACED BY ROOTCONTROLLER UPDATE()
		//cameraObj.ExecuteMove(gYaw, gPitch);
		
		

		lightPos = rootCtrl->TempGetCameraPosition();//cameraObj.getPosition();
		lightPos.y += 0.5f;

		//REPLACED BY ROOTCONTROLLER
		view = rootCtrl->TempGetViewMatrix();//cameraObj.getViewMatrix();	

		projection = perspective(radians(45.f), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 100.0f);
		shaderProgram.SetGlobalUniforms();
		

		spotLights.setPosition(vec3(lightPos));

		//REPLACED BY rootcontroller
		spotLights.setDirection(rootCtrl->TempGetLook());//(cameraObj.getLook());
		//pointLights.draw();
		dirLight.draw();
		spotLights.draw();
		//spotLights2.draw();
		


		model = translate(mat4(), modelPos[0]) * scale(mat4(), modelScale[0]);
		shaderProgram.SetUniform("model", model);

		shaderProgram.SetUniform("material.ambient", vec3(1.0f, 1.0f, 1.0f));
		shaderProgram.SetUniform("material.specular", vec3(1.0f, 1.0f, 1.0f));
		shaderProgram.SetUniform("material.shininess", 90.01f);
		shaderProgram.SetUniformSampler("material.textureMap", 0);
		shaderProgram.SetUniformSampler("material.specularMap", 1);

		//pointLights.draw();


		/* Some models
		
		for (int i = 1; i < numModels; i++) {
			model = translate(mat4(), modelPos[i]) * scale(mat4(), modelScale[i]);
			shaderProgram.SetUniform("model", model);


			shaderProgram.SetUniform("material.ambient", vec3(0.5f, 0.5f, 0.5f));
			shaderProgram.SetUniform("material.specular", vec3(0.4f, 0.4f, 0.4f));
			shaderProgram.SetUniform("material.shininess", 35.0f);
			shaderProgram.SetUniformSampler("material.textureMap", 0);
			shaderProgram.SetUniformSampler("material.specularMap", 1);

			texture[i].bind(0);
			mesh[i].draw();
			texture[i].unbind(0);

		}
		*/

		//LOGCFG.level = INFO;



		LOG(INFO) << "\n\n========== NO CONTROLLER ==========\n\n";
		//Stencil testing - Let's make an outline/shadow!
		if (nanosuit.getOutline()) {

			//Enable stencil testing.. duhg
			glEnable(GL_STENCIL_TEST);

			//Whatever is drawn next will (GL_)ALWAYS have a stencil value of 1 - meaning it will be drawn.
			glStencilFunc(GL_ALWAYS, 1, 0xFF);

			nanosuitTex.bind(0);
			nanosuit.DrawModel(vec3(-1.0f, 0.0f, -1.0f), -1, deltaTime);
			nanosuitTex.unbind(0);

			//Whatever is drawn next will only only be drawn if it has a value of 0 in the stencil buffer 
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			nanosuit.DrawOutline(vec3(0.0f, 0.0f, 0.0f));

			//We disable depth testing too, as otherwise the outline will fail depth test and not appear in front of other objects.
			glDisable(GL_DEPTH_TEST);

			nanosuit.DrawOutlineHidden(vec3(-1.0f, 0.0f, -1.0f));

			//Disable stencil testing and enable depth testing to resume normal drawing.
			glDisable(GL_STENCIL_TEST);
			glEnable(GL_DEPTH_TEST);
		}
		else {
			nanosuitTex.bind(0);
			nanosuit.DrawModel(vec3(0.0f, 0.0f, 0.0f), -1, deltaTime);
			nanosuitTex.unbind(0);
		}

		LOG(INFO) << "\n\n========== ROOT CONTROLLER ==========\n\n";
		rootCtrl->Update();
		
		//LOG(DEBUG) << "delta time: " << deltaTime;
		//floorTex.bind(0);
		//floor.DrawModel(vec3(0.0f, -4.0f, 0.0f), -1, deltaTime);
		//floorTex.unbind(0);
		

		// Finalize and swap buffers
		glfwSwapBuffers(gWindow);
		lastTime = currentTime;
		//glDisable(GL_STENCIL_TEST);
		// **
		
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
		//vec3 pos = cameraObj.getPosition();
		//vec3 targ = cameraObj.getTarget();

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