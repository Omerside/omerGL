#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H
#include "GL/glew.h"
#include <string>
#include "glm/glm.hpp"
#include <map>

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
	void SetUniform(const GLchar* name, const GLfloat& f);
	void SetUniform(const GLchar* name, const glm::vec2& v);
	void SetUniform(const GLchar* name, const glm::vec3& v);
	void SetUniform(const GLchar* name, const glm::vec4& v);
	void SetUniform(const GLchar* name, const glm::mat4& m);
	void SetUniformSampler(const GLchar* name, const GLint& slot);
	void SetSpotLightCount(unsigned int count);
	void SetPointLightCount(unsigned int count);
	void SetDirectionalLightCount(unsigned int count);
	GLuint getProgram()const;

private:
	GLuint mHandle; // prefix m to denote it's a private class member

	string FileToString(const string& filename);
	void CheckCompileErrors(GLuint shader, ShaderType type);
	GLint GetUniformLocation(const GLchar* name);
	std::map<string, GLint> mUniformLocations;
	int spotLightCount, pointLightCount, DirectionalLightCount = 0;
	
};

#endif // SHADER PROGRAM_H