/*
* Spline based physics with user touch
*/

#ifndef __TOUCH_SPLINE_PHYSICS_H__
#define __TOUCH_SPLINE_PHYSICS_H__

#include "SplinePhysics.h"

class TouchSplinePhysics : public SplinePhysics {
public:
	TouchSplinePhysics(float scale, float physVertShift, float recoilMod, float bounceMod, float beserker) : SplinePhysics(scale, physVertShift, recoilMod, bounceMod, beserker) {}
	~TouchSplinePhysics() { dispose(); }

	virtual void move(void *obj, bool isNewResp, cugl::Vec2 direction, cugl::Vec2 *dispVec) override;
};



#endif
