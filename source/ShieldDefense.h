/*
* Only susceptible to shieldbreak with speed
*/

#ifndef __SHIELD_DEFENSE_H__
#define __SHIELD_DEFENSE_H__

#include "DefendCollision.h"
#include "Constants.h"

class ShieldDefense : public DefendCollision {
protected:
	float _killSpeed = KILL_SPEED;
public:
	virtual void init(float killSpeed) { _killSpeed = killSpeed; }
	virtual bool onAttacked(void *obj, int damage, cugl::Vec2 velocity, bool shieldBreaker) override;
};

#endif