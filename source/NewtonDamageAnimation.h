#ifndef __NEWTON_DAMAGE_ANIM_H__
#define __NEWTON_DAMAGE_ANIM_H__

#include "Animation.h"

using namespace cugl;

class NewtonDamageAnimation : public Animation {
protected:
	std::string _animKey;
	int _animState;
public:
	void init(std::string animKey);
	virtual void updateTexture(anim_args_t *args) override;

	virtual void stopAnim() override;
};


#endif