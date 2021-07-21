
#include "SwitchWeapons.h"
#include "PhysicalObject.h"

// one attack for every switch health
void SwitchWeapons::init(std::vector<int> &switchHealths, std::vector<std::shared_ptr<Attack>> &attacks) {
	assert(switchHealths.size() == attacks.size());

	for (int i = 0; i < switchHealths.size(); i++) {
		_switchHealths.push_back(switchHealths[i]);
		_attacks.push_back(attacks[i]);
	}
}

void SwitchWeapons::doLogic(do_logic_args_t *args) {
	PhysicalObject *physObj = (PhysicalObject*)args->obj;

	for (int i = 0; i < _switchHealths.size(); i++) {
		if (physObj->getCurrentHealth() <= _switchHealths[i]) {
			physObj->delegateAttack(_attacks[i]);


			_switchHealths.erase(_switchHealths.begin() + i);
			_attacks[i] = nullptr;
			_attacks.erase(_attacks.begin() + i);
			break;
		}
	}
}