#ifndef __NEWTON_ANIM_H__
#define __NEWTON_ANIM_H__

#include "Animation.h"

using namespace cugl;

class NewtonAnimation : public Animation {
protected:
	std::string _idleAnimKey;
	std::string _shootAnimKey;
	std::string _deathAnimKey;
	std::string _victoryAnimKey;
	bool _startedDeath;
	int _numVictoryPumps;
public:
	void init(std::string idle, std::string shoot, std::string death, std::string victory);
	virtual void updateTexture(anim_args_t *args) override;

	virtual void stopAnim() override;
};

#endif