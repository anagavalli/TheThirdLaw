/*
* Implements what happens on collision besides physics
*/

#ifndef __DOT_COLLISION_H__
#define __DOT_COLLISION_H__

#include "cugl/cugl.h"
#include "LogicCollision.h"

class DOTCollision : public LogicCollision {
protected:
	cugl::Timestamp _lastDamageTime;
	int _numMs;

public:
	void init(int numMs);
	virtual void onCollide(void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) override;
};




#endif
