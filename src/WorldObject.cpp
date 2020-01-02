#include "WorldObject.h"

void WorldObject::setPosition(glm::vec3 loc) {
	mPosition = loc;
}

glm::vec3 WorldObject::getPosition() {
	return mPosition;
}
