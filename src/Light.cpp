#include "Light.h"
static int spotLightCount;


vec3 Light::getAmbient() {
	return mAmbient;
}

vec3 Light::getDiffuse() {
	return mDiffuse;
}

vec3 Light::getSpecular() {
	return mSpecular;
}

void Light::setAmbient(vec3 ambientInput) {
	mAmbient = ambientInput;
}

void Light::setDiffuse(vec3 diffuseInput) {
	mDiffuse = diffuseInput;
}

void Light::setSpecular(vec3 specInput) {
	mSpecular = specInput;
}

void Light::setFocalRadius(FocalRadius *fRadius, float cosOuterConeIn, float cosInnerConeIn) {
	fRadius->cosInnerCone = cosInnerConeIn;
	fRadius->cosOuterCone = cosOuterConeIn;
}

void Light::setAttenuationProperties(AttentuationProperties *attentuation, float constantIn, float linearIn, float exponentIn) {
	attentuation->constant = constantIn;
	attentuation->linear = linearIn;
	attentuation->exponent = exponentIn;
}

void Light::setShader(ShaderProgram *shaderIn) {
	shader = shaderIn;
}


//First attempt at making a generic function to post all values to shader regardless of type of light.
void Light::SetUniformValues(DrawData drawValues[], int size) {
	for (int i = 0; i < size; i++) {
		std::cout << "i VALUE: " + drawValues[i].getName() << std::endl;
		std::cout << "UNIFORM DATA: " << std::endl;
		std::cout << drawValues[i].name << std::endl;
		if (drawValues[i].floatData == NULL) {
			shader->SetUniform(drawValues[i].name, drawValues[i].getVec3Data());

			std::cout << drawValues[i].getVec3Data().x << std::endl;
			std::cout << drawValues[i].getVec3Data().y << std::endl;
			std::cout << drawValues[i].getVec3Data().z << std::endl;
		}
		else {
			shader->SetUniform(drawValues[i].name, drawValues[i].getFloatData());
			std::cout << drawValues[i].getFloatData() << std::endl;
		}
	}	
}


/*
BEGIN DIRECTIONAL LIGHT
*/
int DirectionalLight::dirLightCount = 0;


DirectionalLight::DirectionalLight(ShaderProgram *shaderIn, vec3 directionIn, vec3 ambientIn, vec3 diffuseIn, vec3 specIn) {
	setShader(shaderIn);
	setDirection(directionIn);
	setAmbient(ambientIn);
	setDiffuse(diffuseIn);
	setSpecular(specIn);

	dirLightCount++;
	
}

DirectionalLight::~DirectionalLight() {
	dirLightCount--;
}

void DirectionalLight::setDirection(vec3 directionIn) {
	mDirection = directionIn;
}

vec3 DirectionalLight::getDirection() {
	return mDirection;
}

void DirectionalLight::draw() {
	if (objectLayer != INVISIBLE){
		shader->SetUniform("dirLight.ambient", mAmbient);
		shader->SetUniform("dirLight.diffuse", mDiffuse);
		shader->SetUniform("dirLight.specular", mSpecular);
		shader->SetUniform("dirLight.direction", mDirection);
	} else {
		shader->SetUniform((mStandardUniformBeginning + ".ambient").c_str(), vec3(0));
		shader->SetUniform((mStandardUniformBeginning + ".direction").c_str(), vec3(0));
		shader->SetUniform((mStandardUniformBeginning + ".diffuse").c_str(), vec3(0));
	}
}


/*
BEGIN POINT LIGHT
*/
int PointLight::pointLightCount = 0;

PointLight::PointLight(ShaderProgram *shaderIn, vec3 ambientIn, vec3 diffuseIn, vec3 specIn, float constantIn, float linearIn, float exponentIn) {
	setShader(shaderIn);
	setAmbient(ambientIn);
	setDiffuse(diffuseIn);
	setSpecular(specIn);
	setAttenuationProperties(&attentuation, constantIn, linearIn, exponentIn);

	pointLightCount++;
	mStandardUniformBeginning = "pointLight[" + std::to_string(pointLightCount - 1) + "]";

}

PointLight::~PointLight() {
	pointLightCount--;
}

