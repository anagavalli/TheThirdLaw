#include "OnHitAnimation.h"
#include "cugl/cugl.h"
#include "Constants.h"
#include "PhysicalObject.h"

using namespace cugl;

void OnHitAnimation::init(std::string animKey, int fixedOrientation, float animDuration) {
	_onHitAnimKey = animKey;
	_fixedOrientation = fixedOrientation;
	_animDuration = animDuration;
}

void OnHitAnimation::stopAnim() {

}

void OnHitAnimation::updateTexture(anim_args_t *args) {
	std::shared_ptr<AnimationNode> newtonAnimNode = args->nodes[0];

	PhysicalObject *physObj = (PhysicalObject*)args->obj;
	std::shared_ptr<ActionManager> actions = args->actions;

	int numCols = newtonAnimNode->getCols();

	if (physObj->getLastCollidedObject() != nullptr) {
		if (!actions->isActive(_onHitAnimKey.c_str()) && physObj->getLastCollidedObject()->isPlayer()) {
			//int numCols = newtonAnimNode->getCols();
			//int orientation = 0; // fixed for now, should specify in init function depending on direction it's facing
			int start = numCols * _fixedOrientation;
			int range = numCols - 1;
			int end = start + range;

			std::shared_ptr<Animate> onHitAction = Animate::alloc(start, end, _animDuration);

			actions->activate(_onHitAnimKey.c_str(), onHitAction, newtonAnimNode);
		}
		else {
			
		}
	}
	else {
		
		newtonAnimNode->setFrame(numCols * _fixedOrientation);
	}




}
