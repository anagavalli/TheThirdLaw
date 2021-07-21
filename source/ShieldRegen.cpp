
#include "ShieldRegen.h"
#include "PhysicalObject.h"
#include "LevelStartup.h"
#include "CharacterModel.h"


void ShieldRegen::init(cugl::ObstacleWorld *world, std::shared_ptr<cugl::Node> debug, LevelStartup *lvl, std::vector<int> &regenHealths, cugl::Vec2 objScale, std::string name, nodesInit_t node,
	cugl::Vec2 shieldOffset, int shapeId, cugl::Size size, std::shared_ptr<Collision> physCol,
	std::shared_ptr<LogicCollision> logicCol, std::shared_ptr<DefendCollision> defCol) {

	_world = world;
	_debugNode = debug;
	_level = lvl;
	_objScale = objScale;
	_shieldNode = node;
	_name = name;

	for (int i = 0; i < regenHealths.size(); i++) {
		_regenHealths.push_back(regenHealths[i]);
	}

	// remember how to remake shield
	_shieldOffset = shieldOffset;
	_size = size;
	_shapeId = shapeId;
	_physCol = physCol;
	_logicCol = logicCol;
	_defCol = defCol;
}

void ShieldRegen::doLogic(do_logic_args_t *args) {
	PhysicalObject *physObj = (PhysicalObject*)args->obj;

	for (int i = 0; i < _regenHealths.size(); i++) {
		if (physObj->getCurrentHealth() <= _regenHealths[i]) {

			/*//assert(physObj->getBody(1) == nullptr);
			physObj->createAddHitbox(_shieldOffset, _size, _shapeId, _physCol, _logicCol, _defCol);
			//physObj->setupHitboxes();
			physObj->activateNewHitbox(1, _world); //TODO magic number
			physObj->setIdx(physObj->getIdx());
			physObj->setVersion(physObj->getVersion());
			physObj->setDebugScene(_debugNode); // not working
			_level->AppendNodeToObject(physObj, _objScale, _shieldNode, _name);*/

			physObj->reactivateHitbox(1);

			_regenHealths.erase(_regenHealths.begin() + i);
			break;
		}
	}
}