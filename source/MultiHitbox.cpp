
#include "MultiHitbox.h"
#include "Constants.h"
#include "Box2D/Dynamics/b2World.h"

// Default physics values
/** The density of this rocket */
#define DEFAULT_DENSITY 1.0f
/** The friction of this rocket */
#define DEFAULT_FRICTION 0.1f
/** The restitution of this rocket */
#define DEFAULT_RESTITUTION 0.4f

#define DUMMY_SIZE cugl::Vec2(0.1, 0.1)

#define DYNAMIC_COLOR   Color4::GREEN

#define CENTER_HITBOX _hitboxes[0]->hitbox

/*void MultiHitbox::init(cugl::Vec2 pos) {
	_physBody = cugl::BoxObstacle::alloc(pos, DUMMY_SIZE);
}*/
void MultiHitbox::init(std::shared_ptr<cugl::SimpleObstacle> centerHitbox) {
	/*_hitboxes.push_back(centerHitbox);
	_offsets.push_back(cugl::Vec2::ZERO);*/

	std::shared_ptr<Collision> physCol = std::make_shared<Collision>();
	std::shared_ptr<LogicCollision> logicCol = std::make_shared<LogicCollision>();
	std::shared_ptr<DefendCollision> defCol = std::make_shared<DefendCollision>();

	addHitbox(centerHitbox, cugl::Vec2::ZERO, physCol, logicCol, defCol);
}

// create shape once and then never change the geometry!
// specify location from center hitbox (i.e. _hitbox[0]
//void MultiHitbox::addHitbox(b2PolygonShape *shape, std::shared_ptr<cugl::SimpleObstacle> newHitbox, cugl::Vec2 hitboxOffset){
void MultiHitbox::addHitbox(std::shared_ptr<cugl::SimpleObstacle> newHitbox, cugl::Vec2 hitboxOffset,
							std::shared_ptr<Collision> physCol, std::shared_ptr<LogicCollision> logicCol,
							std::shared_ptr<DefendCollision> defCol) {
	std::shared_ptr<hitbox_t> hitbox = std::make_shared<hitbox_t>();
	hitbox->hitbox = newHitbox;
	hitbox->offset = hitboxOffset;
	hitbox->physCol = physCol;
	hitbox->logicCol = logicCol;
	hitbox->defendCol = defCol;

	//CULog("hitbox offset %f %f\n", newHitbox->getPosition().x, newHitbox->getPosition().y);

	_hitboxes.push_back(hitbox);

	/*_hitboxes.push_back(newHitbox);
	_offsets.push_back(hitboxOffset);*/
}

void MultiHitbox::appendSceneGraphNode(int hitboxIdx, std::shared_ptr<cugl::AnimationNode> node, cugl::Vec2 offset, float drawscale) {
	if (_hitboxes.size() > hitboxIdx) {
		_hitboxes[hitboxIdx]->gfxNodes.push_back(node);
		_hitboxes[hitboxIdx]->nodePhysOffsets.push_back(offset);

		int size = _hitboxes[hitboxIdx]->gfxNodes.size();
		cugl::Node *nodes[MAX_NODES_PER_HITBOX];
		cugl::Vec2 offsets[MAX_NODES_PER_HITBOX];
		for (int i = 0; i < size; i++) { // reduce memory of callback?
			nodes[i] = _hitboxes[hitboxIdx]->gfxNodes[i].get();
			offsets[i] = _hitboxes[hitboxIdx]->nodePhysOffsets[i];
		}

		// set up callback so node updates when physics hitbox moves
		_hitboxes[hitboxIdx]->hitbox->setListener([=](cugl::Obstacle* obs) {
			for (int i = 0; i < size; i++) {
				nodes[i]->setPosition((obs->getPosition() + offsets[i])*drawscale);
			}
		});

		/*	Node* weak = gameNode.get(); // No need for smart pointer in callback
		physBody->setListener([=](Obstacle* obs) {
			weak->setPosition(obs->getPosition()*_scale);
		});*/
	}


}

std::vector<std::shared_ptr<cugl::AnimationNode>> MultiHitbox::getAnimationNodes(int hitboxIdx) {
	if (_hitboxes.size() <= hitboxIdx) {
		CULog("error\n");
	}
	return _hitboxes[hitboxIdx]->gfxNodes;

}

