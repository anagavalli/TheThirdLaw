/*
* Implement melee attacks
*/


#ifndef __MELEE_ATTACK_H__
#define __MELEE_ATTACK_H__

#include "cugl/cugl.h"
#include "Attack.h"

class MeleeAttack : public Attack {
public:
	virtual const attackInfo *attack(void *obj, bool attackReq, cugl::Vec2 attackDir, int *numAttack) override;

};



#endif