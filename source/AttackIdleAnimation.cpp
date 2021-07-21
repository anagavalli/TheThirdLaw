#include "AttackIdleAnimation.h"
#include "cugl/cugl.h"
#include "Constants.h"

using namespace cugl;

void AttackIdleAnimation::init(std::string idle, std::string shoot) {
	_idleAnimKey = idle;
	_shootAnimKey = shoot;
}

void AttackIdleAnimation::stopAnim() {

}

void AttackIdleAnimation::updateTexture(anim_args_t *args) {
	/*std::shared_ptr<AnimationNode> newtonIdleNode = nodes[0];
	std::shared_ptr<AnimationNode> newtonIdleCross = nodes[1];
	std::shared_ptr<AnimationNode> newtonShootNode = nodes[2];
	std::shared_ptr<AnimationNode> newtonShootCross = nodes[3];*/
	std::shared_ptr<AnimationNode> newtonIdleNode = args->nodes[0];
	std::shared_ptr<AnimationNode> newtonShootNode = args->nodes[1];

	std::shared_ptr<ActionManager> actions = args->actions;
	/*Color4f colorTrans = newtonIdleCross->getColor();
	colorTrans.a = 0.0f;
	newtonIdleCross->setColor(colorTrans);
	newtonShootCross->setColor(colorTrans);

	Color4f colorOpaque = newtonIdleCross->getColor();
	colorTrans.a = 1.0f;*/
	animState state;
	if (args->timeSinceLastAction >= IDLE_DURATION_MS)
		state = animState::IDLE;
	else
		state = animState::SHOOT;


	if (state == animState::SHOOT) {
		if (args->newAttack) {
			newtonIdleNode->setVisible(false);
			newtonShootNode->setVisible(true);
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
	else {
		if (!actions->isActive(_shootAnimKey.c_str())) {
			newtonShootNode->setVisible(false);
			newtonIdleNode->setVisible(true);
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
	/*std::vector<int> animVec;
	for (int i = 0; i <= 11; i++)
	animVec.push_back(12 * orientation + i);

	std::shared_ptr<Animate> action = Animate::alloc(animVec, 1.0f);

	std::vector<int> crossfadeVec;

	for (int i = 1; i <= 11; i++) {
	crossfadeVec.push_back(12 * orientation + i);
	}
	crossfadeVec.push_back(12*orientation);

	std::shared_ptr<Animate> lagAction = Animate::alloc(crossfadeVec, 1.0f);

	if (actions->isActive("newton_1")) {
	CULog("Trying to remove animations");
	actions->remove("newton_1");
	actions->remove("newton_2");
	}

	actions->activate("newton_1", action, nodes[0]);
	actions->activate("newton_2", lagAction, nodes[2]);

	std::shared_ptr<FadeOut> fadeOut = FadeOut::alloc(1.0f / 12.0f);
	std::shared_ptr<FadeIn> fadeIn = FadeIn::alloc(1.0f / 12.0f);

	if (!actions->isActive("fade_1")) {
	Color4f color = nodes[0]->getColor();
	color.a = 1.0f;
	nodes[0]->setColor(color);

	color = nodes[2]->getColor();
	color.a = 0.0f;
	nodes[2]->setColor(color);
	}

	if (actions->isActive("newton_1")) {
	actions->activate("fade_1", fadeOut, nodes[0]);
	actions->activate("fade_2", fadeIn, nodes[2]);
	}*/
}