void MultiHitbox::setupHitboxes(std::shared_ptr<cugl::ObstacleWorld> world) {
	/*for (int i = 0; i < _shapes.size(); i++) {
		//b2FixtureDef *fixtureDef = _fixtures[i].get();
		b2FixtureDef fixtureDef;
		fixtureDef.shape = _shapes[i].get();
		CENTER_HITBOX->getBody()->CreateFixture(&fixtureDef);
	}*/
	//setDensity(DEFAULT_DENSITY);

	world->addObstacle(CENTER_HITBOX);
	auto body1 = CENTER_HITBOX->getBody();

	for (int i = 1; i < _hitboxes.size(); i++) {
		appendHitbox(i, world.get());
		/*world->addObstacle(_hitboxes[i]->hitbox);
		b2DistanceJointDef jointDef; // fixed distance
		//b2WeldJointDef jointDef;
		//jointDef.Initialize(myBodyA, myBodyB, worldAnchorOnBodyA, worldAnchorOnBodyB);
		auto body2 = _hitboxes[i]->hitbox->getBody();
		jointDef.Initialize(body1, body2, body1->GetPosition(), body2->GetPosition()); //worldAnchorOnBodyA, worldAnchorOnBodyB);
		//jointDef.Initialize(body1, body2, body1->GetPosition());
		jointDef.collideConnected = false;
		jointDef.dampingRatio = 1.0f;
		//jointDef.length = _hitboxes[i]->offset.length();
		//jointDef.dampingRatio = 1.0f; // do not ossiclate
		b2Joint *joint = world->getWorld()->CreateJoint(&jointDef);

		_hitboxes[i]->joint = joint;*/
	}

	/*for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setPosition(CENTER_HITBOX->getPosition() + _hitboxes[i]->offset);
	}*/
}

void MultiHitbox::appendHitbox(int i, cugl::ObstacleWorld *world) {
	auto body1 = CENTER_HITBOX->getBody();

	world->addObstacle(_hitboxes[i]->hitbox);
	b2DistanceJointDef jointDef; // fixed distance
								 //b2WeldJointDef jointDef;
								 //jointDef.Initialize(myBodyA, myBodyB, worldAnchorOnBodyA, worldAnchorOnBodyB);
	auto body2 = _hitboxes[i]->hitbox->getBody();
	jointDef.Initialize(body1, body2, body1->GetPosition(), body2->GetPosition()); //worldAnchorOnBodyA, worldAnchorOnBodyB);
																				   //jointDef.Initialize(body1, body2, body1->GetPosition());
	jointDef.collideConnected = false;
	jointDef.dampingRatio = 1.0f;
	//jointDef.length = _hitboxes[i]->offset.length();
	//jointDef.dampingRatio = 1.0f; // do not ossiclate
	b2Joint *joint = world->getWorld()->CreateJoint(&jointDef);

	_hitboxes[i]->joint = joint;
}

void MultiHitbox::adjustHitboxes() {
	for (int i = 1; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setPosition(CENTER_HITBOX->getPosition() + _hitboxes[i]->offset); //TODO needs to be relative to local coordinate space
	}
}

// TODO better scene graph removal + merge with single delete
void MultiHitbox::removeHitboxes(std::shared_ptr<cugl::ObstacleWorld> world,
							std::shared_ptr<cugl::Node> worldNode) {

	// remove joints (first hitboxes does not associate a joint
	for (int i = 1; i < _hitboxes.size(); i++) {
		if (_hitboxes[i]->joint != nullptr) {
			world->getWorld()->DestroyJoint(_hitboxes[i]->joint);
		}
	}

	// remove bodies
	for (int i = 0; i < _hitboxes.size(); i++) {
		world->removeObstacle(_hitboxes[i]->hitbox.get());
	}

	for (int i = 0; i < _hitboxes.size(); i++) {
		for (int j = 0; j < _hitboxes[i]->gfxNodes.size(); j++) {
			if (_hitboxes[i]->gfxNodes[j] != nullptr) {
				//_hitboxes[i]->gfxNode->setVisible(false);
				worldNode->removeChild(_hitboxes[i]->gfxNodes[j]);
				_hitboxes[i]->gfxNodes[j] = nullptr;
			}
		}

		_hitboxes[i]->gfxNodes.clear();
		_hitboxes[i]->gfxNodes.shrink_to_fit();

		_hitboxes[i]->nodePhysOffsets.clear();
		_hitboxes[i]->nodePhysOffsets.shrink_to_fit();
	}


	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i] = nullptr;
	}
	_hitboxes.shrink_to_fit();

	int numDeletes = _hitboxes.size();
	for (int i = 0; i < numDeletes; i++) {
		_hitboxes.erase(_hitboxes.begin());
	}


	//_hitboxes.clear();
	_hitboxes.shrink_to_fit();


}

