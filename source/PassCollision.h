/*
* Cancel collision (i.e. no reflections should happen)
*/

#ifndef __PASS_COLLISION_H__
#define __PASS_COLLISION_H__

#include "Collision.h"

class PassCollision : public Collision {
protected:
	bool _passBullets = true;
	bool _passPlayer = true;
public:
	void init(bool passBullets, bool passPlayer) { _passBullets = passBullets; _passPlayer = passPlayer; }
	virtual void onCollide(void *obj, cugl::Vec2 normalVec) override { return; }
	virtual bool doCollide(void *obj, void *otherObj) override;
};



#endif
