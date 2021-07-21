/** Class for working with objects that interact with scene graph and box2d */

#include "PhysicalObject.h"
#include "Constants.h"

using namespace cugl;

// Default physics values
/** The density of this rocket */
#define DEFAULT_DENSITY 1.0f
/** The friction of this rocket */
#define DEFAULT_FRICTION 0.1f
/** The restitution of this rocket */
#define DEFAULT_RESTITUTION 0.4f


#pragma mark -
#pragma mark Constructors

/**
* Initializes a new projectile with the given position and size.
*
* The projectile size is specified in world coordinates.
*
* The scene graph is completely decoupled from the physics system.
* The node does not have to be the same size as the physics body. We
* only guarantee that the scene graph node is positioned correctly
* according to the drawing scale.
*
* @param  pos      Initial position in world coordinates
* @param  size   	The dimensions of the box.
*
* @return  true if the obstacle is initialized properly, false otherwise.
*/
bool PhysicalObject::init(const Vec2& pos, const Size& size) {
	//BoxObstacle::init(pos, size);//TODO inherit Obstacle, so can have different types

	// create small box to represent center of object. hitboxes are specfied as offset to this body
	//_physBody = BoxObstacle::alloc(pos, Vec2(0.001, 0.001));
	//MultiHitbox::init(pos);

	MultiHitbox::init(createHitbox(pos, size));
    //cugl::Timestamp _idleTime;
    //_lastAttackTime.mark();

	//setName(physName); //unique?

	/*_objectNode = nullptr;

	isDestructable = false;
	_collisionDamage = 0;


	// set delegations to defaults
	_movementDelegate = std::make_shared<Movement>();
	_physCollisionDelegate = std::make_shared<Collision>();
	_logicCollisionDelegate = std::make_shared<LogicCollision>();

	return true;*/

	return initFunctionality();
}

bool PhysicalObject::initFunctionality() {
	//CULog("init functi\n");

	_objectNode = nullptr;

	isDestructable = false;
	_collisionDamage = 0;
	_healthLostThisFrame = 0;
    
	_permissionToDie = true;
	_permissionToMoveOn = true;

    // this is for projectile attacks
    _attackDelegation = std::shared_ptr<Attack>();
    //_idleTime.mark();


	// set delegations to defaults
	_movementDelegate = std::make_shared<Movement>();
	_physCollisionDelegate = std::make_shared<Collision>();
	_logicCollisionDelegate = std::make_shared<LogicCollision>();
	_defenseDelegate = std::make_shared<DefendCollision>();
	_AIDelegate = nullptr;

	_isPlayer = false;

	_lastZone = nullptr;
	_lastCollisionTime.mark();


	return true;

}


bool PhysicalObject::initFromList(hitboxes_t *hitboxes) {
	assert(hitboxes->offsets.size() > 0);
	assert(hitboxes->offsets.size() == hitboxes->sizes.size());

	// for now force initial hitbox to be in center of object
	assert(hitboxes->offsets[0].isNearZero());

	std::shared_ptr<SimpleObstacle> centerHitbox;
	if (hitboxes->shapeType[0] == ELLIPSE_HITBOX) {
		centerHitbox = createHitEllipse(hitboxes->center, hitboxes->sizes[0]);
	}
	else {
		centerHitbox = createHitbox(hitboxes->center, hitboxes->sizes[0]);
	}
	MultiHitbox::init(centerHitbox);

	for (int i = 1; i < hitboxes->offsets.size(); i++) {
		CULog("extra hitbox\n");
		createAddHitbox(hitboxes->offsets[i], hitboxes->sizes[i], hitboxes->shapeType[i],
			hitboxes->physCol[i], hitboxes->logicCol[i], hitboxes->defCol[i]);
	}
	
	return initFunctionality();
}



