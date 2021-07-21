/*
* Interface for attacks
*/


#ifndef __ATTACK_H__
#define __ATTACK_H__

#include "cugl/cugl.h"
#include "Movement.h"
#include "Collision.h"
#include "LogicCollision.h"


typedef struct attackInfo {
    bool isProjectile;
    int damage;
    std::shared_ptr<Movement> moveDel;
    std::shared_ptr<Collision> colDel;
    std::shared_ptr<LogicCollision> logicColDel;
    cugl::Vec2 scale;
    int health;
    int aoeType;
    bool validAttack;
    cugl::Vec2 pos;
    cugl::Vec2 velocity;
    void *owner;
	std::string texKey;
	float offsetAlongNormal;
	float offsetAngle;
} attackInfo_t;



class Attack {
public:
	virtual const attackInfo *attack(void *obj, bool attackReq, cugl::Vec2 attackDir, int *numAttacks) {
		/*attackInfo_t info;
		info.validAttack = false;
		return info;*/
		*numAttacks = 0;
		return nullptr;
	}
	virtual Uint64 getElapsedTime() { return 0; }
};


#endif