void PointLight::draw() { 
	LOG(DEBUG) << "PointLight Info: \n" <<
		"mAmbient: " << mAmbient <<
		"mDiffuse: " << mDiffuse<<
		"mSpecular: " << mSpecular <<
		"mPosition: " << mPosition <<
		"constant: " << attentuation.constant << "\n" <<
		"linear: " << attentuation.linear << "\n" <<
		"exponent: " << attentuation.exponent << "\n\n";

	if (objectLayer != INVISIBLE) {
		shader->SetUniform((mStandardUniformBeginning + ".ambient").c_str(), mAmbient);
		shader->SetUniform((mStandardUniformBeginning + ".diffuse").c_str(), mDiffuse);
		shader->SetUniform((mStandardUniformBeginning + ".specular").c_str(), mSpecular);
		shader->SetUniform((mStandardUniformBeginning + ".position").c_str(), mPosition);
		shader->SetUniform((mStandardUniformBeginning + ".constant").c_str(), attentuation.constant);
		shader->SetUniform((mStandardUniformBeginning + ".linear").c_str(), attentuation.linear);
		shader->SetUniform((mStandardUniformBeginning + ".exponent").c_str(), attentuation.exponent);
	} else {
		shader->SetUniform((mStandardUniformBeginning + ".ambient").c_str(), vec3(0));
		shader->SetUniform((mStandardUniformBeginning + ".direction").c_str(), vec3(0));
		shader->SetUniform((mStandardUniformBeginning + ".diffuse").c_str(), vec3(0));
	}
}



/*
BEGIN SPOT LIGHT
*/
int SpotLight::spotLightCount = 0;

SpotLight::SpotLight(ShaderProgram *shaderIn, vec3 directionIn, vec3 ambientIn, vec3 diffuseIn, vec3 specIn, vec3 positionIn, float cosInnerConeIn, float cosOuterConeIn, float constantIn, float linearIn, float exponentIn) {
	setShader(shaderIn);
	setDirection(directionIn);
	setFocalRadius(&fRadius, cosOuterConeIn, cosInnerConeIn);
	setAmbient(ambientIn);
	setDiffuse(diffuseIn);
	setSpecular(specIn);
	setPosition(positionIn);
	setAttenuationProperties(&attentuation, constantIn, linearIn, exponentIn);

	spotLightCount++;
	mStandardUniformBeginning = "spotLight[" + std::to_string(spotLightCount - 1) + "]";
}

SpotLight::~SpotLight() {
	spotLightCount--;
}

void SpotLight::setDirection(vec3 directIn) {
	mDirection = directIn;
}

vec3 SpotLight::getDirection() {
	return mDirection;
}

void SpotLight::draw() {
	if (objectLayer != INVISIBLE) {
		shader->SetUniform((mStandardUniformBeginning + ".ambient").c_str(), mAmbient);
		shader->SetUniform((mStandardUniformBeginning + ".direction").c_str(), mDirection);
		shader->SetUniform((mStandardUniformBeginning + ".diffuse").c_str(), mDiffuse);
		shader->SetUniform((mStandardUniformBeginning + ".specular").c_str(), mSpecular);
		shader->SetUniform((mStandardUniformBeginning + ".position").c_str(), mPosition);
		shader->SetUniform((mStandardUniformBeginning + ".constant").c_str(), attentuation.constant);
		shader->SetUniform((mStandardUniformBeginning + ".linear").c_str(), attentuation.linear);
		shader->SetUniform((mStandardUniformBeginning + ".exponent").c_str(), attentuation.exponent);
		shader->SetUniform((mStandardUniformBeginning + ".cosInnerCone").c_str(), fRadius.cosInnerCone);
		shader->SetUniform((mStandardUniformBeginning + ".cosOuterCone").c_str(), fRadius.cosOuterCone);
	} else {
		shader->SetUniform((mStandardUniformBeginning + ".ambient").c_str(), vec3(0));
		shader->SetUniform((mStandardUniformBeginning + ".direction").c_str(), vec3(0));
		shader->SetUniform((mStandardUniformBeginning + ".diffuse").c_str(), vec3(0));
	}
}
