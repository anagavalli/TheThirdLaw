// handles projecitle wapon functionality


#ifndef __PROJECTILE_ATTACK_H__
#define __PROJECTILE_ATTACK_H__

#include "PhysicalObject.h"
#include "cugl/cugl.h"
#include "Attack.h"
#include "Movement.h"
#include "Collision.h"
#include "LogicCollision.h"


class ProjectileAttack : public Attack {
private:
	std::vector<attackInfo_t> _info;
	
	float _projectileSpeed;
	int _attackRateMS; // MS
	cugl::Timestamp _lastAttackTime;
	float _distToEdge;

public:

	ProjectileAttack(float projSpeed, int attackRate, float distToEdge);
	~ProjectileAttack() { dispose(); }

	void dispose();

	virtual const attackInfo *attack(void *obj, bool attackReq, cugl::Vec2 attackDir, int *numAttacks) override;

	void appendProjectile(int damage, std::shared_ptr<Movement> moveDel, std::shared_ptr<Collision> colDel,
		std::shared_ptr<LogicCollision> logicColDel, cugl::Vec2 scale, int health, int aoeType, std::string texKey, 
		float offsetAlongNormal, float attackAngleOffset);
	cugl::Vec2 projectilePlacement(PhysicalObject *creator, cugl::Vec2 attackDirection, float offsetAlongNormal, float offsetAngle);

	//static void projectileSetup(PhysicalObject *creator, PhysicalObject *proj, cugl::Vec2 dispVec);
	Uint64 getElapsedTime() override;
};


#endif

