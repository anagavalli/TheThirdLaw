/*
* Custom logic called on every update routine not external stimuli (Newton move scheme, shield regen)
* Different from AIControl which deals with move and attacks (external stimuli)
* Different from LogicCollision which is called upon collide
* Also these should not conflict with one another so array of delegates used
*/

#ifndef __UPDATE_LOGIC_H__
#define __UPDATE_LOGIC_H__

#include "cugl/cugl.h"

typedef struct dologic_args{
	void *obj;
	bool isTouch;
	cugl::Vec2 touchLoc;
	float dt;
	std::shared_ptr<cugl::ActionManager> actions;
	cugl::AssetManager *assets;
	void *level;
} do_logic_args_t;


class UpdateLogic {
public:
	virtual void doLogic(do_logic_args_t *args) { return; }
};



#endif