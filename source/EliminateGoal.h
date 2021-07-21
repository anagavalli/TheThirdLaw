#ifndef __ELIMINATE_GOAL_H__
#define __ELIMINATE_GOAL_H__

#include "GoalCollision.h"
#include "CharacterModel.h"

class EliminateGoal : public GoalCollision {
private:
	std::shared_ptr<CharacterModel> _target;
public:
	void init(std::shared_ptr<CharacterModel> target);
	virtual void updateGoal() override;
};


#endif