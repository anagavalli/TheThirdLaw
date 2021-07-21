/*
* AI follows a path exclusively, and does not deviate.
* Not really compatible with force based movement, due to orbiting around a destination point.
* Needs to be velocity based.
*/

#ifndef __PATH_FOLLOW_AI_H__
#define __PATH_FOLLOW_AI_H__

#include "AIControl.h"

class PathFollowAI : public AIControl {
protected:
	std::vector<cugl::Vec2> _pathPts;
	float _shootDistance;
	int _curPatrolIndex;
	float _catchUpDistThresh;
	float _chasePlayerMod;

public:
	void init(std::vector<cugl::Vec2> &pathPts, float shootDistance, float catchUpDistThresh, float chasePlayerMod);
	void processBehavior(AI_args_t *args) override;
	void moveToTarget(AI_args_t *args, cugl::Vec2 targPos);

};

#endif
