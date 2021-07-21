/*
* Common physics methods that will be used in most/every scene.
* Singleton or set of functions? Should this be in PhysicalObject or is this bonus physics?
*/

#ifndef __SPLINE_PHYSICS_H__
#define __SPLINE_PHYSICS_H__

#include "cugl/cugl.h"
#include "Nurbs.h"
#include "PhysicalObject.h"
#include "Movement.h"
#include "Collision.h"

// really sprite physics
class SplinePhysics : public Movement, public Collision {
protected:
	typedef struct resp {
		cugl::Timestamp startTime;
		cugl::Timestamp lastTimeSampled;
		cugl::Vec2 direction;
		float speedMod;
		int splineIdx;
	} resp_t;


	float _scale;
	float _physVertShift;
	float _recoilMod;
	float _bounceMod;
	//std::shared_ptr<Nurbs> _respCurve; //might want to change this depending on character?
	std::vector<std::shared_ptr<Nurbs>> _respCurves;
	std::vector<std::shared_ptr<resp_t>> _respStartTimes;

	// save collision normals and reflect around avg
	//std::vector<cugl::Vec2> _frameNormals;



public:
	SplinePhysics(float scale, float physVertShift, float recoilMod, float bounceMod, bool beserker);

	// TODO: add alloc 

	virtual void init(float scale, float physVertShift, float recoilMod, float bounceMod, bool beserker);

	~SplinePhysics() { dispose(); }

	void dispose();

	void recoil(cugl::Vec2 touchLoc, std::shared_ptr<PhysicalObject> obj, cugl::Vec2 *dispVec);
	//void projectileSetup(PhysicalObject *creator, PhysicalObject *proj, cugl::Vec2 dispVec);
	//int getOrientation(cugl::Vec2 dispVec, int numOrientations);

	void createSpline();
	//void recoilSpline(cugl::Vec2 touchLoc, std::shared_ptr<PhysicalObject> obj, cugl::Vec2 *dispVec);
	void recoilSpline(cugl::Vec2 touchLoc, PhysicalObject *obj);
	//void applyResps(std::shared_ptr<PhysicalObject> obj);
	void applyResps(PhysicalObject *obj);
	void reflectResps(PhysicalObject *obj, cugl::Vec2 normalVec);
	float sampleAverage(std::vector<float> &samples);

	void adjustRespSpeed(cugl::Vec2 direction, float speedMult);

	//void splineMovement(bool isNewResp, cugl::Vec2 direction, PhysicalObject * obj, cugl::Vec2 *dispVec);
	//void splineMovementFromTouch(bool isTouch, cugl::Vec2 touchLoc, PhysicalObject * obj, cugl::Vec2 *dispVec);
	cugl::Vec2 getDirectionFromTouch(cugl::Vec2 touchLoc, cugl::Vec2 objPos);

	virtual void move(void *obj, bool isNewResp, cugl::Vec2 direction, cugl::Vec2 *dispVec) override;
	virtual void adjustSpeed(void *obj, float speedMult) override;
	virtual void onCollide(void *obj, cugl::Vec2 normalVec) override;
	virtual bool doCollide(void *obj, void *otherPhysObj) override;
	//virtual void procAllCollisions(void *obj) override;
};




#endif