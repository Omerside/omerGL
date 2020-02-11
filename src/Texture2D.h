#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "glm/glm.hpp"
#include "GL/glew.h"
#include <string>

using std::string;

class Texture2D {
public:
	Texture2D();
	virtual ~Texture2D();
	bool loadTexture(const  string& filename, bool generateMipMaps = true, bool flipImageData = true);
	void bind(GLuint texUnit = 0);
	void unbind(GLuint texUnit = 0);
	GLuint getTextureId();

	//texture properties
	bool isSpecMap = false;
	float shininess = 0;
	glm::vec3 specular = glm::vec3(1.0f);
	glm::vec3 ambient = glm::vec3(1.0f);
	unsigned int id = 0;
	string filePath;


private:
	GLuint mTexture; 
};

#endif