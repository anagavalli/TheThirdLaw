/*
* Switch weapons after given time periods
*/

#ifndef __SWITCH_WEAPONS_H__
#define __SWITCH_WEAPONS_H__

#include "UpdateLogic.h"
#include "Attack.h"
#include "cugl/cugl.h"

class SwitchWeapons : public UpdateLogic {
protected:
	std::vector<int> _switchHealths;
	std::vector<std::shared_ptr<Attack>> _attacks;

public:
	void init(std::vector<int> &switchHealths, std::vector<std::shared_ptr<Attack>> &attacks);
	void doLogic(do_logic_args_t *args) override;
};



#endif
