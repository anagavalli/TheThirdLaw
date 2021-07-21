#include "RepeatAnimation.h"
#include "cugl/cugl.h"
#include "Constants.h"

using namespace cugl;

void RepeatAnimation::init(std::string animKey) {
	_repeatAnimKey = animKey;
}

void RepeatAnimation::stopAnim() {

}

void RepeatAnimation::updateTexture(anim_args_t *args) {
	std::shared_ptr<AnimationNode> newtonAnimNode = args->nodes[0];

	std::shared_ptr<ActionManager> actions = args->actions;

	/*animState state;
	if (args->timeSinceLastAction >= IDLE_DURATION_MS)
		state = animState::IDLE;
	else
		state = animState::SHOOT;*/

	if (!actions->isActive(_repeatAnimKey.c_str())) {
		int numCols = newtonAnimNode->getCols();
		int orientation = 0; // fixed for now, should specify in init function depending on direction it's facing
		int start = numCols * orientation;
		int range = numCols - 1;
		int end = start + range;

		std::shared_ptr<Animate> repeatAction = Animate::alloc(start, end, 1.0f);

		actions->activate(_repeatAnimKey.c_str(), repeatAction, newtonAnimNode);
	}

}
