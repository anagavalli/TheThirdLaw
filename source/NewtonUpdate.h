/*
* Does update logic for newton called on every frame
*/

#ifndef __NEWTON_UPDATE_H__
#define __NEWTON_UPDATE_H__


#include "UpdateLogic.h"

class NewtonUpdate : public UpdateLogic {
public:
	virtual void doLogic(do_logic_args_t *args) override;
	cugl::Vec2 getDirectionFromTouch(cugl::Vec2 touchLoc, cugl::Vec2 objPos);
};








#endif