/**
* Disposes all resources and assets of this projectile
*
* Any assets owned by this object will be immediately released.  Once
* disposed, a projectile may not be used until it is initialized again.
*/
void PhysicalObject::dispose() {
	_objectNode = nullptr;
	MultiHitbox::dispose();
}

// movement, attack, animation, and AI control should happen here
void PhysicalObject::updateObject(float dt) {

}

void PhysicalObject::updateTexture(bool newAttack, bool levelExit, std::shared_ptr<ActionManager> actions, float dt) {
	cugl::Timestamp currTime;
	currTime.mark();
	auto timeSinceLastAction = 0;

	/*animState state;
	if (timeSinceLastAction >= IDLE_DURATION_MS)
	state = animState::IDLE;
	else
	state = animState::SHOOT;*/

	std::shared_ptr<cugl::AnimationNode> associatedNodes[MAX_NODES_PER_HITBOX];



	for (int i = 0; i < _anims.size(); i++) {
		// create nodes package to pass to delegate
		auto nodes = getAnimationNodes(_anims[i].hitboxIdx);
		for (int j = 0; j < _anims[i].nodeIdxs.size(); j++) {
			int nodeIdx = _anims[i].nodeIdxs[j];
			associatedNodes[j] = nodes[nodeIdx];
		}

		anim_args_t args;
		args.obj = (void*)this;
		args.actions = actions;
		args.dt = dt;
		args.newAttack = newAttack;
		args.levelExit = levelExit;
		args.orientation = 0;
		args.timeSinceLastAction = timeSinceLastAction;
		args.nodes = associatedNodes;

		_anims[i].animDelegate->updateTexture(&args);
	}

	// TODO move this to a more general update routine
	resetHealthLostThisFrame();

}

#pragma mark -
#pragma mark Physics
/**
* Applies the force to the body of this ship
*
* This method should be called after the force attribute is set.
*/
void PhysicalObject::applyForce() {
	if (!isActive()) {
		return;
	}

	// Orient the force with rotation.
	Vec4 netforce(_force.x, _force.y, 0.0f, 1.0f);

	// this transformation was messing up enemy movement, possible b/c we don't set angle right?
	/*Mat4::createRotationZ(getAngle(), &_affine);
	netforce *= _affine;*/

	// Apply force to the rocket BODY, not the rocket
	//_body->ApplyForceToCenter(b2Vec2(netforce.x, netforce.y), true);
	ApplyForceToCenter(b2Vec2(netforce.x, netforce.y), true);
}


/**
* Updates the object's physics state (NOT GAME LOGIC).
*
* This method is called AFTER the collision resolution state. Therefore, it
* should not be used to process actions or any other gameplay information.
* Its primary purpose is to adjust changes to the fixture, which have to
* take place after collision.
*
* In other words, this is the method that updates the scene graph.  If you
* forget to call it, it will not draw your changes.
*
* @param delta Timing values from parent loop
*/
//void PhysicalObject::update(float delta) {
	/*Obstacle::update(delta);

	if (_objectNode != nullptr) {
		std::vector<std::shared_ptr<Node>> children = _objectNode->getChildren();
		//begin at index 1 since body 0 does not correspond to a node
		//given how we populate the children in setSceneNode
		int i = 0;
		//CULog("update0\n");
		// Update the nodes of the attached bodies
		for (auto it = children.begin(); it < children.end(); it++) {
			//CULog("update\n");
			(*it)->setPosition(_bodies[i]->getPosition()*_drawscale);
			(*it)->setAngle(_bodies[i]->getAngle());

			// Propagate the update to the bodies attached to the RopeBridge
			_bodies[i]->update(delta);
			i++;
		}
	}*/
	/*Obstacle::update(delta);
	if (_objectNode != nullptr) {
		_objectNode->setPosition(getPosition()*_drawscale);
		_objectNode->setAngle(getAngle());
	}*/

	/*for (auto it = children.begin(); it != children.end(); ++it) {
		(*it)->setPosition(_bodies[i]->getPosition()*_drawScale);
		(*it)->setAngle(_bodies[i]->getAngle());

		// Propagate the update to the bodies attached to the RopeBridge
		_bodies[i]->update(delta);
		i++;
	}*/

