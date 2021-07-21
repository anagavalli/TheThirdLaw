
#include "FencedAI.h"
#include "PhysicalObject.h"

void FencedAI::init(float aggroDistance, float unAggroDistance, float shootDistance, cugl::Vec2 startingPos, float fencedRadius) {
	BasicAI::init(aggroDistance, unAggroDistance, shootDistance);

	_startingPos = startingPos;
	_fencedRadius = fencedRadius;
}


void FencedAI::pathfinding(AI_args_t *args, bool doMaxEffort) {

	// move backwards if past the fence
	PhysicalObject *player = (PhysicalObject*)args->player;
	PhysicalObject *enemy = (PhysicalObject*)args->enemy;
	Vec2 pPos = player->getPosition();
	Vec2 ePos = enemy->getPosition();
	
	// don't move if 0.0f
	if (fabs(_fencedRadius) < CU_MATH_EPSILON) {
		enemy->setLinearVelocity(Vec2::ZERO);
		enemy->setPosition(_startingPos + args->staticPosChange);
		return;
	}


	Vec2 dispFromHook = ePos - (_startingPos + args->staticPosChange);
	float dist = dispFromHook.length();

	//CULog("%f %f\n", ePos, (_startingPos + args->staticPosChange));

	if (dist > _fencedRadius) {
		enemy->setLinearVelocity(Vec2::ZERO);
		standardMovement(args, _startingPos + args->staticPosChange); // TODO starting point changes
	}
	else {
		standardMovement(args, pPos);
	}

	//if (dist <)
}