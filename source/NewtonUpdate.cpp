
#include "NewtonUpdate.h"
#include "cugl/cugl.h"
#include "LevelStartup.h"
#include "Constants.h"


void NewtonUpdate::doLogic(do_logic_args_t *args) {

	bool isTouch = args->isTouch;
	cugl::Vec2 dispVec = args->touchLoc;
	
	PhysicalObject *newton = (PhysicalObject*)args->obj;
	LevelStartup *level = (LevelStartup*)args->level;

	bool valAtk = false;

	if (!newton->isDead()) {
		cugl::Vec2 attackVec = cugl::Vec2::ZERO;
		//attackVec = dispVec;
		attackVec = getDirectionFromTouch(dispVec, newton->getPosition());
		dispVec = attackVec;

		int numAttacks = 0;
		newton->setRejectionThisFrame(false);
		const attackInfo_t *projInfo; 

		bool reducedSpeed = false;
		if (isTouch) {
			cugl::Vec2 newtonVel = newton->getVelocity();
			float angle = acosf(newtonVel.getNormalization().dot(attackVec.getNormalization()));
			//CULog("%f %f %f %f %f\n", newtonVel.x, newtonVel.y, attackVec.x, attackVec.y, angle);
			if ((newtonVel.length() > NEWTON_STOP_TURN_SPEED) /*(newton->getTimeSinceLastAttack() < 750)*/ && (angle > NEWTON_BLOCK_ANGLE)) {
				CULog("invalid\n");
				reducedSpeed = true;
				//valAtk = false;
				newton->setRejectionThisFrame(true);
			}
			//else {
				projInfo = newton->doAttack(isTouch, attackVec, &numAttacks);
			//}
		}
		//projInfo = newton->doAttack(isTouch, attackVec, &numAttacks);

		
		if (numAttacks > 0) {
			if (projInfo[0].validAttack) {
				valAtk = true;
			}
		}


		/*// prevent a large change in angle from current velocity (at least if you are above a certain speed threshold
		if (isTouch) {
			cugl::Vec2 newtonVel = newton->getVelocity();
			float angle = acosf(newtonVel.getNormalization().dot(attackVec.getNormalization()));
			//CULog("%f %f %f %f %f\n", newtonVel.x, newtonVel.y, attackVec.x, attackVec.y, angle);
			if ((newtonVel.length() > NEWTON_STOP_TURN_SPEED) && (angle > NEWTON_BLOCK_ANGLE)) {
				CULog("invalid\n");
				valAtk = false;
			}
		}*/

		//if (isTouch) CULog("dispVec %f %f\n", dispVec.x, dispVec.y);
		// gun determiens if I can move? TODO: need seperate argumenet for validattack
		if (reducedSpeed) newton->doMovement(isTouch & valAtk, 0.25f * dispVec, &dispVec);
		else newton->doMovement(isTouch & valAtk, dispVec, &dispVec);

		//CULog("%f\n", _rocket->getVelocity().length());


		if (valAtk & isTouch) {
			//attackInfo_t attk;
			//AddProjectile(projInfo, dispVec);
			for (int i = 0; i < numAttacks; i++) {
				level->AddProjectile(projInfo[i], dispVec);
			}
			/*std::shared_ptr<Projectile> newProjectile = Projectile::alloc(Vec2::ZERO, Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE));
			AddGameObject<Projectile>(newProjectile, EARTH_TEXTURE, 10);

			Weapon::projectileSetup(_rocket.get(), newProjectile.get(), dispVec);*/
			std::string key = "NEWTON_SHOOT";
			auto source = args->assets->get<Sound>(NEWTON_SHOOT);
			if (!AudioEngine::get()->isActiveEffect(key)) {
				AudioEngine::get()->playEffect(key, source, false, 0.1);
			}

		}
	}

	newton->updateTexture(isTouch & valAtk, level->getGoal()->isComplete(), args->actions, args->dt);
}



cugl::Vec2 NewtonUpdate::getDirectionFromTouch(cugl::Vec2 touchLoc, cugl::Vec2 objPos) {
	cugl::Vec2 dispVec;
	int physVertShift = Application::get()->getDisplayHeight();

	cugl::Vec2 pos = objPos * Application::get()->getDisplayWidth() / 32.0f;// TODO _scale;
	pos.y = physVertShift - pos.y;
	dispVec = pos - touchLoc;
	dispVec.y = -1.0f * dispVec.y;

	dispVec.normalize();

	return dispVec;
}