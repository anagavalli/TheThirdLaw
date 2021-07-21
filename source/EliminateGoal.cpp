#include "EliminateGoal.h"

void EliminateGoal::init(std::shared_ptr<CharacterModel> target ) {
	_complete = false;

	_target = target;
}

void EliminateGoal::updateGoal() {
	if (_target->getCurrentHealth() > 0)
		_complete = false;
	else
		_complete = true;
}