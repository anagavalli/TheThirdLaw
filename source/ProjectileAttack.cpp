#include "ProjectileAttack.h"
#include "Constants.h"

ProjectileAttack::ProjectileAttack(float projSpeed, int attackRateMS, float distToEdge) {
	/*_info.isProjectile = true;
	_info.damage = damage;
	_info.moveDel = moveDel;
	_info.colDel = colDel;
	_info.logicColDel = logicColDel;
	_info.scale = scale;
	_info.health = health;
	_info.aoeType = aoeType;
	_info.validAttack = false;*/

	_projectileSpeed = projSpeed;
	_attackRateMS = attackRateMS;
	_distToEdge = distToEdge;
	_lastAttackTime.mark();
}

void ProjectileAttack::appendProjectile(int damage, std::shared_ptr<Movement> moveDel, std::shared_ptr<Collision> colDel,
	std::shared_ptr<LogicCollision> logicColDel, cugl::Vec2 scale, int health, int aoeType, std::string texKey, 
	float offsetAlongNormal, float attackAngleOffset) {

	attackInfo_t info;
	info.isProjectile = true;
	info.damage = damage;
	info.moveDel = moveDel;
	info.colDel = colDel;
	info.logicColDel = logicColDel;
	info.scale = scale;
	info.health = health;
	info.aoeType = aoeType;
	info.validAttack = false;
	info.texKey = texKey;
	info.offsetAlongNormal = offsetAlongNormal;
	info.offsetAngle = attackAngleOffset;

	_info.push_back(info);
}

void ProjectileAttack::dispose() {
	return;
}

// create projectile for attack
const attackInfo_t *ProjectileAttack::attack(void *creator, bool attackReq, cugl::Vec2 attackDirection, int *numAttacks) {
	std::vector<attackInfo_t> infoVec;

	PhysicalObject *physCreator = (PhysicalObject*)creator;
	
	// check if within fire rate // TODO: couple with character movement!
	// get current time

	for (int i = 0; i < _info.size(); i++) {
		_info[i].owner = creator;

		if (!attackReq) {
			_info[i].validAttack = false;
			*numAttacks = _info.size();
			return _info.data();
		}

		cugl::Timestamp currTime = cugl::Timestamp();
		currTime.mark();

		auto timeSinceLastFire = cugl::Timestamp::ellapsedMillis(_lastAttackTime, currTime);
		if ((int)timeSinceLastFire >= _attackRateMS) {
			_info[i].validAttack = true;
			_lastAttackTime.mark();
		}
		else {
			_info[i].validAttack = false;
		}

		_info[i].pos = projectilePlacement(physCreator, attackDirection, _info[i].offsetAlongNormal, _info[i].offsetAngle);

		cugl::Quaternion rot = cugl::Quaternion();
		rot.set(cugl::Vec3(0.0f, 0.0f, 1.0f), _info[i].offsetAngle);
		cugl::Vec3 attackDir3D = rot.getRotation(cugl::Vec3(attackDirection));
		cugl::Vec2 attackDir = Vec2(attackDir3D.x, attackDir3D.y);
		attackDir.normalize();
		//CULog("%f : %f %f -> %f %f \n", offsetAngle, attackDirection, attakDir);

		_info[i].velocity = attackDir * _projectileSpeed * -1.0f;

		

	}
	*numAttacks = _info.size();
	return _info.data();
}

// TODO better hitbox detection
cugl::Vec2 ProjectileAttack::projectilePlacement(PhysicalObject *creator, cugl::Vec2 attackDirection, float offsetAlongNormal, float offsetAngle) {

	// start at edge of character with initial velocity
	// get angle of shot (dispVec) use trig to find where edge of hit box is
	// punt for now
	//int angle = int(attackDirection.getAngle() * 180.0 / 3.1454); //-pi - pi radians
	//float distToEdge = 1.25f * 1.5f; //2.0f * sinf(angle % 4) * hitBox.y; //TODO: BAD

							  /**projPos = creat;
							  *projPos += -1.0f * distToEdge * dispVec;
							  *projVel = dispVec * -3.0f;*/

	//float distToEdge = 

	// rotate about direction vec to get normal
	cugl::Quaternion rot = cugl::Quaternion();
	rot.set(cugl::Vec3(0.0f, 0.0f, 1.0f), PI / 2); // rotate into z-axis by 90 degrees around normal vec
	cugl::Vec3 normal3D = rot.getRotation(cugl::Vec3(attackDirection));
	cugl::Vec2 normal = Vec2(normal3D.x, normal3D.y);
	normal.normalize();

	cugl::Vec2 offset = normal * offsetAlongNormal;

	
	cugl::Vec2 projPos = creator->getPosition() + offset - (_distToEdge * attackDirection);

	return projPos;
	//proj->setPosition(projPos);
	//proj->setLinearVelocity(dispVec * -3.0f);
}


//void ProjectileAttack::projectileSetup(PhysicalObject *creator, PhysicalObject *proj, cugl::Vec2 attackDirection) {
//
//	// start at edge of character with initial velocity
//	// get angle of shot (dispVec) use trig to find where edge of hit box is
//	// punt for now
//	int angle = int(attackDirection.getAngle() * 180.0 / 3.1454); //-pi - pi radians
//	float distToEdge = 1.25f; //2.0f * sinf(angle % 4) * hitBox.y; //TODO: BAD
//
//							  /**projPos = creat;
//							  *projPos += -1.0f * distToEdge * dispVec;
//							  *projVel = dispVec * -3.0f;*/
//
//	cugl::Vec2 projPos = creator->getPosition() - (distToEdge * attackDirection);
//	proj->setPosition(projPos);
//	proj->setLinearVelocity(attackDirection * -3.0f);
//}

Uint64 ProjectileAttack::getElapsedTime() {
	cugl::Timestamp currTime = cugl::Timestamp();
	currTime.mark();

	auto timeSinceLastFire = cugl::Timestamp::ellapsedMillis(_lastAttackTime, currTime);
	return timeSinceLastFire;
}