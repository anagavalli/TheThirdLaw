/* interface for movement delegation. 
* all movement delegations should implement the following methods
*/

#ifndef __MOVEMENT_H__
#define __MOVEMENT_H__

#include "cugl/cugl.h"

class Movement {
public:
	virtual void move(void *obj, bool isNewResp, cugl::Vec2 direction, cugl::Vec2 *dispVec) { return; }
	virtual void adjustSpeed(void *obj, float speedMult) { return; }
};

#endif
