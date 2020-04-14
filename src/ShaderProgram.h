#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H
#include "Log.h"
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
	void SetUniform(const GLchar* name, const glm::mat4* mArr);
	void SetUniformSampler(const GLchar* name, const GLint& slot);

	void SetGlobalUniforms();
	GLuint getProgram()const;
	static glm::mat4 *gProjection;
	static glm::mat4 *gView;


private:
	GLuint mHandle; 

	string FileToString(const string& filename);
	void CheckCompileErrors(GLuint shader, ShaderType type);
	GLint GetUniformLocation(const GLchar* name);
	std::map<string, GLint> mUniformLocations;

};

#endif // SHADER PROGRAM_H