//}

#pragma mark -
#pragma mark Animation
/**
* Sets the scene graph node representing this rocket.
*
* By storing a reference to the scene graph node, the model can update
* the node to be in sync with the physics info. It does this via the
* {@link Obstacle#update(float)} method.
*
* If the animation nodes are not null, this method will remove them from
* the previous scene and add them to the new one.
*
* @param node  The scene graph node representing this rocket.
*/
void PhysicalObject::setSceneGraphNode(std::shared_ptr<cugl::AnimationNode>& node) {
	_objectNode = node;
	//CULog("set node %d\n", (int)_bodies.size());

	/*// these nodes are set as children to _objectNode, they should move as object node moves
	for (int ii = 0; ii < _bodies.size(); ii++) {
		CULog("set children\n");

		std::shared_ptr<Node> sprite = Node::alloc();
		//sprite->setScale(node->getScale());
		sprite->setPosition(_bodies[ii]->getPosition() * _drawscale);
		_objectNode->addChild(sprite);
	}*/
}

void PhysicalObject::addSceneGraphNode(std::shared_ptr<cugl::AnimationNode>& node, cugl::Vec2 physOffset, int hitboxIdx) {
	// TODO polish
	if (hitboxIdx == 0) {
		setSceneGraphNode(node);
	}

	appendSceneGraphNode(hitboxIdx, node, physOffset, _drawscale);

	/*if (_hitboxes.size() > hitboxIdx) {
		_hitboxes[hitboxIdx]->gfxNode = node;
	}*/
}

/**
* Sets the ratio of the ship sprite to the physics body
*
* The rocket needs this value to convert correctly between the physics
* coordinates and the drawing screen coordinates.  Otherwise it will
* interpret one Box2D unit as one pixel.
*
* All physics scaling must be uniform.  Rotation does weird things when
* attempting to scale physics by a non-uniform factor.
*
* @param scale The ratio of the ship sprite to the physics body
*/
void PhysicalObject::setDrawScale(float scale) {
	_drawscale = scale;
	if (_objectNode != nullptr) {
		_objectNode->setPosition(getPosition()*_drawscale);
	}
}


#pragma mark -
#pragma mark Delegation

/*void PhysicalObject::delegateMovement(
	std::function<void(bool, cugl::Vec2, PhysicalObject *, cugl::Vec2 *)> movementDel) {

	_delegations.movementHandler = movementDel;

}*/

void PhysicalObject::delegateMovement(std::shared_ptr<Movement> del) {
	_movementDelegate = del;

	assert(_movementDelegate != nullptr);
	//CULog("delegate\n");
}

//override to do texture as well
void PhysicalObject::doMovement(bool isNewResp, cugl::Vec2 actionVec, cugl::Vec2 *returnVec) {
	//_delegations.movementHandler(isNewResp, actionVec, this, returnVec);
	//CULog("start move\n");
	_movementDelegate->move((void*)this, isNewResp, actionVec, returnVec);
}




void PhysicalObject::delegatePhysCollision(std::shared_ptr<Collision> del) {
	//_delegations.collisionHandler = collisionDel;
	_physCollisionDelegate = del;
}

void PhysicalObject::delegateLogicCollision(std::shared_ptr<LogicCollision> del) {
	_logicCollisionDelegate = del;
}

void PhysicalObject::delegateDefense(std::shared_ptr<DefendCollision> del) {
	_defenseDelegate = del;
}

