/*
* Delegate for AI Control
*/


#ifndef __AI_CONTROL_H__
#define __AI_CONTROL_H__

#include "cugl/cugl.h"
#include "Attack.h"

typedef struct AI_args {
	void *player;
	void *enemy;
	bool doMove;
	bool doAttack;
	cugl::ObstacleWorld *world;
	void *objContainer;
	cugl::AssetManager *assets;
	const attackInfo_t *atk;
	int numAttacks;
	std::shared_ptr<cugl::Node> debugNode;
	cugl::Vec2 staticPosChange;
} AI_args_t;


class AIControl {
public:
	virtual void processBehavior(AI_args_t *args) { return; }

};

#endif