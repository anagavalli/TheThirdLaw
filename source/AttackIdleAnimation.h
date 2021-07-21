#ifndef __ATTACK_IDLE_ANIM_H__
#define __ATTACK_IDLE_ANIM_H__

#include "Animation.h"

using namespace cugl;

class AttackIdleAnimation : public Animation {
protected:
	std::string _idleAnimKey;
	std::string _shootAnimKey;
public:
	void init(std::string idle, std::string shoot);
	virtual void updateTexture(anim_args_t *args) override;

	virtual void stopAnim() override;
};

#endif