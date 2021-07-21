/*
* Regenerate shield when a certain amount of health is lost 
*/

#include "UpdateLogic.h"
#include "cugl/cugl.h"
#include "Collision.h"
#include "LogicCollision.h"
#include "DefendCollision.h"
#include "LevelStartup.h"

class ShieldRegen : public UpdateLogic {
protected:
	LevelStartup *_level; // TODO remove
	std::shared_ptr<cugl::Node> _debugNode;
	cugl::ObstacleWorld *_world;

	std::vector<int> _regenHealths;

	// parameters of shield hitbox
	cugl::Vec2 _shieldOffset;
	cugl::Size _size;
	int _shapeId;
	std::shared_ptr<Collision> _physCol; 
	std::shared_ptr<LogicCollision> _logicCol;
	std::shared_ptr<DefendCollision> _defCol;


	cugl::Vec2 _objScale;
	nodesInit_t _shieldNode;
	std::string _name;

	/*
	// parameters of shield node
	int _hitboxId = 1; 
	int _numNodes = 1;
	std::string _texture = "shield"; 
	int _filmStripRow = 1; node1.filmStripCol[0] = 1; node1.zOrders[0] = SHIELD_Z_ORDER; node1.physOffsets[0] = Vec2(0.0f, 0.0f); 
	node1.relativeScale[0] = SAME_SCALE * 2;
	*/
public:
	void init(cugl::ObstacleWorld *world, std::shared_ptr<cugl::Node> debug, LevelStartup *lvl, std::vector<int> &regenHealths, cugl::Vec2 objScale, std::string name, nodesInit_t node,
		cugl::Vec2 _shieldOffset, int shapeId, cugl::Size _size, std::shared_ptr<Collision> _physCol, 
	std::shared_ptr<LogicCollision> _logicCol, std::shared_ptr<DefendCollision> _defCol);
	void doLogic(do_logic_args_t *args) override;
};


