/*
* Delegate for collison handling
* Return - whether a collision should happen or not.
*/


#ifndef __COLLISION_H__
#define __COLLISION_H__

#include "cugl/cugl.h"

class Collision {
protected:
	bool _isBeserker = false;  // when going at kill speed, will not bounce off bullets but still take damage
public:
	virtual void onCollide(void *obj, cugl::Vec2 normalVec) { return; }
	virtual bool doCollide(void *obj, void *otherPhysObj) { return true; }
	bool IsBerserker() const { return _isBeserker; }
	//virtual void procAllCollisions(void *obj) {}

};


#endif
