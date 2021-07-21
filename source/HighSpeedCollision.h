
/* Tries to deal damage upon colliding with an enemy */

#ifndef __HIGH_SPEED_COLLISION_H__
#define __HIGH_SPEED_COLLISION_H__

#include "LogicCollision.h"

class HighSpeedCollision : public LogicCollision {
	void onCollide(void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) override;
};



#endif