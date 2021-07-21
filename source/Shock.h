/*
* Implements what happens on collision besides physics
*/

#ifndef __SHOCK_H__
#define __SHOCK_H__

#include "LogicCollision.h"

class Shock : public LogicCollision {
protected:
	bool _doShieldBreak = false;
	bool _onlyPlayer = false;
public:
	virtual void init(bool doBreak, bool onlyPlayer) { _doShieldBreak = doBreak; _onlyPlayer = onlyPlayer; }
	virtual void onCollide(void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) override;
};




#endif
