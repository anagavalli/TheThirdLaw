#ifndef __ON_HIT_ANIM_H__
#define __ON_HIT_ANIM_H__

#include "Animation.h"

using namespace cugl;

class OnHitAnimation : public Animation {
protected:
	std::string _onHitAnimKey;
	int _fixedOrientation;
	float _animDuration;
public:
	void init(std::string animKey, int fixedOrientation, float animDuration);
	virtual void updateTexture(anim_args_t *args) override;

	virtual void stopAnim() override;
};

#endif
