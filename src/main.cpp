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

#define  GLM_FORCE_CTOR_INIT

using namespace glm;

const char* APP_TITLE = "OmerGL";
int gWindowWidth = 1024;
int gWindowHeight = 768;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;
const std::string texture1 = "wooden_crate.jpg";
const std::string texture2 = "crate.jpg";

float gCubeAngle = 0.0f;

OrbitCamera orbitCamera;
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
	if (!initOpenGL()) {
		std::cerr << "GLFW init error - aborting" << std::endl;
		return -1;
	}
	GLfloat vertices[] = {
		// position		 // tex coords

	   // front face
	   -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
	   -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
	   -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

		// back face
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

		 // left face
		 -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		 -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
		 -1.0f,  1.0f,  1.0f, 1.0f, 1.0f,
		 -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		 -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		 -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

		 // right face
		  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		  1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		  1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
		  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
		  1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
		  1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

		  // top face
		 -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
		  1.0f,  1.0f, -1.0f, 1.0f, 1.0f,
		 -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,
		 -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
		  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,

		  // bottom face
		 -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
		  1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		  1.0f, -1.0f,  1.0f, 1.0f, 1.0f,
		 -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
		 -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		  1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
	};
	vec3 cubePos = vec3(0.0f, 0.0f, -5.0f);

	// 2. Set up buffers on the GPU
	GLuint vbo, vao;

	glGenBuffers(1, &vbo);					// Generate an empty vertex buffer on the GPU
	glBindBuffer(GL_ARRAY_BUFFER, vbo);		// "bind" or set as the current buffer we are working with
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	// copy the data from CPU to GPU

	glGenVertexArrays(1, &vao);				// Tell OpenGL to create new Vertex Array Object
	glBindVertexArray(vao);					// Make it the current one

	//Define position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);	// Define a layout for the first vertex buffer "0"
	glEnableVertexAttribArray(0);			// Enable the first attribute or attribute "0"

	//define texture 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);			//enable the second attribute.

	/* Set up index buffer (deprecated)
	glGenBuffers(1, &ibo);	// Create buffer space on the GPU for the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	*/

	glBindVertexArray(0);					// unbind to make sure other code doesn't change it


	//creating shader
	ShaderProgram shaderProgram;
	shaderProgram.LoadShaders("basic.vert", "basic.frag");

	Texture2D textureObj1;
	textureObj1.loadTexture(texture1, true);

	//Texture2D textureObj2;
	//textureObj2.loadTexture(texture2, true);

	float cubeAngle = 0.0f;
	double lastTime = glfwGetTime();

	//main loop
	while (!glfwWindowShouldClose(gWindow)) {
		showFPS(gWindow);
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//bind texture information we created
		textureObj1.bind(0);
		//textureObj2.bind(1);
		
		cubeAngle += (float)(deltaTime * 50.0f);
		if (cubeAngle >= 360.0f) cubeAngle = 0.0f;

		mat4 model, view, projection;

		orbitCamera.setLookAt(cubePos);
		orbitCamera.rotate(gYaw, gPitch);
		orbitCamera.setRadius(gRadius);

		model = translate(model, cubePos) * rotate(model, radians(cubeAngle), vec3(0.0f, 1.0f, 0.0));
		
		/* Hard-setting camera values
		vec3 camPos(0.0f, 0.0f, 0.0f); // where our camera is looking from.
		vec3 targetPos(0.0f, 0.0f, -1.0f); // What our target is
		vec3 up(0.0f, 1.0f, 0.0f); // The up vector is basically a vector defining your world's "upwards" direction
		*/

		//view = lookAt(camPos, targetPos, up); // creates a viewing matrix derived from an eye point, a reference point indicating the center of the scene, and an UP vector.
		view = orbitCamera.getViewMatrix();
		
		projection = perspective(radians(45.f), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 100.0f);

		//Use the "program", which is our two shaders (vertex and fragment)
		//glUseProgram(shaderProgram);
		shaderProgram.Use();

		shaderProgram.SetUniform("model", model);
		shaderProgram.SetUniform("view", view);
		shaderProgram.SetUniform("projection", projection);

		//Bind the vertex information we created
		//glBindVertexArray(vao);



		/*bind uniform values to texture bind values (deprecated)
		glUniform1i(glGetUniformLocation(shaderProgram.getProgram(), "myTexture"), 0);
		glUniform1i(glGetUniformLocation(shaderProgram.getProgram(), "myTexture2"), 1);
		*/

		//Defining the vlue blueColor to change based on time using some fun math.
		/*
		GLfloat time = glfwGetTime();
		GLfloat blueColor = (sin(time) / 2) + 0.5f;

		//Defining the position to vary and give our square a bounce.
		vec2 pos;
		pos.x = sin(time*5) / 4;
		pos.y = cos(time*10) / 2;

		shaderProgram.SetUniform("posOffset", pos);
		shaderProgram.SetUniform("vertColor", vec4(blueColor, 0.0f, 0.0f, 1.0f));
		*/

		//shaderProgram.SetUniform("vertColor", vec4(0.0f, 0.0f, 0.0f, 1.0f));
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// Swap front and back buffers
		glfwSwapBuffers(gWindow);
		lastTime = currentTime;
	}

	// Clean up
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

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

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW NOT OK" << std::endl;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//0.23f, 0.38f, 0.47f, 1.0f);
	glViewport(0, 0, gWindowWidth, gWindowHeight);
	glEnable(GL_DEPTH_TEST); // render closer vertices last


	return true;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}


	//Add wireframe mode on `W` key
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
		gYaw -= ((float)posX - lastMousePos.x) * MOUSE_SENSITIVITY;
		gPitch -= ((float)posY - lastMousePos.y) * MOUSE_SENSITIVITY;
	}

	// update angles based on left mouse button input 
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

		std::ostringstream outs;
		outs.precision(3);
		outs << std::fixed
			<< APP_TITLE << "   "
			<< "FPS: " << fps << "   "
			<< "FRAME TIME: " << msPerFrame << "  (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		frameCount = 0;
	}

	frameCount++;
	
}