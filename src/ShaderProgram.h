#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H
#include "GL/glew.h"
#include <string>
using std::string;

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	enum ShaderType {
		VERTEX,
		FRAGMENT,
		PROGRAM
	};

	bool LoadShaders(const char* vsFilename, const char* fsFilename);
	void Use();

private:

	string FileToString(const string& filename);
	void CheckCompileErrors(GLuint shader, ShaderType type);
	GLuint mHandle; // prefix m to denote it's a private class member
};

#endif // SHADER PROGRAM_H