#ifndef __LOCK_GOAL_H__
#define __LOCK_GOAL_H__

#include "GoalCollision.h"
#include "ObjectContainer.h"
#include "PhysicalObject.h"

class LockGoal : public GoalCollision {
protected:
	ObjectContainer *_objContainer;
	bool enemiesRem;
	PhysicalObject *_goalZone;

public:
	void init(ObjectContainer *objCont, PhysicalObject *goalZone);

	virtual void onCollide(void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) override;

	virtual void updateGoal() override;
};

#endif
