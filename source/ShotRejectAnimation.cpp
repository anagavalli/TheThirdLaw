
#include "ShotRejectAnimation.h"
#include "PhysicalObject.h"
#include "Constants.h"

void ShotRejectAnimation::init(std::string animKey) {
	_animKey = animKey;
}

void ShotRejectAnimation::updateTexture(anim_args_t *args) {
	PhysicalObject *physObj = (PhysicalObject*)args->obj;

	std::shared_ptr<AnimationNode> exclaimNode = args->nodes[0];
	std::shared_ptr<ActionManager> actions = args->actions;



	if (physObj->getRejectionThisFrame() && !actions->isActive(_animKey.c_str())) {
		exclaimNode->setVisible(true);
		int numCols = exclaimNode->getCols() - 1;
		std::shared_ptr<Animate> exclaimAction = Animate::alloc(0, numCols, 0.5f);
		actions->activate(_animKey.c_str(), exclaimAction, exclaimNode);
	}
	else if (!actions->isActive(_animKey.c_str())) {
		exclaimNode->setVisible(false);
	}

}



void ShotRejectAnimation::stopAnim() {
	return;
}