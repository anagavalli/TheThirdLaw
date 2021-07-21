
#include "StandardDefense.h"
#include "PhysicalObject.h"


bool StandardDefense::onAttacked(void *obj, int damage, cugl::Vec2 velocity, bool shieldBreaker) {
	PhysicalObject *physObj = (PhysicalObject*)obj;


	if (shieldBreaker && velocity.length() > _killSpeed) {
		physObj->addHealthLostThisFrame(physObj->getCurrentHealth());
		physObj->kill();
	}
	else {
		int currHealth = physObj->getCurrentHealth();
		physObj->setCurrentHealth(currHealth - damage); // TODO merge?
		physObj->addHealthLostThisFrame(damage);
	}

	return true; // allow more dmg
	
}
