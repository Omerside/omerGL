#include <iostream>
#include <sstream>
#pragma comment (lib, "glew32s.lib")
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Mesh.h"
#include "glm/ext.hpp"

#define  GLM_FORCE_CTOR_INIT

using namespace glm;

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
const float MOUSE_SENSITIVITY = 0.15F;

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_OnFrameBufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseMove(GLFWwindow* window, double posX, double posY);

void showFPS(GLFWwindow * window);
bool initOpenGL();


int main() {
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
	Mesh sphere;
	
	Texture2D texture[numModels];

	mesh[0].loadObj("crate.obj");
	mesh[1].loadObj("woodCrate.obj");
	mesh[2].loadObj("robot.obj");
	mesh[3].loadObj("floor.obj");

	sphere.loadObj("sphere.obj");
	
	//light position
	vec3 lightPos = cameraObj.getPosition();
	vec3 lightColor(1.0f, 1.0f, 1.0f);

	texture[0].loadTexture("crate.jpg", true);
	texture[1].loadTexture("woodcrate_diffuse.jpg", true);
	texture[2].loadTexture("robot_diffuse.jpg", true);
	texture[3].loadTexture("tile_floor.jpg", true);


	double lastTime = glfwGetTime();
	float angle = 0.0f;

	cameraObj.setMoveSpeed(0.025f);
	cameraObj.setCameraPositionVectors(10.3f, 1.5f, 10.0f);

	//main loop
	while (!glfwWindowShouldClose(gWindow)) {
		showFPS(gWindow);
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		
		angle += radians(deltaTime * 50.0f);
			
		//lightPos.x = 10.0f * sinf(angle);
		//lightPos.x -= 0.5f;

		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		mat4 model, view, projection;

		
		//cameraObj.rotateOnCamera(gYaw, gPitch);
		//cameraObj.rotateOnObject(gYaw, gPitch);
		cameraObj.ExecuteMove(gYaw, gPitch);
		//cameraObj.setRadius(gRadius);
		lightPos = cameraObj.getPosition();
		lightPos.y += 0.5f;


		view = cameraObj.getViewMatrix();	
		projection = perspective(radians(45.f), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 100.0f);

		//Use the "program", which is our two shaders (vertex and fragment)
		//glUseProgram(shaderProgram);
		shaderProgram.Use();
		shaderProgram.SetUniform("viewPos", cameraObj.getPosition());


		//directional light
		shaderProgram.SetUniform("view", view);
		shaderProgram.SetUniform("dirLight.ambient", glm::vec3(0.2f));
		shaderProgram.SetUniform("dirLight.diffuse", glm::vec3(0.5f));
		shaderProgram.SetUniform("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shaderProgram.SetUniform("dirLight.direction", vec3(0.0f, -0.9f, -0.17f));

		//spot light
		shaderProgram.SetUniform("spotLight.ambient", glm::vec3(1.0f, 0.1f, 1.0f));
		shaderProgram.SetUniform("spotLight.diffuse", glm::vec3(100.0f, 10.0f, 1.2f));
		shaderProgram.SetUniform("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		shaderProgram.SetUniform("spotLight.position", vec3(lightPos));//0.0f, -0.9f, -0.17f));
		shaderProgram.SetUniform("spotLight.direction", cameraObj.getLook());
		shaderProgram.SetUniform("spotLight.cosInnerCone", cos(radians(5.0f)));
		shaderProgram.SetUniform("spotLight.cosOuterCone", cos(radians(10.0f)));
		shaderProgram.SetUniform("spotLight.constant", 1.0f);
		shaderProgram.SetUniform("spotLight.linear", 0.07f);
		shaderProgram.SetUniform("spotLight.exponent", 0.017f);
		

		//point light
		shaderProgram.SetUniform("pointLight.ambient", vec3(1.0f, 1.0f, 1.0f));
		shaderProgram.SetUniform("pointLight.diffuse", vec3(10.0f));
		shaderProgram.SetUniform("pointLight.specular", vec3(0.1f, 0.1f, 0.1f));
		shaderProgram.SetUniform("pointLight.position", vec3(20.0f, 1.0f, 20.0f));
		shaderProgram.SetUniform("pointLight.constant", 1.0f);
		shaderProgram.SetUniform("pointLight.linear", 0.07f);
		shaderProgram.SetUniform("pointLight.exponent", 0.017f);


		model = translate(mat4(), vec3(3.0f, 2.0f, 2.0f));
		shaderProgram.SetUniform("model", model);
		sphere.draw();

		for (int i = 0; i < numModels; i++) {
			model = translate(mat4(), modelPos[i]) * scale(mat4(), modelScale[i]);
			shaderProgram.SetUniform("model", model);

			shaderProgram.SetUniform("material.ambient", vec3(0.1f, 0.1f, 0.1f));
			shaderProgram.SetUniform("material.specular", vec3(0.8f, 0.8f, 0.8f));
			shaderProgram.SetUniform("material.shininess", 35.0f);
			shaderProgram.SetUniformSampler("material.textureMap", 0);

			texture[i].bind(0);
			mesh[i].draw();
			texture[i].unbind(0);

		}
		shaderProgram.SetUniform("projection", projection);

		// Swap front and back buffers
		glfwSwapBuffers(gWindow);
		lastTime = currentTime;
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
	glEnable(GL_DEPTH_TEST); // render closer vertices last


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