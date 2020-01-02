#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H
#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include <iostream>


class WorldObject
{
public:
	virtual void setPosition(glm::vec3 loc);
	virtual glm::vec3 getPosition();

protected:
	glm::vec3 mPosition = glm::vec3(0.0f);
};

#endif

