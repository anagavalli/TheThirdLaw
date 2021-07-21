/** Class for working with projectiles (BOOM) 
*	Inherits most scene graph + box2d interactions from PhysicalObject
*/

#include "Projectile.h"

// custom init functionality for sub classes (virtual to allow for overriding)
bool Projectile::init(const cugl::Vec2& pos, const cugl::Size& size) {
	//call parent constructor
	PhysicalObject::init(pos, size);


	// use raycast collision detection b/c going fast?
	// maybe check what speed we decide to use (requires more cycles if do use)
	setBullet(true);
	isDestructable = true;
	_totalHealth = 1;
	_currentHealth = _totalHealth;

	return true;
}

bool Projectile::init(const attackInfo_t attackInfo) {
	init(attackInfo.pos, attackInfo.scale);

	_movementDelegate = attackInfo.moveDel;
	_physCollisionDelegate = attackInfo.colDel;
	_logicCollisionDelegate = attackInfo.logicColDel;

	setLinearVelocity(attackInfo.velocity);
	setDamage(attackInfo.damage);
	setTotalHealth(attackInfo.health);
	setCurrentHealth(attackInfo.health);

	return true;
}

void Projectile::dispose() {
	PhysicalObject::dispose();
}

// slot in behavior (maybe lambda struct?)