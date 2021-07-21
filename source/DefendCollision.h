// responsible for managing defense


#ifndef __DEFEND_COLLISION_H__
#define __DEFEND_COLLISION_H__

#include "cugl/cugl.h"

class DefendCollision {
public:
	virtual bool onAttacked(void *obj, int damage, cugl::Vec2 velocity, bool shieldBreaker) { return true; }
};



#endif
