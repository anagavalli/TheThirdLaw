#include "DOTCollision.h"
#include "PhysicalObject.h"


void DOTCollision::init(int numMs) {
	_lastDamageTime.mark();
	_numMs = numMs;
}

// obj will deal damage to otherObj
// also obj currHealth becomes zero
void DOTCollision::onCollide(void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) {
	PhysicalObject *otherPhysObj = (PhysicalObject*)otherObj;

	if (otherPhysObj->isPlayer()) {
		otherPhysObj->kill();
	}
	
	/*cugl::Timestamp currTime;
	currTime.mark();

	auto timeSinceLastFire = cugl::Timestamp::ellapsedMillis(_lastDamageTime, currTime);


	if (timeSinceLastFire > (uint64_t)_numMs) {

		PhysicalObject *physObj = (PhysicalObject*)obj;
		PhysicalObject *otherPhysObj = (PhysicalObject*)otherObj;

		int damage = physObj->getDamage();

		int currHealth = otherPhysObj->getCurrentHealth();
		otherPhysObj->setCurrentHealth(currHealth - damage);

		_lastDamageTime.mark();
	}*/

}