
#include "TouchSplinePhysics.h"

/*TouchSplinePhysics::TouchSplinePhysics(float scale, float physVertShift, float recoilMod) {
	init(scale, physVertShift, recoilMod);
}*/

void TouchSplinePhysics::move(void *obj, bool isTouch, cugl::Vec2 touchLoc, cugl::Vec2 *dispVec) {
	PhysicalObject *physObj = (PhysicalObject*)obj;
	
	if (isTouch) {
		*dispVec = getDirectionFromTouch(touchLoc, physObj->getPosition());
		recoilSpline(*dispVec, physObj);
	}

	applyResps(physObj);
}