#include <iostream>
#include <sstream>
#pragma comment (lib, "glew32s.lib")
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

const char* APP_TITLE = "Introduction to Modern OpenGL - Hello Window 1";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
bool gFullScreen = false;

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void showFPS(GLFWwindow * window);

int main() {
	
	if (!glfwInit()) {
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* pWindow = NULL;
		
	if (gFullScreen) {
		GLFWmonitor * pMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* pVmode = glfwGetVideoMode(pMonitor);
		if (pVmode != NULL) {
			pWindow = glfwCreateWindow(pVmode->width, pVmode->height, APP_TITLE, pMonitor, NULL);
		}

	} else {
		pWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
	}

	if (pWindow == NULL) {
		std::cerr << "Failed to initialize window"<< std::endl;
		return -1;
	}

	glfwMakeContextCurrent(pWindow);
	glfwSetKeyCallback(pWindow, glfw_onKey);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW NOT OK" << std::endl;
	}

	//main loop
	while (!glfwWindowShouldClose(pWindow)) {
		showFPS(pWindow);
		glfwPollEvents();

		glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(pWindow);
	}

	glfwTerminate();
	return 0;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
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