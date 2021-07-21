/*
* Put exclaimation point over character's head when going fast!
*/
#ifndef __EXCLAIM_ANIM_H__
#define __EXCLAIM_ANIM_H__

#include "Animation.h"

using namespace cugl;

class ExclaimAnimation : public Animation {
protected:
	std::string _animKey;
public:
	void init(std::string animKey);
	virtual void updateTexture(anim_args_t *args) override;

	virtual void stopAnim() override;
};




#endif

