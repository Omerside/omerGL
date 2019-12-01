#include "ShaderProgram.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

ShaderProgram::ShaderProgram()
	: mHandle(0)
{
}


ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(mHandle);
}

bool ShaderProgram::LoadShaders(const char* vsFilename, const char* fsFilename) {
	string vsString = FileToString(vsFilename);
	string fsString = FileToString(fsFilename);
	cout << vsString << endl;
	const GLchar* vsSourcePtr = vsString.c_str();
	const GLchar* fsSourcePtr = fsString.c_str();

	//Below we create our fragment and vertex shaders using the text defined at the top.
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &vsSourcePtr, NULL);
	glShaderSource(fs, 1, &fsSourcePtr, NULL);

	glCompileShader(vs);
	CheckCompileErrors(vs, VERTEX);

	glCompileShader(fs);
	CheckCompileErrors(fs, FRAGMENT);


	//Create the program and attach the shaders we created to it
	mHandle = glCreateProgram();
	if (mHandle == 0)
	{
		std::cerr << "Unable to create shader program!" << std::endl;
		return false;
	}

	glAttachShader(mHandle, vs);
	glAttachShader(mHandle, fs);
	glLinkProgram(mHandle);
	CheckCompileErrors(mHandle, PROGRAM);


	//Delete the shaders after you linked them to the program. Good cleanup practice.
	glDeleteShader(vs);
	glDeleteShader(fs);

	return true;
}
void ShaderProgram::Use() {
	if (mHandle > 0) {
		//Use the "program", which is our two shaders (vertex and fragment)
		glUseProgram(mHandle);

	}
}

string ShaderProgram::FileToString(const string& filename) {
	std::stringstream ss;
	std::ifstream file;

	try {
		file.open(filename, std::ios::in);
		if (!file.fail()) {
			ss << file.rdbuf();
		}

		file.close();
	}
	catch (std::exception ex) {
		std::cerr << "Error reading shader file" << std::endl;
	}

	return ss.str();
}
void ShaderProgram::CheckCompileErrors(GLuint shader, ShaderType type) {
	int status = 0;
	if (type == PROGRAM) {
		glGetProgramiv(mHandle, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {

			GLint length = 0;
			glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &length);

			string errorLog(length, ' ');
			glGetProgramInfoLog(mHandle, length, &length, &errorLog[0]);
			std::cerr << "Error! Program failed to link: " << errorLog << std::endl;

		}
	}
	else {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint length = 0;
			glGetShaderiv(mHandle, GL_INFO_LOG_LENGTH, &length);

			string errorLog(length, ' ');
			glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
			std::cerr << "Error! Shader failed to link" << errorLog << std::endl;
		}

	}
}