/*
* Take less damage from bullets when going fast (KILL_SPEED).
* Encourages high speed play.
*/


#ifndef __SPEED_ARMOR_H__
#define __SPEED_ARMOR_H__

#include "DefendCollision.h"
#include "Constants.h"

class SpeedArmor : public DefendCollision {
protected:
	float _killSpeed = KILL_SPEED;
	int _damageReduction = 2;
public:
	virtual void init(float killSpeed) { _killSpeed = killSpeed; }
	virtual bool onAttacked(void *obj, int damage, cugl::Vec2 velocity, bool shieldBreaker) override;
};


#endif