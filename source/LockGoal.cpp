#include "LockGoal.h"
#include "PhysicalObject.h"
#include "PassCollision.h"

void LockGoal::init(ObjectContainer *objCont, PhysicalObject *goalZone) {
	_objContainer = objCont;
	enemiesRem = true;
	_goalZone = goalZone;
	GoalCollision::init();
}

void LockGoal::onCollide(void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) {
	PhysicalObject *otherPhysObj = (PhysicalObject*)otherObj;

	if (!enemiesRem)
		_complete = otherPhysObj->isPlayer();
}

void LockGoal::updateGoal() {
	enemiesRem = false;
	for (int i = 0; i < _objContainer->getSize(); i++) {
		std::shared_ptr<PhysicalObject> enemy = _objContainer->get(i);
		if (enemy->getIsEnemy()) {
			if (enemy->getCurrentHealth() > 0) {
				enemiesRem = true;
				break;
			}

		}
	}

	auto animNodes = _goalZone->getAnimationNodes(0);
	if (enemiesRem && animNodes[0]->isVisible()) {
		animNodes[0]->setVisible(false);
		animNodes[1]->setVisible(true);
	}
	else if (!enemiesRem && animNodes[1]->isVisible()) {
		animNodes[0]->setVisible(true);
		animNodes[1]->setVisible(false);

		auto pass = std::make_shared<PassCollision>();
		pass->init(true, true);
		_goalZone->delegatePhysCollision(pass);
	}
}