

#include "BasicAI.h"
#include "Attack.h"
#include "PhysicalObject.h"
#include "ObjectContainer.h"
#include "MapPath.h"
#include <Box2D/Dynamics/Contacts/b2Contact.h>

using namespace cugl;

void BasicAI::init(float aggroDistance, float unAggroDistance, float shootDistance) {
	_lastMoveTime.mark();
	_aggroDistance = aggroDistance;
	_unAggroDistance = unAggroDistance;
	_aggroed = false;
	_shootDistance = shootDistance;
	_searchHeight = (aggroDistance + aggroDistance) / SEARCH_BLOCK_SIZE;
	_searchWidth = (aggroDistance + aggroDistance) / SEARCH_BLOCK_SIZE;
	_useOldAStarSearch = false;
}

void BasicAI::processBehavior(AI_args_t *args) {
	PhysicalObject *enemy = (PhysicalObject*)args->enemy;
	PhysicalObject *player = (PhysicalObject*)args->player;


	if (enemy->getCurrentHealth() <= 0) {
//        std::string key1 = "ENEMY_MOVE2";
//        if (AudioEngine::get()->isActiveEffect(key1)) { AudioEngine::get()->stopEffect(key1); }
//
//        std::string key2 = "ENEMY_DEATH1";
//        auto source = args->assets->get<Sound>(ENEMY_DEATH1);
//        if (!AudioEngine::get()->isActiveEffect(key2)) { AudioEngine::get()->playEffect(key2, source, false, source->getVolume()); }

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

		float distance = diffVec.length();//sqrt(pow(rocketPos.x - enemyPos.x, 2) + pow(rocketPos.y - enemyPos.y, 2));

		// enemy movement w/ aggro hysteresis
		if (distance < _aggroDistance) {

			// only update movement desicion periodically?

			pathfinding(args, true);
			_aggroed = true;
		}
		else if (distance < _unAggroDistance & _aggroed) {
			pathfinding(args, false);
		}
		else {
			_aggroed = false;
		}

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

void BasicAI::pathfinding(AI_args_t *args, bool doMaxEffort) {
	PhysicalObject *enemy = (PhysicalObject*)args->enemy;
	PhysicalObject *player = (PhysicalObject*)args->player;


	Vec2 pPos = player->getPosition();
	Vec2 ePos = enemy->getPosition();

	/*Vec2 moveVec = pPos - ePos;
	moveVec.normalize();
	Vec2 dummyVec;
	CULog("move %f %f\n", moveVec.x, moveVec.y);
	enemy->doMovement(true, moveVec, &dummyVec);*/
	//doAStar(args, std::vector<Vec2>());
	bool foundPath = doRayCast(args);
	if (foundPath) {
		standardMovement(args, pPos);
		_useOldAStarSearch = false;
	}
	else if (doMaxEffort){
		// use old a* if we already have computed a path
		if (_useOldAStarSearch && _travelPts.size() > 0) {

			// check if have reached the next pt in search (or close enough!)
			Vec2 nextLoc = _travelPts[0] + args->staticPosChange; // account for camera
			Vec2 disp = nextLoc - ePos;
			float dist = disp.length();
			if (dist < SEARCH_BLOCK_SIZE / 2.0f) { // within block
				_travelPts.erase(_travelPts.begin()); // pop off so go to next pt in the frame
			}

			Vec2 moveVec = nextLoc - ePos;
			moveVec.normalize();
			Vec2 dummyVec;
			enemy->doMovement(true, moveVec, &dummyVec);
			
		}
		// try to find new path with a*
		else {

			//std::vector<Vec2> travelPts;
			_travelPts.clear();
			_travelPts.shrink_to_fit();

			foundPath = doAStar(args, _travelPts);

			if (foundPath) {
				Vec2 nextLoc = _travelPts[0] + args->staticPosChange;

				Vec2 moveVec = nextLoc - ePos;
				moveVec.normalize();

				Vec2 dummyVec;
				CULog("a* move %f %f\n", moveVec.x, moveVec.y);
				enemy->doMovement(true, moveVec, &dummyVec);

				_useOldAStarSearch = true;

			}
			else { // if we still cannot find a path just move towards the player
				CULog("did not find path\n");
				standardMovement(args, pPos);
				_useOldAStarSearch = false;

			}
		}
	}
	else { // no good movement options
		standardMovement(args, pPos);
		_useOldAStarSearch = false;
	}
	
	

	/*Vec2 enemyLoc = enemy->getPosition();
	Vec2 charLoc = player->getPosition();

	float dX = charLoc.x - enemyLoc.x;
	float dY = charLoc.y - enemyLoc.y;



	Vec2 up = Vec2(enemyLoc.x, enemyLoc.y + 1.0);
	Vec2 down = Vec2(enemyLoc.x, enemyLoc.y - 1.0);
	Vec2 right = Vec2(enemyLoc.x + 1.0, enemyLoc.y);
	Vec2 left = Vec2(enemyLoc.x - 1.0, enemyLoc.y);
	Vec2 directions[4] = { up, down, left, right };
	float distance = 100000.0;
	int direction = 0;
	for (int dir = 0; dir < 4; ++dir) {
		float pathDist = sqrt(pow(directions[dir].x - enemyLoc.x, 2) + pow(directions[dir].y - enemyLoc.y, 2));
		if (pathDist < distance) {
			distance = pathDist;
			direction = dir;
		}
	}

	if (dX < 1.0 && dY < 1.0) {
		Vec2 dummyVec;
		enemy->doMovement(true, Vec2(dX, dY), &dummyVec);
	}
	else {
		dX = directions[direction].x - enemyLoc.x + 1.0;
		dY = directions[direction].y - enemyLoc.y;
		Vec2 dummyVec;
		enemy->doMovement(true, Vec2(dX, dY), &dummyVec);
	}*/
}

void BasicAI::standardMovement(AI_args_t *args, cugl::Vec2 targPos) {
	PhysicalObject *enemy = (PhysicalObject*)args->enemy;
	//PhysicalObject *player = (PhysicalObject*)args->player;


	Vec2 pPos = targPos; //player->getPosition();
	Vec2 ePos = enemy->getPosition();

	Vec2 moveVec = pPos - ePos;
	float mag = moveVec.length();
	moveVec.normalize();

	// normal acceleration if need to turn
	// get angle between move vec and current velocity
	Vec2 currentVel = enemy->getVelocity();
	currentVel.normalize();
	float dot = currentVel.dot(moveVec); // both normalized so no need to divide by scalr
	float angle = acosf(dot); // always positive, but I think increase from 0 in both directions

	if (angle > 1.0f) {
		moveVec *= (angle * SEEKER_TURNING_RATE);
	}

	//CULog("%f angle\n", angle);



	// speed up if farther away
	moveVec *= (mag * SEEKER_DISTANCE_SPEED_UP);


	Vec2 dummyVec;
	//CULog("raycast move %f %f\n", moveVec.x, moveVec.y);
	enemy->doMovement(true, moveVec, &dummyVec);
}

bool BasicAI::doAStar(AI_args_t *args, std::vector<cugl::Vec2> &travelPts) {
	PhysicalObject *enemy = (PhysicalObject*)args->enemy;
	PhysicalObject *player = (PhysicalObject*)args->player;
	ObjectContainer *allObjects = (ObjectContainer*)args->objContainer;

	int pathMap[MAX_SEARCH_BOXES];
	int startX = _searchWidth / 2;
	int startY = _searchHeight / 2;
	int targX = 0;
	int targY = 0;

	Vec2 ePos = enemy->getPosition();
	Vec2 pPos = player->getPosition();
	Vec2 size = Vec2(SEARCH_BLOCK_SIZE, SEARCH_BLOCK_SIZE);

	//int queries[SEARCH_WIDTH * SEARCH_HEIGHT];

	// create query boxes
	for (int i = 0; i < (_searchWidth * _searchHeight); i++) {
		int y = (i / _searchWidth) - _searchHeight / 2;
		int x = (i % _searchWidth) - _searchHeight / 2;

		Vec2 checkPos = ePos + (Vec2(x, y) * size);


		if (searchBoxes[i].get() != nullptr) {
			searchBoxes[i]->setDebugScene(nullptr);
			//args->world->removeObstacle(searchBoxes[i].get());
		}

		searchBoxes[i] = BoxObstacle::alloc(checkPos, size);
		searchBoxes[i]->setSensor(true);
		args->world->addObstacle(searchBoxes[i]);
	}

	// process query (can't do aabb query because aabb query on boundary are really big (axis-aligned!!!)
	args->world->activateCollisionCallbacks(false);
	args->world->update(0);
	args->world->activateCollisionCallbacks(true);

	// check if we got any hits
	for (int i = 0; i < (_searchWidth * _searchHeight); i++) {
		int y = (i / _searchWidth) - _searchHeight / 2;
		int x = (i % _searchWidth) - _searchHeight / 2;

		b2ContactEdge* conts = searchBoxes[i]->getBody()->GetContactList();
		int testit = 0;
		while (conts != nullptr) {
			bool bothSensor = conts->contact->GetFixtureA()->IsSensor() &&
				conts->contact->GetFixtureB()->IsSensor();
			if (conts->contact->IsTouching() && !bothSensor) {

				//queries[i] = 1;
				testit = 1;

				// check if enemy is newton
				b2Body* body1 = conts->contact->GetFixtureA()->GetBody();
				//b2Body* body2 = conts->contact->GetFixtureB()->GetBody();
				cugl::SimpleObstacle *obj1 = (cugl::SimpleObstacle *)body1->GetUserData();
				//cugl::SimpleObstacle *obj2 = (cugl::SimpleObstacle *)body2->GetUserData();
				int idx1 = obj1->getIdx();
				//int idx2 = obj2->getIdx();
				int ver1 = obj1->getVersion();
				//int ver2 = obj2->getVersion();
				if (idx1 >= 0) {
					PhysicalObject *physObj1 = (allObjects->lookup(idx1, ver1)).get();
                    if (physObj1 != nullptr){
                        if (physObj1->isPlayer()) {
                            testit = 2;
                            break;
                        }
                        else if (physObj1 == enemy | !physObj1->doCollide(nullptr)) {
                            testit = 0;
                        }
                    
                    }
					
				}


			}
			conts = conts->next;
		}

		if (testit == 1) {
			searchBoxes[i]->setDebugColor(HIT_COLOR);
			pathMap[i] = BLOCKED_TERRAIN;
		}
		else if (testit == 2) {
			searchBoxes[i]->setDebugColor(GOAL_COLOR);
			pathMap[i] = UNBLOCKED_TERRAIN;
			targX = x + _searchWidth / 2;
			targY = y + _searchHeight / 2;
		}
		else {
			searchBoxes[i]->setDebugColor(MISS_COLOR);
			pathMap[i] = UNBLOCKED_TERRAIN;
		}

		searchBoxes[i]->setDebugScene(args->debugNode);

		// cleanup
		args->world->removeObstacle(searchBoxes[i].get());
	}

	int isValid;
	MapPath searcher;
	travelPts = searcher.doSearch(pathMap, _searchWidth, _searchHeight, 
		ePos - args->staticPosChange, size.x, startX, startY, targX, targY, &isValid);

	if (isValid == 1) {
		/*for (int i = 0; i < travelPts.size(); i++) {
			if (i == 0)
				CULog("next pt %f %f -> %f %f", ePos.x, ePos.y, travelPts[i].x, travelPts[i].y);
			else
				CULog("next pt %f %f -> %f %f", travelPts[i - 1].x, travelPts[i - 1].y, travelPts[i].x, travelPts[i].y);
		}*/

		return true;
	}
	else {
		return false;
	}
}


bool BasicAI::doRayCast(AI_args_t *args) {
	PhysicalObject *enemy = (PhysicalObject*)args->enemy;
	PhysicalObject *player = (PhysicalObject*)args->player;
	ObjectContainer *allObjects = (ObjectContainer*)args->objContainer;

	Vec2 ePos = enemy->getPosition();
	Vec2 pPos = player->getPosition();

	
	float stepRange = 2.0f; // TODO
	int numSteps = 3;
	float stepSize = stepRange / (float)(numSteps-1);
	Vec2 dir = enemy->getVelocity().getNormalization();

	// rotate about direction vec to get normal
	cugl::Quaternion rot = cugl::Quaternion();
	rot.set(cugl::Vec3(0.0f,0.0f,1.0f), PI / 2); // rotate into z-axis by 90 degrees around normal vec
	cugl::Vec3 normal3D = rot.getRotation(cugl::Vec3(dir));

	cugl::Vec2 normal = Vec2(normal3D.x, normal3D.y);
	normal.normalize();

	// move along normal and ray cast from there
	ePos -= (stepRange / 2.0f) * normal;
	//pPos -= (stepRange / 2.0f) * normal;

	// ray cast to check if anything in between, otherwise do pathfind (note lambda cannot modify local variable)
	rayCastHit = false;

	for (int i = 0; i < numSteps; i++) {
 
		//CULog("check %d: %f %f going %f %f  , %f %f", i, enemy->getPosition().x, enemy->getPosition().y, enemy->getVelocity().x, enemy->getVelocity().y, ePos.x, ePos.y);


		args->world->rayCast([=](b2Fixture *fixture, const Vec2 &point, const Vec2 &normal, float fraction) {
			b2Body* body = fixture->GetBody();
			cugl::SimpleObstacle *obj = (cugl::SimpleObstacle *)body->GetUserData();
			int idx = obj->getIdx();
			int ver = obj->getVersion();
			if (idx >= 0) {
				auto physObj = allObjects->lookup(idx, ver);

				// make sure we can actually collide with this (not zone or sensor), is not a bullet, and is not the player hitbox
                if (physObj != nullptr){
					if ((physObj->doCollide(nullptr)) && (!fixture->IsSensor()) && (!body->IsBullet()) 
						&& (!physObj->isPlayer()) && /*(physObj.get() != enemy) &&*/ (!physObj->getIsEnemy())) {
						//CULog("collide loc %f", fraction);
						if (fraction < 1.0f) rayCastHit = true;
					}
                }

			}
			return 0.0f; // not sure what the return does

		}, ePos, pPos);

		ePos += stepSize * normal;
		//pPos += stepSize * normal;

	}

	return !rayCastHit;
}
