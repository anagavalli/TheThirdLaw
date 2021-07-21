#ifndef __REPEAT_ANIM_H__
#define __REPEAT_ANIM_H__

#include "Animation.h"

using namespace cugl;

class RepeatAnimation : public Animation {
protected:
	std::string _repeatAnimKey;
public:
	void init(std::string animKey);
	virtual void updateTexture(anim_args_t *args) override;

	virtual void stopAnim() override;
};

#endif
