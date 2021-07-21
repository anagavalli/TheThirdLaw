/* 
* Modify speed upon collision 
*/

#include "LogicCollision.h"


#ifndef __CHANGE_SPEED_COLLISION_H__
#define __CHANGE_SPEED_COLLISION_H__

class ChangeSpeedCollision : public LogicCollision {
protected:
	float _speedMult;
public:
	void init(float speedMult);
	virtual void onCollide(void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) override;
};



#endif