void PhysicalObject::delegateAnimation(std::shared_ptr<Animation> del, int hitboxIdx, int *associatedNodes, int numNodes) {
	animPack_t pack;
	pack.hitboxIdx = hitboxIdx;
	for (int i = 0; i < numNodes; i++) {
		pack.nodeIdxs.push_back(associatedNodes[i]);
	}
	pack.animDelegate = del;

	_anims.push_back(pack);
	//_animDelegates.push_back(del); 
}


void PhysicalObject::delegateUpdateLogic(std::shared_ptr<UpdateLogic> del) {
	_updateLogicDelegates.push_back(del);
}


//TODO bad
void PhysicalObject::setHitboxCol(cugl::SimpleObstacle *hitbox) {
	_lastHitboxCol = hitbox;
}

void PhysicalObject::deactivateLastHitbox() {
	deactivateHitbox(_lastHitboxCol);
}

void PhysicalObject::onCollide(PhysicalObject *otherObj, cugl::SimpleObstacle *thisHitbox,
	cugl::SimpleObstacle *otherHitbox, cugl::Vec2 normalVec, bool noPhysCollision, unsigned long long frameNum) {
	//CULog("collision %s\n", this->getName().c_str());

	/*// if a certain amount of time has passed, forgot about a previously logged collision (if we had one)
	cugl::Timestamp currTime;
	currTime.mark();
	auto timeSinceLastCollision = cugl::Timestamp::ellapsedMillis(_lastCollisionTime, currTime);

	if (timeSinceLastCollision > COLLISION_RESET_TIME) {
		_lastZone = nullptr;
		_lastCollidedObject = nullptr; // may want to have seperate timeers?
	}*/

	// make sure there are no recollisions
	if (/*(_lastLoggedFrame >= frameNum - 1) &&*/ (_lastCollidedObject == otherObj)) {
		_lastCollisionTime.mark();
		return;
	}
	
	// update that collision has happened
	_lastCollisionTime.mark();



	_lastLoggedFrame = frameNum;
	_lastCollidedObject = otherObj;


	// check for hitbox specific collision behavior
	MultiHitbox::onCollide(thisHitbox, (void*)this, otherObj, normalVec, noPhysCollision);

	// do behavior for character

	if (!noPhysCollision) {
		_physCollisionDelegate->onCollide((void*)this, normalVec);
	}
	_logicCollisionDelegate->onCollide((void*)this, (void*)otherObj, normalVec, noPhysCollision);

	
	
}

bool PhysicalObject::doCollide(PhysicalObject *otherObj) {

	// check colliding hitbox 
	int hitboxIdx = findHitboxIdx(_lastHitboxCol);
	bool doPhysCol = _hitboxes[hitboxIdx]->physCol->doCollide((void*)this, (void*)otherObj);
	bool doLogicCol = _hitboxes[hitboxIdx]->logicCol->doCollide((void*)this, (void*)otherObj);

	if (!doPhysCol || !doLogicCol) return false;

	// check object as a whole
	doPhysCol = _physCollisionDelegate->doCollide((void*)this, (void*)otherObj);
	doLogicCol = _logicCollisionDelegate->doCollide((void*)this, (void*)otherObj);

	if (!doPhysCol || !doLogicCol) return false;

	return true;
	
}

/*void PhysicalObject::procPhysCollisions() {
	_physCollisionDelegate->procAllCollisions((void*)this);
}*/

void PhysicalObject::onAttacked(int damage, cugl::Vec2 velocity, bool shieldBreak) {
	bool allowMoreDmg = MultiHitbox::onAttacked((void*)this, _lastHitboxCol, damage, velocity, shieldBreak);

	if (allowMoreDmg) {
		_defenseDelegate->onAttacked((void*)this, damage, velocity, shieldBreak);
	}
}


#pragma mark -
#pragma mark Hitbox
#define DYNAMIC_COLOR   Color4::GREEN

