
#include "ShieldDefense.h"
#include "PhysicalObject.h"


bool ShieldDefense::onAttacked(void *obj, int damage, cugl::Vec2 velocity, bool shieldBreaker) {
	PhysicalObject *physObj = (PhysicalObject*)obj;


	if (shieldBreaker && velocity.length() > _killSpeed) {
		physObj->deactivateLastHitbox();
	}

	return false; // do not allow any more damage

}