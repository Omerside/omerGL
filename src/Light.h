#ifndef LIGHT_H
#define LIGHT_H
#include "WorldObject.h"
#include "ShaderProgram.h"
#include <string>
#include <cstring>
#include <vector>

using namespace glm;

struct FocalRadius {
	float cosOuterCone;
	float cosInnerCone;
};

struct AttentuationProperties {
	float constant;
	float linear;
	float exponent;
};

struct DrawData {
	const char* name;
	inline void updateData(float *in) { floatData = in; vec3Data = NULL; };
	inline void updateData(vec3 *in) { vec3Data = in; floatData = NULL; };
	inline float getFloatData() { return *floatData; };
	inline vec3 getVec3Data() { return *vec3Data; };
	inline const char getName() { return *name; };
	float *floatData = NULL;
	vec3 *vec3Data = NULL;

};



/*
BEGIN ABSTRACT LIGHT
*/

class Light : public WorldObject {
public:

	virtual vec3 getAmbient();
	virtual vec3 getDiffuse();
	virtual vec3 getSpecular();
	virtual void setAmbient(vec3 ambientInput);
	virtual void setDiffuse(vec3 diffuseInput);
	virtual void setSpecular(vec3 specInput);
	virtual void setAttenuationProperties(AttentuationProperties *attentuation, float constantIn, float linearIn, float exponentIn);
	virtual void setFocalRadius(FocalRadius *fRadius, float cosOuterConeIn, float cosInnerConeIn);
	virtual void setShader(ShaderProgram *shaderIn);
	virtual void SetUniformValues(DrawData drawValues[], int size);

protected:
	vec3 mAmbient = vec3(1.0f);
	vec3 mDiffuse = vec3(1.0f);
	vec3 mSpecular = vec3(1.0f);
	ShaderProgram *shader;
	string mStandardUniformBeginning;
	
};



/*
BEGIN DIRECTIONAL LIGHT 
*/
class DirectionalLight : public Light {
public:
	DirectionalLight(ShaderProgram *shaderIn, vec3 directionIn, vec3 ambientIn, vec3 diffuseIn, vec3 specIn);
	~DirectionalLight();

	void setDirection(vec3 directionIn);
	vec3 getDirection();
	void draw();

private:
	vec3 mDirection;
	static int dirLightCount;
};



/*
BEGIN POINT LIGHT
*/
class PointLight : public Light {
public:
	PointLight(
		ShaderProgram *shaderIn,
		vec3 ambientIn,
		vec3 diffuseIn,
		vec3 specIn,
		float constantIn,
		float linearIn,
		float exponentIn);
	~PointLight();

	void draw();


private:
	AttentuationProperties attentuation;
	static int pointLightCount;
};



/*
BEGIN SPOT LIGHT
*/
class SpotLight : public Light {
public:
	SpotLight(
		ShaderProgram *shaderIn,
		vec3 directionIn,
		vec3 ambientIn,
		vec3 diffuseIn,
		vec3 specIn,
		vec3 positionIn,
		float cosInnerConeIn,
		float cosOuterConeIn,
		float constantIn,
		float linearIn,
		float exponentIn);
	~SpotLight();

	void setDirection(vec3 directionIn);
	vec3 getDirection();
	void draw();
	

private:
	FocalRadius fRadius;
	AttentuationProperties attentuation;
	vec3 mDirection;
	static int spotLightCount;

};


#endif