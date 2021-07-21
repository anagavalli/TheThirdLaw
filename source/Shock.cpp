
#include "Shock.h"
#include "PhysicalObject.h"

// obj will deal damage to otherObj
void Shock::onCollide(void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) {
	if (noPhysCollision) return;

	PhysicalObject *physObj = (PhysicalObject*)obj;
	PhysicalObject *otherPhysObj = (PhysicalObject*)otherObj;

	int damage = physObj->getDamage();

	if (_onlyPlayer && otherPhysObj->isPlayer() || !_onlyPlayer) {
		//otherPhysObj->onAttacked(damage, physObj->getVelocity(), _doShieldBreak);

		if (!(physObj == otherPhysObj->getLastZone())) {
			otherPhysObj->onAttacked(damage, physObj->getVelocity(), _doShieldBreak);
			otherPhysObj->setLastZone(physObj);
		}
	}
}