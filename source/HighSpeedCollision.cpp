
#include "HighSpeedCollision.h"
#include "PhysicalObject.h"


void HighSpeedCollision::onCollide(void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) {
	PhysicalObject *physObj = (PhysicalObject*)obj;
	PhysicalObject *otherPhysObj = (PhysicalObject*)otherObj;

	cugl::Vec2 velocity = physObj->getVelocity();
	otherPhysObj->onAttacked(0, velocity, true);
}
