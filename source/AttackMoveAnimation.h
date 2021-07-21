#ifndef __ATTACK_MOVE_ANIM_H__
#define __ATTACK_MOVE_ANIM_H__

#include "Animation.h"

using namespace cugl;

class AttackMoveAnimation : public Animation {
protected:
	std::string _moveAnimKey;
	std::string _shootAnimKey;
public:
	void init(std::string move, std::string shoot);
	virtual void updateTexture(anim_args_t *args) override;

	virtual void stopAnim() override;
};

#endif