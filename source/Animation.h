
#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "cugl/cugl.h"

using namespace cugl;

enum animState {
    IDLE,
    SHOOT,
	MOVE,
	TRANSFORM,
	DAMAGED,
	DEATH,
	VICTORY
};

typedef struct anim_args {
	void *obj;
	bool newAttack;
	bool levelExit;
	int orientation;
	int timeSinceLastAction;
	std::shared_ptr<AnimationNode> *nodes;
	std::shared_ptr<ActionManager> actions;
	float dt;
} anim_args_t;

class Animation {
protected:
	

public:
	virtual void updateTexture(anim_args_t *args) { return; }

	virtual void stopAnim() { return; }
};
#endif
