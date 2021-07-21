#include "VelocityPhysics.h"
#include "PhysicalObject.h"
#include "Constants.h"

void VelocityPhysics::init(float moveSpeed) {
	_moveSpeed = moveSpeed;
}

void VelocityPhysics::move(void *obj, bool isNewResp, cugl::Vec2 direction, cugl::Vec2 *dispVec) {
	PhysicalObject *physObj = (PhysicalObject *)obj;

	//physObj->setForce(direction * _moveSpeed);
	//physObj->applyForce();

	physObj->setLinearVelocity(direction * _moveSpeed);

	*dispVec = -1.0f * direction; // TODO: why is -1 needed to be consitent with recoil?
}

void VelocityPhysics::onCollide(void *obj, cugl::Vec2 normalVec) {
	PhysicalObject *physObj = (PhysicalObject*)obj;
	cugl::Vec2 normalizedNormal = normalVec.getNormalization();
	cugl::Vec2 flipVelocity = physObj->getVelocity() * -1.0f;

	// rotate about collision
	cugl::Quaternion rot = cugl::Quaternion();

	rot.set(cugl::Vec3(normalizedNormal), PI); // rotate into z-axis by 180 degrees around normal vec
	cugl::Vec3 newRespVel3d = rot.getRotation(cugl::Vec3(flipVelocity));

	cugl::Vec2 newRespVel = Vec2(newRespVel3d.x, newRespVel3d.y);
	physObj->setLinearVelocity(newRespVel);
}