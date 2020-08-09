#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H
#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include "ControllerDefinitions.h"
#include <iostream>


class WorldObject
{
public:
	virtual void setPosition(glm::vec3 loc);
	virtual glm::vec3 getPosition();
	virtual void setLayer(Layer layer);
	virtual Layer getLayer() { return objectLayer; };

protected:
	glm::vec3 mPosition = glm::vec3(0.0f);
	Layer objectLayer = VISIBLE;
};

#endif

