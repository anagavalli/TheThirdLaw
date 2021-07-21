#include "Polyzone.h"

#define STATIC_COLOR    Color4::YELLOW

#define BASIC_DENSITY       0.0f
#define BASIC_FRICTION      0.1f
#define BASIC_RESTITUTION   0.1f


using namespace cugl;



bool Polyzone::init(const cugl::Vec2& pos, const cugl::Size& size) {
	// init parent (should put some functionality into here from outside)
	//PhysicalObject::init(pos, size);
	//MultiHitbox::init(pos);
	MultiHitbox::init(createHitbox(pos, size));
	// need to delegate since don't call physical object
	// TODO: integrate into physicalObject
	_movementDelegate = std::make_shared<Movement>();
	_physCollisionDelegate = std::make_shared<Collision>();
	_logicCollisionDelegate = std::make_shared<LogicCollision>();
	_defenseDelegate = std::make_shared<DefendCollision>();

	_objectNode = nullptr;

	isDestructable = false;

	return true;
}


bool Polyzone::initWithPoly(const cugl::Poly2 poly, float scale, bool lockImageToMesh) {
	// init parent (should put some functionality into here from outside)
	//PhysicalObject::init(pos, size);
	_poly = poly;
	_lockImageToMesh = lockImageToMesh;

	init(cugl::Vec2::ZERO, cugl::Vec2::ZERO);
	/*MultiHitbox::init(cugl::Vec2::ZERO);
	createHitbox(cugl::Vec2::ZERO, cugl::Vec2::ZERO);
	// need to delegate since don't call physical object
	// TODO: integrate into physicalObject
	_movementDelegate = std::make_shared<Movement>();
	_physCollisionDelegate = std::make_shared<Collision>();
	_logicCollisionDelegate = std::make_shared<LogicCollision>();

	_polyNode = nullptr;

	isDestructable = false;*/

	return true;
}

/*void Polyzone::update(float delta) {
	Obstacle::update(delta);
	if (_polyNode != nullptr) {
		_polyNode->setPosition(getPosition()*_drawscale);
		_polyNode->setAngle(getAngle());
	}
}*/

void Polyzone::setSceneGraphNode(std::shared_ptr<cugl::AnimationNode>& node) {
	_objectNode = node;

	// fill hitbox with wrapped image
	if (_lockImageToMesh) {
		_objectNode->setPolygon(_poly * _drawscale);
	}

	//PhysicalObject::setSceneGraphNode(std::dynamic_pointer_cast<Node, PolygonNode>(_polyNode)); //?
}


/*void Polyzone::addSceneGraphNode(const std::shared_ptr<cugl::AnimationNode>& node, int hitboxIdx) {
	// TODO polish
	if (hitboxIdx == 0) {
		setSceneGraphNode(node);
	}

	if (_hitboxes.size() > hitboxIdx) {
		_hitboxes[hitboxIdx]->gfxNode = node;
	}
}*/

/*void Polyzone::setPolyObstacle(std::shared_ptr<PolygonObstacle> &obs) {
	//_body = obs->getBody();
	//tempPos = obs->getPosition();
}*/

void Polyzone::dispose() {
	PhysicalObject::dispose();
}

std::shared_ptr<cugl::SimpleObstacle> Polyzone::createHitbox(cugl::Vec2 pos, const cugl::Size &size) {
	std::shared_ptr<PolygonObstacle> wallobj = PolygonObstacle::alloc(_poly);
	
	wallobj->setDebugColor(STATIC_COLOR);
	wallobj->setName("TODO");

	// Set the physics attributes
	wallobj->setBodyType(b2_staticBody);
	wallobj->setDensity(BASIC_DENSITY);
	wallobj->setFriction(BASIC_FRICTION);
	wallobj->setRestitution(BASIC_RESTITUTION);

	//_hitbox = wallobj;
	//addHitbox(wallobj);
	return wallobj;
}