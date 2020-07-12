#include <iostream>
#include <sstream>
#pragma comment (lib, "glew32s.lib")
#pragma comment (lib, "assimp-vc141-mtd.lib")
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Model.h"
#include "AnimatedModel.h"
#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "glm/ext.hpp"
//#include "Model.h"
#include "Log.h"


#define  GLM_FORCE_CTOR_INIT

using namespace glm;

structlog LOGCFG = {};
const char* APP_TITLE = "OmerGL";
int gWindowWidth = 1024;
int gWindowHeight = 768;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;
float gCubeAngle = 0.0f;


//cameraObj cameraObj;
FirstPersonCamera cameraObj;
float gYaw = 0.0f;
float gPitch = 0.0f;
float gRadius = 0.0f;
const float MOUSE_SENSITIVITY = 0.25F;

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_OnFrameBufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseMove(GLFWwindow* window, double posX, double posY);


void showFPS(GLFWwindow * window);
bool initOpenGL();



int main() {
	LOGCFG.level = INFO;
	if (!initOpenGL()) {
		std::cerr << "GLFW init error - aborting" << std::endl;
		return -1;
	}

	//creating shader
	ShaderProgram shaderProgram;
	shaderProgram.LoadShaders("basic.vert", "lighting_multifunc.frag");

	//model positions
	vec3 modelPos[] = {
		vec3(-2.5f, 1.0f, 0.0f),
		vec3(2.5f, 1.0f, 0.0f),
		vec3(0.0f, 0.0f, 2.0f),
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
	Mesh mesh[numModels];
	Mesh robet;
	//robet.loadObj("nanosuit.obj");

	
	Texture2D texture[numModels];
	Texture2D specularMap;
	
	//mesh[0].loadObj("crate.obj");
	//mesh[1].loadObj("woodCrate.obj");
	//mesh[2].loadObj("robot.obj");
	//mesh[3].loadObj("floor.obj");
	

	specularMap.loadTexture("container2_specular.png", true);
	specularMap.isSpecMap = true;
	
	//light position
	vec3 lightPos = cameraObj.getPosition();
	vec3 lightColor(1.0f, 1.0f, 1.0f);
	
	texture[0].loadTexture("crate.jpg", true);
	texture[1].loadTexture("woodcrate_diffuse.jpg", true);
	texture[2].loadTexture("robot_diffuse.jpg", true);
	texture[3].loadTexture("tile_floor.jpg", true);
	
	Texture2D nanosuitTex;
	nanosuitTex.loadTexture("Character Texture.png", true);
	AnimatedModel nanosuit(&shaderProgram, "char_running_v2.dae");
	//AnimatedModel bendingRod(&shaderProgram, "turnstick.dae");
	//AnimatedModel nanosuitAnimated(&shaderProgram, "Character Running.obj", "Character Running.dae");
	LOG() << "FINISHED LOADING MODELS";

	Mesh meshTest;


	double lastTime = glfwGetTime();
	float angle = 0.0f;

	cameraObj.setMoveSpeed(0.25f);

	PointLight pointLights(&shaderProgram, vec3(100.0f), vec3(100.0f), vec3(1.0f), 1.0f, 0.07f, 0.017f);
	SpotLight spotLights(&shaderProgram, cameraObj.getLook(), vec3(100.0f), vec3(100.0f), vec3(100.0f), vec3(lightPos), cos(radians(5.0f)), cos(radians(10.0f)), 1.0f, 0.07f, 0.017f);
	SpotLight spotLights2(&shaderProgram, vec3(0.0f, -1.0f, 0.0f), vec3(50.0f), vec3(100.0f), vec3(100.0f), vec3(0.0f, 10.0f, 0.0f), cos(radians(5.0f)), cos(radians(10.0f)), 1.0f, 0.07f, 0.017f);
	DirectionalLight dirLight(&shaderProgram, vec3(0.0f, -0.9f, -0.17f), vec3(2.0f, 2.0f, 2.0f), vec3(0.6f), vec3(0.0f, 0.0f, 0.0f));

	pointLights.setPosition(vec3(-8.0f, 1.0f, -5.0f));

	//main loop

	shaderProgram.Use();
	vec3 initPos = vec3(0.35f, 4.6f, 16.0f);
	cameraObj.setCameraPositionVectors(initPos);
	

	vec3 initLookAt = vec3(0.0f, 0.0f, 2.0f);
	gYaw = -89.9f;
	cameraObj.setCameraTargetVectors(initLookAt);
	mat4 model, view, projection;
	ShaderProgram::gProjection = &projection;
	ShaderProgram::gView = &view;

	LOG() << "ENTERING MAIN LOOP";

	while (!glfwWindowShouldClose(gWindow)) {

		showFPS(gWindow);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		LOG() << " --------------------  Delta time: " << deltaTime;
		
		//angle += (float) radians(deltaTime * 50.0f);


		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		
		//I need to derive the Yaw and Pitch of where I want the camera to initally face!
		cameraObj.ExecuteMove(gYaw, gPitch);

		lightPos = cameraObj.getPosition();
		lightPos.y += 0.5f;


		view = cameraObj.getViewMatrix();	
		projection = perspective(radians(45.f), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 100.0f);
		shaderProgram.SetGlobalUniforms();
		

		spotLights.setPosition(vec3(lightPos));
		spotLights.setDirection(cameraObj.getLook());
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


		//* Some models
		/*
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

		nanosuitTex.bind(0);
		nanosuit.DrawModel(vec3(0.0f, 0.0f, 0.0f), -1);
		nanosuitTex.unbind(0);

		
		texture[3].bind(0);
		//bendingRod.DrawModel(vec3(0.0f, 4.0f, 4.0f), -1);
		texture[3].unbind(0);
		

		// Swap front and back buffers
		glfwSwapBuffers(gWindow);
		//lastTime = currentTime;
	
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

	// render closer vertices last
	glEnable(GL_DEPTH_TEST); 

	// cull back-facing faces to improve performance
	glEnable(GL_CULL_FACE); 
	glFrontFace(GL_CCW);

	return true;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}


	//Add wireframe mode on `P` key
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		if (typeid(cameraObj) == typeid(FirstPersonCamera)) {
			cameraObj.move(FirstPersonCamera::DIRECTION::FORWARD);
		}
	}
	else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		if (typeid(cameraObj) == typeid(FirstPersonCamera)) {
			cameraObj.move(FirstPersonCamera::DIRECTION::NONE);
		}
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		if (typeid(cameraObj) == typeid(FirstPersonCamera)) {
			cameraObj.move(FirstPersonCamera::DIRECTION::BACK);
		}
	}
	else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		if (typeid(cameraObj) == typeid(FirstPersonCamera)) {
			cameraObj.move(FirstPersonCamera::DIRECTION::NONE);
		}
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		if (typeid(cameraObj) == typeid(FirstPersonCamera)) {
			cameraObj.move(FirstPersonCamera::DIRECTION::LEFT);
		}
	}
	else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		if (typeid(cameraObj) == typeid(FirstPersonCamera)) {
			cameraObj.move(FirstPersonCamera::DIRECTION::NONE);
		}
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		if (typeid(cameraObj) == typeid(FirstPersonCamera)) {
			cameraObj.move(FirstPersonCamera::DIRECTION::RIGHT);
		}
	}
	else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		if (typeid(cameraObj) == typeid(FirstPersonCamera)) {
			cameraObj.move(FirstPersonCamera::DIRECTION::NONE);
		}
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		if (typeid(cameraObj) == typeid(FirstPersonCamera)) {
			cameraObj.move(FirstPersonCamera::DIRECTION::UP);
		}
	}
	else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
		if (typeid(cameraObj) == typeid(FirstPersonCamera)) {
			cameraObj.move(FirstPersonCamera::DIRECTION::NONE);
		}
	}

}

void glfw_OnFrameBufferSize(GLFWwindow* window, int width, int height) {
	gWindowWidth = width;
	gWindowHeight = height;
	glViewport(0, 0, gWindowWidth, gWindowHeight);
}
 
void glfw_onMouseMove(GLFWwindow* window, double posX, double posY) {
	static glm::vec2 lastMousePos = glm::vec2(0.0);
	
	// update angles based on left mouse button input 
	if (glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_LEFT) == 1) {

		//1 PIXEL = .25 OF AN ANGLE DEGREE ROTATION
		gYaw += ((float)posX - lastMousePos.x) * MOUSE_SENSITIVITY;
		gPitch -= ((float)posY - lastMousePos.y) * MOUSE_SENSITIVITY;
	}

	// update angles based on left mouse button input 
	//Only applicable to retating camera
	if (glfwGetMouseButton(gWindow, GLFW_MOUSE_BUTTON_RIGHT) == 1) {

		//1 PIXEL = .25 OF AN ANGLE DEGREE ROTATION
		float dx = 0.01f * ((float)posX - lastMousePos.x);
		float dy = 0.01f * ((float)posY - lastMousePos.y);
		gRadius += dx - dy;
	}

	lastMousePos.x = (float)posX;
	lastMousePos.y = (float)posY;
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
		vec3 pos = cameraObj.getPosition();
		vec3 targ = cameraObj.getTarget();

		std::ostringstream outs;
		outs.precision(3);
		outs << std::fixed
			<< APP_TITLE << "   "
			<< "CAM POS: " << to_string(pos) << "   "
			<< "TARGET POS: " << to_string(targ) << "  (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		frameCount = 0;
	}

	frameCount++;
	
}