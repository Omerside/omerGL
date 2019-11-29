#include <iostream>
#include <sstream>
#pragma comment (lib, "glew32s.lib")
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

const char* APP_TITLE = "OmerGL";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gWindow = NULL;

const GLchar* vertexShaderSrc =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;"
"void main()"
"{"
"gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);" 
"}";

const GLchar* fragmentShaderSrc =
"#version 330 core\n"
"out vec4 frag_color;"
"void main()"
"{"
"	frag_color = vec4(0.35f, 0.96f, 0.3f, 1.0f);"
"}";


void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void showFPS(GLFWwindow * window);
bool initOpenGL();

int main() {
	if (!initOpenGL()) {
		std::cerr << "GLFW init error - aborting" << std::endl;
		return -1;
	}

	/*\
		Why use GLfloat instead of float?
		Different OS's assign different mem values for native types.
		Using GLfloat guarantees the conditions necessary for our program to have proper memory allocated.
	*/
	GLfloat vertices[] = {
		//This is our triangle.
		0.5f, 0.5f, 0.0f, // top vertex
		0.5f, -0.5f, 0.0f,// right vertex
		-0.5f, -0.5f, 0.0f // left vertex
	};

	//Vertex-buffer object
	GLuint vbo;

	//Vertex-array object
	GLuint vao;

	//Create a buffer to allow memory to be stored in GPU, skip the bus!
	glGenBuffers(1, &vbo);

	//This makes our defined Vertex-buffer the current buffer. You can only have 1 active.
	//Why GL_ARRAY_BUFFER? Because vertices[] is an array.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//Give your buffer information on what you're about to use it for.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// define an array of generic vertex attribute data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);


	//Below we create our fragment and vertex shaders using the text defined at the top.
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexShaderSrc, NULL);
	glCompileShader(vs);
	GLint result;
	GLchar infoLog[512];
	glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(vs, sizeof(infoLog), NULL, infoLog);
		std::cout << "Error! vertex shader failed to compile. " << infoLog << std::endl;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(fs, sizeof(infoLog), NULL, infoLog);
		std::cout << "Error! Fragment shader failed to compile. " << infoLog << std::endl;
	}

	//Create the program and attach the shaders we created to it
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
		std::cout << "Error! shader program failed to compile. " << infoLog << std::endl;
	}
	
	//Delete the shaders after you linked them to the program. Good cleanup practice.
	glDeleteShader(vs);
	glDeleteShader(fs);

	//main loop
	while (!glfwWindowShouldClose(gWindow)) {
		showFPS(gWindow);
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);

		//Bind the vertex information we created
		glBindVertexArray(vao);

		//Use the "program", which is our two shaders (vertex and fragment)
		glUseProgram(shaderProgram);

		//Draw the triangle!
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//Unbind the vertex array, since we're done with it.
		glBindVertexArray(0);

		glfwSwapBuffers(gWindow);
	}

	glDeleteProgram(shaderProgram);
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

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cerr << "GLEW NOT OK" << std::endl;
	}

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);
	return true;
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