// create shape once and then never change the geometry!
std::shared_ptr<cugl::SimpleObstacle> PhysicalObject::createHitbox(cugl::Vec2 pos, const cugl::Size &size) {
	//CULog("create hitbox\n");

	std::shared_ptr<BoxObstacle> newbox = BoxObstacle::alloc(pos, size);

	//_hitbox->setBodyType(b2_dynamicBody);
	newbox->setDensity(DEFAULT_DENSITY);
	newbox->setFriction(DEFAULT_FRICTION);
	newbox->setRestitution(DEFAULT_RESTITUTION);
	newbox->setFixedRotation(true);

	return newbox;
	//addHitbox(newbox, pos);
	//_hitbox = newbox;

}


std::shared_ptr<cugl::SimpleObstacle> PhysicalObject::createHitEllipse(cugl::Vec2 pos, const cugl::Size &size) {
	//CULog("create hitbox\n");

	Poly2 ellipse = Poly2::createEllipse(pos, size, CHARACTER_ELLIPSE_SEGS, true);
	std::shared_ptr<PolygonObstacle> newbox = PolygonObstacle::alloc(ellipse);

	//_hitbox->setBodyType(b2_dynamicBody);
	newbox->setDensity(DEFAULT_DENSITY);
	newbox->setFriction(DEFAULT_FRICTION);
	newbox->setRestitution(DEFAULT_RESTITUTION);
	newbox->setFixedRotation(true);

	return newbox;
	//addHitbox(newbox, pos);
	//_hitbox = newbox;

}

// consider add listener
void PhysicalObject::createAddHitbox(cugl::Vec2 offsetPos, const cugl::Size &size, int shapeId,
	std::shared_ptr<Collision> physCol, std::shared_ptr<LogicCollision> logicCol,
	std::shared_ptr<DefendCollision> defCol) {
	//cugl::Vec2 absolutePos = _hitboxes[0]->getPosition() + offsetPos;
	cugl::Vec2 absolutePos = getPosition() + offsetPos;


	// need to set upon creation (can't modify after created b/c don't remember how to reconstruct the shape)
	//auto hitbox = createHitbox(absolutePos, size);
	std::shared_ptr<SimpleObstacle> hitbox;
	if (shapeId == ELLIPSE_HITBOX) {
		hitbox = createHitEllipse(absolutePos, size);
	}
	else { // STANDARD_HITBOX
		hitbox = createHitbox(absolutePos, size);
	}

	//addHitbox(nullptr, hitbox, offsetPos);
	if (physCol == nullptr) {
		physCol = std::make_shared<Collision>();
	}
	if (logicCol == nullptr) {
		logicCol = std::make_shared<LogicCollision>();
	}
	if (defCol == nullptr) {
		defCol = std::make_shared<DefendCollision>();
	}

	addHitbox(hitbox, offsetPos, physCol, logicCol, defCol);

}

void PhysicalObject::delegateAttack(std::shared_ptr<Attack> del) {
    _attackDelegation = del;
}

const attackInfo_t *PhysicalObject::doAttack(bool attackReq, cugl::Vec2 attackDir, int *numAttacks) {
    return _attackDelegation->attack(this, attackReq, attackDir, numAttacks);

}

void PhysicalObject::processBehavior(AI_args_t *args) {
	if (_AIDelegate != nullptr) {
		_AIDelegate->processBehavior(args);
	}
}

void PhysicalObject::updateLogic(do_logic_args_t *args) {
	args->obj = (void*)this;

	// if a certain amount of time has passed, forgot about a previously logged collision (if we had one)
	cugl::Timestamp currTime;
	currTime.mark();
	auto timeSinceLastCollision = cugl::Timestamp::ellapsedMillis(_lastCollisionTime, currTime);

	if (timeSinceLastCollision > COLLISION_RESET_TIME) {
		_lastZone = nullptr;
		_lastCollidedObject = nullptr; // may want to have seperate timeers?
	}

	for (int i = 0; i < _updateLogicDelegates.size(); i++) {
		_updateLogicDelegates[i]->doLogic(args);
	}
}