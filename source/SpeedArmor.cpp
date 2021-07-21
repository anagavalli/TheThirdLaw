
#include "SpeedArmor.h"
#include "PhysicalObject.h"


bool SpeedArmor::onAttacked(void *obj, int damage, cugl::Vec2 velocity, bool shieldBreaker) {
	PhysicalObject *physObj = (PhysicalObject*)obj;

	
	if (shieldBreaker && velocity.length() > _killSpeed) {// needed?
		physObj->addHealthLostThisFrame(physObj->getCurrentHealth());
		physObj->kill();
	}
	else {
		int currHealth = physObj->getCurrentHealth();

		int actualDamage = damage;

		// reduce damage if at kill speed
		if (physObj->getVelocity().length() > _killSpeed) {
			actualDamage /= _damageReduction;
		}

		physObj->setCurrentHealth(currHealth - actualDamage); // TODO merge?
		physObj->addHealthLostThisFrame(actualDamage);
	}

	return true; // allow more dmg

}