void MultiHitbox::deactivateHitbox(cugl::SimpleObstacle *hitbox) {
	//CULog("deactivate hitbox\n");


	int hitboxIdx = findHitboxIdx(hitbox);

	_hitboxDeactQueue.push(hitboxIdx);
	//_hitboxes[hitboxIdx]->hitbox->setActive(false); // can't do during collision
	//_hitboxes[hitboxIdx]->gfxNode->setVisible(false);
}

void MultiHitbox::procDeactivations(std::shared_ptr<cugl::ObstacleWorld> world, 
									std::shared_ptr<cugl::Node> worldNode) {
	while (_hitboxDeactQueue.size() != 0) {
		//CULog("delete hitbox\n");
		int toRemove = _hitboxDeactQueue.front();
		_hitboxDeactQueue.pop();


		for (int i = 0; i < _hitboxes[toRemove]->gfxNodes.size(); i++) {
			_hitboxes[toRemove]->gfxNodes[i]->setVisible(false);
		}

		_hitboxes[toRemove]->hitbox->setActive(false);

		/*world->getWorld()->DestroyJoint(_hitboxes[toRemove]->joint);
		_hitboxes[toRemove]->hitbox->setDebugScene(nullptr);
		_hitboxes[toRemove]->hitbox->deactivatePhysics(*world->getWorld());
		_hitboxes[toRemove]->joint = nullptr;
		_hitboxes[toRemove]->hitbox = nullptr;


		for (int j = 0; j < _hitboxes[toRemove]->gfxNodes.size(); j++) {
			if (_hitboxes[toRemove]->gfxNodes[j] != nullptr) {
				worldNode->removeChild(_hitboxes[toRemove]->gfxNodes[j]);
				_hitboxes[toRemove]->gfxNodes[j] = nullptr;
			}
		}

		_hitboxes[toRemove]->gfxNodes.clear();
		_hitboxes[toRemove]->gfxNodes.shrink_to_fit();

		_hitboxes[toRemove]->nodePhysOffsets.clear();
		_hitboxes[toRemove]->nodePhysOffsets.shrink_to_fit();

		_hitboxes[toRemove] = nullptr;



		_hitboxes.erase(_hitboxes.begin() + toRemove);*/


		
	}
}

bool MultiHitbox::reactivateHitbox(int hitboxIdx) {
	for (int i = 0; i < _hitboxes[hitboxIdx]->gfxNodes.size(); i++) {
		_hitboxes[hitboxIdx]->gfxNodes[i]->setVisible(true);
	}

	_hitboxes[hitboxIdx]->hitbox->setActive(true);

	return true;
}

int MultiHitbox::findHitboxIdx(cugl::SimpleObstacle *hitbox) {
	// find hitbox id
	int hitboxIdx = 0;
	for (int i = 0; i < _hitboxes.size(); i++) {
		if (hitbox == _hitboxes[i]->hitbox.get()) {
			hitboxIdx = i;
		}
	}

	return hitboxIdx;
}

// call on collid for each individual hitbox, then call oncollide for model
// TODO store hitbox id in cugl
// BAD
void MultiHitbox::onCollide(cugl::SimpleObstacle *hitbox, void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision) {
	//PhysicalObject *physObj = (PhysicalObject*)obj;
	//PhysicalObject *otherPhysObj = (PhysicalObject*)otherObj;

	
	int hitboxIdx = findHitboxIdx(hitbox);
	

	//CULog("multihitbox collide %d\n", hitboxIdx);

	// call for individual hitbox
	if (!noPhysCollision) {
		_hitboxes[hitboxIdx]->physCol->onCollide(obj, normalVec);
	}
	_hitboxes[hitboxIdx]->logicCol->onCollide(obj, otherObj, normalVec, noPhysCollision);


}

