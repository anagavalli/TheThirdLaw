#ifndef __GOAL_COLLISION_H__
#define __GOAL_COLLISION_H__

#include "LogicCollision.h"

class GoalCollision : public LogicCollision {
protected:
	bool _complete;
	
public:
	void init();
	bool isComplete() { return _complete; }
	virtual void updateGoal() { return; }
};

#endif