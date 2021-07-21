#include "ChaseBossAnim.h"
#include "cugl/cugl.h"
#include "Constants.h"
#include "PhysicalObject.h"

using namespace cugl;

void ChaseBossAnim::init(std::string idle, std::string shoot, std::string transform) {
	_idleAnimKey = idle;
	_shootAnimKey = shoot;
	_transformAnimKey = transform;
	_isTransformed = false;
}

void ChaseBossAnim::stopAnim() {

}

void ChaseBossAnim::updateTexture(anim_args_t *args) {
	PhysicalObject *physObj = (PhysicalObject*)args->obj;

	std::shared_ptr<AnimationNode> p1MoveNode = args->nodes[0];
	std::shared_ptr<AnimationNode> p1ShootNode = args->nodes[1];
	std::shared_ptr<AnimationNode> transformNode = args->nodes[2];
	std::shared_ptr<AnimationNode> p2MoveNode = args->nodes[3];
	std::shared_ptr<AnimationNode> p2ShootNode = args->nodes[4];

	std::shared_ptr<ActionManager> actions = args->actions;
	animState state;
	/*if (args->timeSinceLastAction >= IDLE_DURATION_MS)
		state = animState::IDLE;
	else
		state = animState::SHOOT;*/
	int totHealth = physObj->getTotalHealth();
	int currHealth = physObj->getCurrentHealth();

	if (totHealth/currHealth >= 2 && !_isTransformed){
		state = animState::TRANSFORM;
		_isTransformed = true;
	}
	else if (args->newAttack) {
		state = animState::SHOOT;
	}
	else {
		state = animState::MOVE;
	}

	if (state == animState::SHOOT) {
		if (args->newAttack && !actions->isActive(_transformAnimKey.c_str())) {
			if (actions->isActive(_idleAnimKey.c_str())) {
				actions->remove(_idleAnimKey.c_str());
			}
			if (actions->isActive(_shootAnimKey.c_str()))
				actions->remove(_shootAnimKey.c_str());

			transformNode->setVisible(false);
			p1MoveNode->setVisible(false);
			p2MoveNode->setVisible(false);

			if (_isTransformed) {
				p1ShootNode->setVisible(false);
				p2ShootNode->setVisible(true);

				int numCols = p2ShootNode->getCols();
				int start = numCols * args->orientation;
				int range = numCols - 1;
				int end = start + range;

				std::shared_ptr<Animate> shootAction = Animate::alloc(start, end, 1.5f);

				actions->activate(_shootAnimKey.c_str(), shootAction, p2ShootNode);
			}
			else {
				p1ShootNode->setVisible(true);
				p2ShootNode->setVisible(false);

				int numCols = p1ShootNode->getCols();
				int start = numCols * args->orientation;
				int range = numCols - 1;
				int end = start + range;

				std::shared_ptr<Animate> shootAction = Animate::alloc(start, end, 1.5f);

				actions->activate(_shootAnimKey.c_str(), shootAction, p1ShootNode);
			}
			
				
		}
	}
	else if (state == animState::TRANSFORM) {
		p1MoveNode->setVisible(false);
		p1ShootNode->setVisible(false);
		transformNode->setVisible(true);

		if (actions->isActive(_idleAnimKey.c_str())) {
			actions->remove(_idleAnimKey.c_str());
		}
		if (actions->isActive(_shootAnimKey.c_str()))
			actions->remove(_shootAnimKey.c_str());

		// should never do transform twice

		// only two orientations for this just choose one for now.
		int tempOrientation = 0;

		int numCols = transformNode->getCols();
		int start = numCols * tempOrientation;
		int range = numCols - 1;
		int end = start + range;

		std::shared_ptr<Animate> transformAction = Animate::alloc(start, end, 2.0f);

		actions->activate(_transformAnimKey.c_str(), transformAction, transformNode);


	}
	else { // MOVE
		if (!actions->isActive(_shootAnimKey.c_str()) && !actions->isActive(_transformAnimKey.c_str())) {

			transformNode->setVisible(false);
			p1ShootNode->setVisible(false);
			p2ShootNode->setVisible(false);
			
			if (!actions->isActive(_idleAnimKey.c_str())) {

				if (_isTransformed) {
					p1MoveNode->setVisible(false);
					p2MoveNode->setVisible(true);

					int numCols = p2MoveNode->getCols();
					int start = numCols * args->orientation;
					int range = numCols - 1;
					int end = start + range;

					std::shared_ptr<Animate> idleAction = Animate::alloc(start, end, 1.0f);

					actions->activate(_idleAnimKey.c_str(), idleAction, p2MoveNode);
				}
				else {
					p1MoveNode->setVisible(true);
					p2MoveNode->setVisible(false);

					int numCols = p1MoveNode->getCols();
					int start = numCols * args->orientation;
					int range = numCols - 1;
					int end = start + range;

					std::shared_ptr<Animate> idleAction = Animate::alloc(start, end, 1.0f);

					actions->activate(_idleAnimKey.c_str(), idleAction, p1MoveNode);
				}
	
			}
		}
	}

	/*if (state == animState::SHOOT) {
		if (args->newAttack) {
			newtonIdleNode->setVisible(false);
			newtonShootNode->setVisible(true);
			if (actions->isActive(_idleAnimKey.c_str())) {
				actions->remove(_idleAnimKey.c_str());
			}
			if (actions->isActive(_shootAnimKey.c_str()))
				actions->remove(_shootAnimKey.c_str());

			int numCols = newtonShootNode->getCols();
			int start = numCols * args->orientation;
			int range = numCols - 1;
			int end = start + range;

			std::shared_ptr<Animate> shootAction = Animate::alloc(start, end, 1.0f);

			actions->activate(_shootAnimKey.c_str(), shootAction, newtonShootNode);
		}
	}
	else {
		if (!actions->isActive(_shootAnimKey.c_str())) {
			newtonShootNode->setVisible(false);
			newtonIdleNode->setVisible(true);
			if (!actions->isActive(_idleAnimKey.c_str())) {
				int numCols = newtonIdleNode->getCols();
				int start = numCols * args->orientation;
				int range = numCols - 1;
				int end = start + range;

				std::shared_ptr<Animate> idleAction = Animate::alloc(start, end, 1.0f);

				actions->activate(_idleAnimKey.c_str(), idleAction, newtonIdleNode);
			}
		}
	}*/
}
