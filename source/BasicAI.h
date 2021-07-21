

#ifndef __BASIC_AI__
#define __BASIC_AI__

#include "AIControl.h"
#include "cugl/cugl.h"
#include "Constants.h"

//#define SEARCH_WIDTH 7
//#define SEARCH_HEIGHT 7

class BasicAI : public AIControl {
protected:
	cugl::Timestamp _lastMoveTime;
	float _aggroDistance;
	float _unAggroDistance;
	bool _aggroed; // hysteresis after being initially aggroed
	float _shootDistance;
	int _searchHeight;
	int _searchWidth;
	bool _useOldAStarSearch;
	std::vector<cugl::Vec2> _travelPts;

	std::shared_ptr<cugl::BoxObstacle> searchBoxes[MAX_SEARCH_BOXES]; // TODO
	bool rayCastHit;
public:
	virtual void init(float aggroDistance, float unAggroDistance, float shootDistance);
	virtual void processBehavior(AI_args_t *args) override;
	virtual void pathfinding(AI_args_t *args, bool doMaxEffort);
	bool doAStar(AI_args_t *args, std::vector<cugl::Vec2> &travelPts);
	bool doRayCast(AI_args_t *args);
	void standardMovement(AI_args_t *args, cugl::Vec2 targPos);


};


#endif
