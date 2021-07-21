/*
* Delegate for handling on non-physics logic that happens on collision
*/



#ifndef __LOGIC_COLLISION_H__
#define __LOGIC_COLLISION_H__

#include "cugl/cugl.h"

class LogicCollision {
public:
	virtual void onCollide(void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) { return; }
	virtual bool doCollide(void *obj, void *otherObj) { return true; }
};

#endif
