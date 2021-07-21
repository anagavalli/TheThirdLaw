
#include "ExclaimAnimation.h"
#include "PhysicalObject.h"
#include "Constants.h"

void ExclaimAnimation::init(std::string animKey) {
	_animKey = animKey;
}

void ExclaimAnimation::updateTexture(anim_args_t *args) {
	PhysicalObject *physObj = (PhysicalObject*)args->obj;

	std::shared_ptr<AnimationNode> exclaimNode = args->nodes[0];
	std::shared_ptr<ActionManager> actions = args->actions;

	cugl::Vec2 velocity = physObj->getVelocity();
	float speedMag = velocity.length();


	if (speedMag > KILL_SPEED) {
		exclaimNode->setVisible(true);

		if (!actions->isActive(_animKey.c_str())) {
			int numCols = exclaimNode->getCols() - 1;
			std::shared_ptr<Animate> exclaimAction = Animate::alloc(0, numCols, 0.35f);
			actions->activate(_animKey.c_str(), exclaimAction, exclaimNode);
		}
	}
	else {
		//exclaimNode->setVisible(false);

		// remove animation
		if (actions->isActive(_animKey.c_str())) {
			actions->remove(_animKey.c_str());
		}

		exclaimNode->setFrame(0);
	}
}



void ExclaimAnimation::stopAnim() {
	return;
}