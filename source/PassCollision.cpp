
#include "PassCollision.h"
#include "PhysicalObject.h"

bool PassCollision::doCollide(void *obj, void *otherObj) {
	PhysicalObject *otherPhysObj = (PhysicalObject*)otherObj;

	if (!_passBullets && otherPhysObj->getBody(0)->isBullet()) {
		return true;
	}

	if (!_passPlayer && otherPhysObj->isPlayer()) {
		return true;
	}

	return false;
}