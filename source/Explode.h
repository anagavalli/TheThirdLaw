/*
* Implements what happens on collision besides physics
*/

#ifndef __EXPLODE_H__
#define __EXPLODE_H__

#include "LogicCollision.h"

class Explode : public LogicCollision {
protected:
	bool _doShieldBreak = false;
	bool _excludeEnemy = false;
public:
	virtual void init(bool doBreak, bool excludeEnemy) { _doShieldBreak = doBreak; _excludeEnemy = excludeEnemy; }
	virtual void onCollide(void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) override;
	bool doCollide(void *obj, void *otherObj) override;
};




#endif