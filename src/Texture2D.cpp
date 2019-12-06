#include "Texture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include <iostream>
using namespace std;


Texture2D::Texture2D() {

}

Texture2D::~Texture2D() {
	
}

bool Texture2D::loadTexture(const string& filename, bool generateMipMaps) {
	int width, height, components;
	unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &components, STBI_rgb_alpha);

	if (imageData == NULL) {
		cerr << "Error loading texture " << filename << endl;
		return false;
	}

	//invert image
	int widthInBytes = width * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int halfHeight = height / 2;
	for (int row = 0; row < halfHeight; row++)
	{
		top = imageData + row * widthInBytes;
		bottom = imageData + (height - row - 1) * widthInBytes;
		for (int col = 0; col < widthInBytes; col++)
		{
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	//Bind to the current texture. 
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	//Define properties for texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S = X axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // T = y axis

	//Define properties for image filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //If the texture is smaller than the object, it will use GL_LINEAR to compensate.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // If you're shrinking the texture to fit the object, use GL_LINEAR to compensate.

	//specify a two-dimensional texture image - i.e, the file's unsigned char pointer.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

	if (generateMipMaps) {
		//A mipmap is the recursively-smaller version of a texture that is often used when the object is far away and doesn't require as high a quality.
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//free memory
	stbi_image_free(imageData);

	//Unbind the texture.
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

void Texture2D::bind(GLuint texUnit) {
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}