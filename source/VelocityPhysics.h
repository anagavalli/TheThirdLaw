/*
* Move by specifying velocity
*/

#ifndef __VELOCITY_PHYSICS_H__
#define __VELOCITY_PHYSICS_H__

#include "Movement.h"
#include "Collision.h"

class VelocityPhysics : public Movement, public Collision {
protected:
	float _moveSpeed = 1.0f;
public:
	virtual void init(float moveSpeed);
	virtual void move(void *obj, bool isNewResp, cugl::Vec2 direction, cugl::Vec2 *dispVec) override;
	virtual void adjustSpeed(void *obj, float speedMult) override { return; }
	//virtual void onCollide(void *obj, cugl::Vec2 normalVec) override { return; }
	virtual void onCollide(void *obj, cugl::Vec2 normalVec) override;
	virtual bool doCollide(void *obj, void *otherObj) override { return true; }
};


#endif