bool MultiHitbox::onAttacked(void* obj, cugl::SimpleObstacle *hitbox, int damage, cugl::Vec2 velocity, bool shieldBreak) {
	// find hitbox id
	int hitboxIdx = findHitboxIdx(hitbox);
	
	return _hitboxes[hitboxIdx]->defendCol->onAttacked(obj, damage, velocity, shieldBreak);
}



// define physics functions by accessing hitbox
void MultiHitbox::setDensity(float density) { 
	//_physBody->setDensity(density);
	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setDensity(density);
	}
}
void MultiHitbox::setFriction(float friction) {
	//_physBody->setFriction(friction);
	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setFriction(friction);
	}
}
void MultiHitbox::setRestitution(float rest) {
	//_physBody->setRestitution(rest);
	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setRestitution(rest);
	}
}
void MultiHitbox::setFixedRotation(bool isFixed) {
	//_physBody->setFixedRotation(isFixed);
	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setFixedRotation(isFixed);
	}
}
bool MultiHitbox::isActive() { return CENTER_HITBOX->isActive(); }

// TODO
cugl::Vec2 MultiHitbox::getPosition() {
	return CENTER_HITBOX->getPosition();
}
void MultiHitbox::setPosition(cugl::Vec2 pos) {
	//_physBody->setPosition(pos);
	//CULog("set position\n");

	CENTER_HITBOX->setPosition(pos);
	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setPosition(pos + _hitboxes[i]->offset);
	}
}
float MultiHitbox::getAngle() { return CENTER_HITBOX->getAngle(); }
void MultiHitbox::setLinearVelocity(cugl::Vec2 vel) {
	CENTER_HITBOX->setLinearVelocity(vel);
	//_physBody->setLinearVelocity(vel);
	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setLinearVelocity(vel);
	}
}
cugl::Vec2 MultiHitbox::getVelocity() { return CENTER_HITBOX->getLinearVelocity(); }


void MultiHitbox::setAngle(float angle) {
	//_physBody->setAngle(angle);
	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setAngle(angle);
	}
}
void MultiHitbox::setBullet(bool isBullet) {
	//_physBody->setBullet(isBullet);
	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setBullet(isBullet);
	}
}

int MultiHitbox::getIdx() { return CENTER_HITBOX->getIdx(); }
int MultiHitbox::getVersion() { return CENTER_HITBOX->getVersion(); }
void MultiHitbox::setIdx(int idx) {
	//_physBody->setIdx(idx);
	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setIdx(idx);
	}
}
void MultiHitbox::setVersion(int version) {
	//_physBody->setVersion(version);
	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setVersion(version);
	}
}

std::string MultiHitbox::getName() { return CENTER_HITBOX->getName(); }
void MultiHitbox::setName(std::string name) {
	//_physBody->setName(name);
	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setName(name);
	}
}

void MultiHitbox::setDebugScene(const std::shared_ptr<cugl::Node> &node) {
	//_physBody->setDebugScene(node);
	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setDebugScene(node);
	}
}
void MultiHitbox::setDebugColor(cugl::Color4 color) {
	//_physBody->setDebugColor(color);
	for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->setDebugColor(color);
	}
}

// TODO BAd with welded bodies
void MultiHitbox::ApplyForceToCenter(b2Vec2 forceVec, bool wakeup) {
	CENTER_HITBOX->getBody()->ApplyForceToCenter(forceVec, wakeup);
	/*for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i]->hitbox->getBody()->ApplyForceToCenter(forceVec, wakeup);
	}*/
}

/*std::vector<std::shared_ptr<cugl::SimpleObstacle>> MultiHitbox::getHitbox() { 
	return _hitboxes; 
}*/

// just set center as static
void MultiHitbox::setBodyType(b2BodyType type) {
	CENTER_HITBOX->setBodyType(type);
}

void MultiHitbox::dispose() {
	//CULog("try to dispose %d\n", _hitboxes.size());

	/*for (int i = 0; i < _hitboxes.size(); i++) {
		_hitboxes[i] = nullptr;
	}*/

	_hitboxes.clear();
	_hitboxes.shrink_to_fit();

	while (_hitboxDeactQueue.size() > 0) {
		_hitboxDeactQueue.pop();
	}
}