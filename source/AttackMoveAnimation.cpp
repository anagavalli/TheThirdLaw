#include "AttackMoveAnimation.h"
#include "cugl/cugl.h"
#include "Constants.h"

using namespace cugl;

void AttackMoveAnimation::init(std::string move, std::string shoot) {
	_moveAnimKey = move;
	_shootAnimKey = shoot;
}

void AttackMoveAnimation::stopAnim() {

}

void AttackMoveAnimation::updateTexture(anim_args_t *args) {
	std::shared_ptr<AnimationNode> newtonMoveNode = args->nodes[0];
	std::shared_ptr<AnimationNode> newtonShootNode = args->nodes[1];

	std::shared_ptr<ActionManager> actions = args->actions;
	animState state;
	if (args->newAttack)
		state = animState::SHOOT;
	else
		state = animState::MOVE;


	if (state == animState::SHOOT) {
		if (args->newAttack) {
			newtonMoveNode->setVisible(false);
			newtonShootNode->setVisible(true);
			if (actions->isActive(_moveAnimKey.c_str())) {
				actions->remove(_moveAnimKey.c_str());
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
	else {
		if (!actions->isActive(_shootAnimKey.c_str())) {
			newtonShootNode->setVisible(false);
			newtonMoveNode->setVisible(true);
			if (!actions->isActive(_moveAnimKey.c_str())) {
				int numCols = newtonMoveNode->getCols();
				int start = numCols * args->orientation;
				int range = numCols - 1;
				int end = start + range;

				std::shared_ptr<Animate> idleAction = Animate::alloc(start, end, 1.0f);

				actions->activate(_moveAnimKey.c_str(), idleAction, newtonMoveNode);
			}
		}
	}
}
