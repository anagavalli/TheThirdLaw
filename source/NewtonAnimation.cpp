#include "NewtonAnimation.h"
#include "cugl/cugl.h"
#include "Constants.h"
#include "PhysicalObject.h"

using namespace cugl;

void NewtonAnimation::init(std::string idle, std::string shoot, std::string death, std::string victory) {
	_idleAnimKey = idle;
	_shootAnimKey = shoot;
	_deathAnimKey = death;
	_victoryAnimKey = victory;
	_startedDeath = false;
	_numVictoryPumps = 0;
}

void NewtonAnimation::stopAnim() {

}

void NewtonAnimation::updateTexture(anim_args_t *args) {
	std::shared_ptr<AnimationNode> newtonIdleNode = args->nodes[0];
	std::shared_ptr<AnimationNode> newtonShootNode = args->nodes[1];
	std::shared_ptr<AnimationNode> newtonDeathNode = args->nodes[2];

	PhysicalObject *newton = (PhysicalObject*)args->obj;
	
	std::shared_ptr<ActionManager> actions = args->actions;
	animState state;
	if (args->levelExit)
		state = animState::VICTORY;
	else if (newton->isDead())
		state = animState::DEATH;
	else if (args->timeSinceLastAction >= IDLE_DURATION_MS)
		state = animState::IDLE;
	else
		state = animState::SHOOT;


	if (state == animState::SHOOT) {
		if (args->newAttack) {
			newtonIdleNode->setVisible(false);
			newtonShootNode->setVisible(true);
			newtonDeathNode->setVisible(false); // don't worry about removing because will reset after
			if (actions->isActive(_idleAnimKey.c_str())) {
				actions->remove(_idleAnimKey.c_str());
			}
			if (actions->isActive(_shootAnimKey.c_str()))
				actions->remove(_shootAnimKey.c_str());

			int numCols = newtonShootNode->getCols();
			int start = numCols * args->orientation;
			int range = numCols - 1;
			int end = start + range;

			std::shared_ptr<Animate> shootAction = Animate::alloc(start, end, 1.0f);

			actions->activate(_shootAnimKey.c_str(), shootAction, newtonShootNode);
		}
	}
	else if (state == animState::IDLE){
		if (!actions->isActive(_shootAnimKey.c_str())) {
			newtonShootNode->setVisible(false);
			newtonIdleNode->setVisible(true);
			newtonDeathNode->setVisible(false);
			if (!actions->isActive(_idleAnimKey.c_str())) {
				int numCols = newtonIdleNode->getCols();
				int start = numCols * args->orientation;
				int range = numCols - 1;
				int end = start + range;

				std::shared_ptr<Animate> idleAction = Animate::alloc(start, end, 1.0f);

				actions->activate(_idleAnimKey.c_str(), idleAction, newtonIdleNode);
			}
		}
	}
	else if (state == animState::DEATH) {
		if (!actions->isActive(_deathAnimKey.c_str())) {
			if (_startedDeath) { // okay to kill because we have already played death animation
				newton->setPermissionToDie(true);
			}

			newtonShootNode->setVisible(false);
			newtonIdleNode->setVisible(false);
			newtonDeathNode->setVisible(true);

			// remove all other actions
			if (actions->isActive(_idleAnimKey.c_str())) {
				actions->remove(_idleAnimKey.c_str());
			}
			if (actions->isActive(_shootAnimKey.c_str())) {
				actions->remove(_shootAnimKey.c_str());
			}

			int numCols = newtonDeathNode->getCols();
			int start = numCols * args->orientation;
			int range = numCols - 1;
			int end = start + range;

			std::shared_ptr<Animate> deathAction = Animate::alloc(start, end, 1.0f);

			actions->activate(_deathAnimKey.c_str(), deathAction, newtonDeathNode);

			_startedDeath = true;
		}
	}
	else if (state == animState::VICTORY) {
		if (!actions->isActive(_victoryAnimKey.c_str())) {
			if (_numVictoryPumps >= VICTORY_PUMPS) { // okay to exit level because we have already played the win animation
				newton->setPermissionToMoveOn(true);
			}

			newtonShootNode->setVisible(true); // just reuse shoot node for now
			newtonIdleNode->setVisible(false);
			newtonDeathNode->setVisible(false);

			// remove all other actions
			if (actions->isActive(_idleAnimKey.c_str())) {
				actions->remove(_idleAnimKey.c_str());
			}
			if (actions->isActive(_shootAnimKey.c_str())) {
				actions->remove(_shootAnimKey.c_str());
			}
			if (actions->isActive(_deathAnimKey.c_str())) {
				actions->remove(_deathAnimKey.c_str());
			}

			int numCols = newtonShootNode->getCols();
			int start = numCols * args->orientation;
			int range = numCols - 1;
			int end = start + range;

			std::shared_ptr<Animate> victoryAction = Animate::alloc(start, end, 0.4f);

			actions->activate(_victoryAnimKey.c_str(), victoryAction, newtonShootNode);

			_numVictoryPumps++;
		}
	}
}
