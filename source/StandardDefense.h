// take damage upon damage request (maybe have armor)

#ifndef __STANDARD_DEFENSE_H__
#define __STANDARD_DEFENSE_H__

#include "DefendCollision.h"
#include "Constants.h"

class StandardDefense : public DefendCollision {
protected:
	float _killSpeed = KILL_SPEED;
public:
	virtual void init(float killSpeed) { _killSpeed = killSpeed; }
	virtual bool onAttacked(void *obj, int damage, cugl::Vec2 velocity, bool shieldBreaker) override;
};


#endif