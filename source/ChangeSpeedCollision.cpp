#include "ChangeSpeedCollision.h"
#include "PhysicalObject.h"

void ChangeSpeedCollision::init(float speedMult) {
	_speedMult = speedMult;
}

void ChangeSpeedCollision::onCollide(void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) {
	PhysicalObject *physObj = (PhysicalObject*)obj;
	PhysicalObject *otherPhysObj = (PhysicalObject*)otherObj;
	
	//CULog("try adjust speed\n");

	if (!(physObj == otherPhysObj->getLastZone())) {
		//CULog("adjust speed\n");
		otherPhysObj->adjustSpeed(_speedMult);
		otherPhysObj->setLastZone(physObj);
	}

}