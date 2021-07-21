#ifndef __CHASE_BOSS_ANIM_H__
#define __CHASE_BOSS_ANIM_H__

#include "Animation.h"

using namespace cugl;

class ChaseBossAnim : public Animation {
protected:
	std::string _idleAnimKey;
	std::string _shootAnimKey;
	std::string _transformAnimKey;
	bool _isTransformed;
public:
	void init(std::string idle, std::string shoot, std::string transform);
	virtual void updateTexture(anim_args_t *args) override;

	virtual void stopAnim() override;
};

#endif
