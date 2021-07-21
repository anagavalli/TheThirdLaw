
#include "PathFollowAI.h"
#include "PhysicalObject.h"
#include "Constants.h"

using namespace cugl;

void PathFollowAI::init(std::vector<cugl::Vec2> &pathPts, float shootDistance, float catchUpDistThresh, float chasePlayerMod) {
	for (int i = 0; i < pathPts.size(); i++) {
		_pathPts.push_back(pathPts[i]);
	}

	_shootDistance = shootDistance;
	_catchUpDistThresh = catchUpDistThresh;
	_chasePlayerMod = chasePlayerMod;
	_curPatrolIndex = 0;
}

void PathFollowAI::processBehavior(AI_args_t *args) {
	PhysicalObject *enemy = (PhysicalObject*)args->enemy;
	PhysicalObject *player = (PhysicalObject*)args->player;


	if (enemy->getCurrentHealth() <= 0) {
		// TODO
		enemy->setCurrentHealth(-1); // so sound doesn't play again
	}
	else if (enemy->getCurrentHealth() > 0) {
		/*std::string key1 = "ENEMY_MOVE2"; // causing a crash sometimes
		std::string key = "ENEMY_MOVE2";
		auto source = args->assets->get<Sound>(ENEMY_MOVE2);
		if (!AudioEngine::get()->isActiveEffect(key) && !AudioEngine::get()->isActiveEffect(key1)) {
		AudioEngine::get()->playEffect(key, source, false, 0.2);
		}*/

		Vec2 rocketPos = player->getPosition();
		Vec2 enemyPos = enemy->getPosition();
		Vec2 diffVec = enemyPos - rocketPos;

		float distance = diffVec.length();



		//hardcoded pathToBox2d for patrol testing
		//float pathToBox2d = 0.0651041791f;

		//hardcoded positions (in Box2d units) for patrol testing
		//std::vector<Vec2> patrolPos({ Vec2(990 * pathToBox2d, 458 * pathToBox2d), Vec2(990 * pathToBox2d, 500 * pathToBox2d), Vec2(900 * pathToBox2d, 475 * pathToBox2d) });
		//std::vector<Vec2> patrolPos({ Vec2(990 * pathToBox2d, 458 * pathToBox2d), Vec2(990 * pathToBox2d, 500 * pathToBox2d), Vec2(990 * pathToBox2d, 600 * pathToBox2d) });


		Vec2 targetPos = _pathPts[_curPatrolIndex] + args->staticPosChange;
		//Vec2 enemyPos = enemy->getPosition();

		/*if (_curPatrolIndex == 1) {
			CULog("enemyPos: (%f, %f)", enemyPos.x, enemyPos.y);
			CULog("targetPos: (%f, %f)", targetPos.x, targetPos.y);
		}
		if (_curPatrolIndex == 2) {
			CULog("heading to patrol index 2");
			CULog("enemyPos: (%f, %f)", enemyPos.x, enemyPos.y);
			CULog("targetPos: (%f, %f)", targetPos.x, targetPos.y);
		}*/

		if ((enemyPos - targetPos).lengthSquared() < PATH_GRANULARITY) {
			_curPatrolIndex = (_curPatrolIndex + 1) % _pathPts.size();
			targetPos = _pathPts[_curPatrolIndex] + args->staticPosChange;
		}

		//CULog("enemy pos %f %f targ pos %f %f offset %f %f rock pos %f %f camera %f %f\n", enemyPos.x, enemyPos.y, _pathPts[_curPatrolIndex].x, _pathPts[_curPatrolIndex].y, targetPos.x, targetPos.y, rocketPos.x, rocketPos.y, args->staticPosChange.x, args->staticPosChange.y);

		// movement
		moveToTarget(args, targetPos);

		// enemy attacks
		if (distance < _shootDistance) {
			// get unit attackVec
			Vec2 normDiffVec = diffVec.getNormalization();


			// let 'gun' decide whether another attack is allowed (based on fire-rate)
			args->atk = enemy->doAttack(true, normDiffVec, &(args->numAttacks));

			args->doAttack = false;
			if (args->numAttacks > 0) {
				if (args->atk[0].validAttack) {
					/*CULog("fire %f %f %f %f -> %f %f %f %f \n", enemyPos.x, enemyPos.y, rocketPos.x, rocketPos.y,
					args->atk.pos.x, args->atk.pos.y, args->atk.velocity.x, args->atk.velocity.y);*/

					args->doAttack = true;
				}
			}

		}
		else {
			args->doAttack = false;
		}
	}
	else {
		args->doAttack = false;
	}
}

void PathFollowAI::moveToTarget(AI_args_t *args, cugl::Vec2 targPos) {
	PhysicalObject *nme = (PhysicalObject*)args->enemy;
	PhysicalObject *player = (PhysicalObject*)args->player;

	cugl::Vec2 direction = targPos - nme->getPosition();

	if (direction.length() > 1) {
		direction = direction.getNormalization();
	}

	// move faster if far from player
	float distance = (nme->getPosition() - player->getPosition()).length();

	bool fequals = fabs(direction.length() - 1.0f) < 0.0001f;
	if (distance > _catchUpDistThresh && fequals) {
		float magMod = distance * _chasePlayerMod; // 0.0f if no modifier
		direction = direction * (1.0f + magMod);
	}


	cugl::Vec2 dummy;
	nme->doMovement(true, direction, &dummy);

}