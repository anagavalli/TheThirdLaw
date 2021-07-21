
#include "Explode.h"
#include "PhysicalObject.h"

// obj will deal damage to otherObj
// also obj currHealth becomes zero
void Explode::onCollide(void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) {
	
	
	PhysicalObject *physObj = (PhysicalObject*)obj;
	PhysicalObject *otherPhysObj = (PhysicalObject*)otherObj;

	// no physics collision means no explosion unless character can pass through due to beserker
	if (noPhysCollision && !otherPhysObj->isBeserker()) return;

	int damage = physObj->getDamage();

	if (_excludeEnemy && !otherPhysObj->getIsEnemy() || !_excludeEnemy) {
		otherPhysObj->onAttacked(damage, physObj->getVelocity(), _doShieldBreak);

		/*int currHealth = otherPhysObj->getCurrentHealth();
		otherPhysObj->setCurrentHealth(currHealth - damage);*/

		physObj->kill();
	}
}

// can reject physics collision from here
bool Explode::doCollide(void *obj, void *otherObj) {
	PhysicalObject *otherPhysObj = (PhysicalObject*)otherObj;

	if (otherPhysObj == nullptr) return true;

	if (_excludeEnemy && !otherPhysObj->getIsEnemy() || !_excludeEnemy) {
		return true;
	}
	else {
		return false;
	}
}