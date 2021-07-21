#include "NewtonDamageAnimation.h"
#include "PhysicalObject.h"
#include "Constants.h"

void NewtonDamageAnimation::init(std::string animKey) {
	_animKey = animKey;
	_animState = 0;
}

void NewtonDamageAnimation::updateTexture(anim_args_t *args) {
	PhysicalObject *physObj = (PhysicalObject*)args->obj;

	std::shared_ptr<AnimationNode> flashNode = args->nodes[0];
	std::shared_ptr<ActionManager> actions = args->actions;

	if (physObj->getHealthLostThisFrame() > 0) {
		if (_animState != 0) {
			actions->remove(_animKey.c_str());
		}

		flashNode->setVisible(true);

		auto nodeColor = flashNode->getColor();
		nodeColor.a = 0.0f;
		flashNode->setColor(nodeColor);
		//flashNode->setBlendFunc(GL_CONSTANT_COLOR, GL_ONE);
		//flashNode->setBlendEquation(GL_FUNC_ADD);
		//flashNode->set

		std::shared_ptr<FadeIn> fadeInAction = FadeIn::alloc(0.2f);

		actions->activate(_animKey.c_str(), fadeInAction, flashNode);
		_animState = 1;
	}
	else {
		if (_animState == 0 && flashNode->isVisible())
			flashNode->setVisible(false);
		if (_animState != 0 && !actions->isActive(_animKey.c_str())) {
			if (_animState == 1) {
				std::shared_ptr<FadeOut> fadeOutAction = FadeOut::alloc(0.2f);

				actions->activate(_animKey.c_str(), fadeOutAction, flashNode);
				_animState = 2;
			}
			else if (_animState == 2) {
				flashNode->setVisible(false);

				_animState = 0;
			}
		}
	}

}



void NewtonDamageAnimation::stopAnim() {
	return;
}