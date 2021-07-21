/*
* Health bar, needs to update texture when take damage
*/
#ifndef __HEALTH_BAR_ANIM_H__
#define __HEALTH_BAR_ANIM_H__

#include "Animation.h"

class HealthBarAnimation : public Animation {
protected:

public:
	virtual void updateTexture(anim_args_t *args) override;

	virtual void stopAnim() override;
};




#endif
