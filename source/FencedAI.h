
#ifndef __FENCED_AI_H__
#define __FENCED_AI_H__

#include "BasicAI.h"

class FencedAI : public BasicAI {
protected:
	cugl::Vec2 _startingPos;
	float _fencedRadius;
public:

	virtual void init(float aggroDistance, float unAggroDistance, float shootDistance) override { init(aggroDistance, unAggroDistance, shootDistance, cugl::Vec2::ZERO, 0.0f); }
	virtual void init(float aggroDistance, float unAggroDistance, float shootDistance, cugl::Vec2 startingPos, float fencedRadius);
	virtual void pathfinding(AI_args_t *args, bool doMaxEffort) override;
};


#endif
