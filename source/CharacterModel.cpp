#include "CharacterModel.h"
#include "Animation.h"
#include "StandardDefense.h"

using namespace cugl;

#pragma mark -
#pragma mark Animation and Physics Constants

/** This is adjusted by screen aspect ratio to get the height */
#define GAME_WIDTH 1024

/** The number of frames for the afterburner */
#define FIRE_FRAMES     4

#define PI	3.14159265359f


#pragma mark -
#pragma mark Constructors

// custom init functionality for sub classes (virtual to allow for overriding)
bool CharacterModel::init(const cugl::Vec2& pos, const cugl::Size& size) {
	//call parent constructor
	PhysicalObject::init(pos, size);


	//circle = WheelObstacle::alloc(pos, size.width);
	//_body = circle->getBody();//???


	return initFunctionality();
	/*isDestructable = true;

	_characterNode = nullptr;

	_currFrame = 0;
	_currAnimation = 0;

	// set delegations not set in parent
	_attackDelegation = std::shared_ptr<Attack>();

	// set health, should be move to another function
	_totalHealth = 2;
	_currentHealth = 2;

	return true;*/
}

bool CharacterModel::initFunctionality() {
	isDestructable = true;

	//_characterNode = nullptr;

	//_currFrame = 0;
	//_currAnimation = 0;

	// set delegations not set in parent
	_attackDelegation = std::shared_ptr<Attack>();
	_defenseDelegate = std::make_shared<StandardDefense>();

	// set health, should be move to another function
	_totalHealth = 2;
	_currentHealth = 2;

	_idleTime.mark();
	_idleTime -= IDLE_DURATION_MS; //immediately idle when game starts
	return true;
}

bool CharacterModel::initFromList(hitboxes_t *hitboxes) {
	PhysicalObject::initFromList(hitboxes);

	return initFunctionality();
}

/**
 * Disposes all resources and assets of this rocket
 *
 * Any assets owned by this object will be immediately released.  Once
 * disposed, a rocket may not be used until it is initialized again.
 */
void CharacterModel::dispose() {
	PhysicalObject::dispose();
}


#pragma mark -
#pragma mark Physics

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
/*void RocketModel::update(float delta) {
    Obstacle::update(delta);
    if (_shipNode != nullptr) {
        _shipNode->setPosition(getPosition()*_drawscale);
        _shipNode->setAngle(getAngle());
    }
}*/


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
/*void CharacterModel::setSceneGraphNode(const std::shared_ptr<cugl::AnimationNode>& node) {
    _characterNode = node;

	// keep track of NODE class for resuse
	PhysicalObject::setSceneGraphNode(std::dynamic_pointer_cast<Node, PolygonNode>(_characterNode));
	//_objectNode = std::dynamic_pointer_cast<Node, PolygonNode>(_characterNode);
}

void CharacterModel::addSceneGraphNode(const std::shared_ptr<cugl::AnimationNode>& node, int hitboxIdx) {
	// TODO polish
	if (hitboxIdx == 0) {
		setSceneGraphNode(node);
	}
	

	_hitboxes[hitboxIdx]->gfxNode = node;


}*/

/*//TODO don't use in the future for anything
void CharacterModel::addAnimation(std::string texName, int numFrames, int numOrientations, std::shared_ptr<AssetManager> &assets) {

	// TODO keep track of number of frames in animation filmstrip

	// setup newton orientations
	std::vector<std::shared_ptr<Texture>> orientations;
	for (int i = 0; i < numOrientations; i++) {

		// get all frames for an orientation
		std::string orientationStr = texName + std::to_string(i);
		
			
		std::shared_ptr<Texture> image = assets->get<Texture>(orientationStr.c_str());
		orientations.push_back(image);
		
	}
	
	
	//add vector for animation
	_textures.push_back(orientations);
}*/
// used?
void CharacterModel::setTexture(cugl::Vec2 movementVec) {
	int orientationID = getOrientation(movementVec, 0);

	/*float angleRes;
	if (_omode == NORMAL) {
		angleRes = 2.0f * PI / (float)numOrientations;
	}
	else {
		angleRes = PI / (float)numOrientations;
	}*/

	float angleRes = 2.0f * PI / (float)numOrientations;
	

	_lastOrientation = _currOrientation;

	if (_omode == ROUND_DOWN) {
		// round down to nearest factor of 2
		//CULog("pre orientationID %d\n", orientationID);
		orientationID -= orientationID % 2;
		//CULog("orientationID %d\n", orientationID);
	}
	else if (_omode == ROUND_UP) {
		// round to nearest off-factor of 2
		//CULog("pre orientationID %d\n", orientationID);
		orientationID -= (orientationID + 1) % 2;
		//CULog("orientationID %d\n", orientationID);
	}

	int angle = orientationID;
	angle *= -1;
	setAngle(((float)angle * angleRes));
	_currOrientation = orientationID;


	//TODO
	/*if (!_isPlayer) {
		//_objectNode->setTexture(tex);
		std::shared_ptr<Texture> tex = _textures[_currAnimation][orientationID];

		// getAnimationNodes(hitboxIdx)[nodeIdx]
		getAnimationNodes(0)[0]->setTexture(tex);
	}*/

	
	
}

// should be called after orientation update due to action
/*void CharacterModel::updateTexture(float dt) {
	// TODO
	int orientations = 8;
	int totFrames = 14;

	// check state machine?
	cugl::Timestamp currTime;
	currTime.mark();
	auto timeSinceLastAction = cugl::Timestamp::ellapsedMillis(_idleTime, currTime);

	//CULog("%llu \n", timeSinceLastAction);
	if (timeSinceLastAction >= IDLE_DURATION_MS) {
		_currFrame = (_currFrame + 1) % totFrames;
	}
	else {
		_currFrame = 0;
	}

	int frameID = _currOrientation * totFrames + _currFrame;
	//_objectNode->setFrame(frameID);

	// getAnimationNodes(hitboxIdx)[nodeIdx]
	getAnimationNodes(0)[0]->setFrame(frameID);
}*/

void CharacterModel::updateObject(float dt) {

}

void CharacterModel::updateTexture(bool newAttack, bool levelExit, std::shared_ptr<ActionManager> actions, float dt) {
	cugl::Timestamp currTime;
	currTime.mark();
	auto timeSinceLastAction = cugl::Timestamp::ellapsedMillis(_idleTime, currTime);

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
		args.orientation = _currOrientation;
		args.timeSinceLastAction = timeSinceLastAction;
		args.nodes = associatedNodes;

		_anims[i].animDelegate->updateTexture(&args);
	}

	// TODO move this to a more general update routine
	resetHealthLostThisFrame();

}


int CharacterModel::getOrientation(cugl::Vec2 directionVec, int animationID) {
	//if (numOrientations == 0)
	//	numOrientations = _textures[animationID].size();
	//int numOrientations = getAnimationNodes(0)[0]->getCols();

	// getAngle() is measured from -x axis and goes clockwise. newton model 0 is on -y axis and goes counterclockwise
	float newtonAngle = (PI / 2.0f) - directionVec.getAngle(); //pi/2 - angle

	// get newton angle
	// make angle positive if negative
	if (newtonAngle < 0.0f) {
		float addon = PI + newtonAngle;
		newtonAngle = PI + addon; //2pi + newtonAngle?
	}

	// discretize
	float sector = (2.0f * PI) / numOrientations;

	// don't just round down (i.e. integer cast) b/c will favor certain orientations
	float div = newtonAngle / sector;
	int pic = int(round(div));

	//wrap around
	if (pic == numOrientations) {
		pic = 0;
	}

	//CULog("Angle: %f -> %f %d\n", dispVec.getAngle(), newtonAngle, pic);

	return pic;
}


//override to do texture as well
void CharacterModel::doMovement(bool isNewResp, cugl::Vec2 actionVec, cugl::Vec2 *returnVec) {
	_movementDelegate->move((void*)this, isNewResp, actionVec, returnVec);

	// pseudo-angular movement
	if (isNewResp && _turnOnMove) { // TODO: just set orientation here
		setTexture(*returnVec);
	}

	if (isNewResp) {
		_idleTime.mark();
	}	

	// animation function here or move to another method
}


/*void CharacterModel::delegateAttack(std::shared_ptr<Attack> del) {
	_attackDelegation = del;
}*/

const attackInfo_t *CharacterModel::doAttack(bool attackReq, cugl::Vec2 attackDir, int *numAttacks) {
	/*if (attackReq && _turnOnAttack) {
		_idleTime.mark();
		setTexture(attackDir);
	}
	return _attackDelegation->attack(this, attackReq, attackDir, numAttacks);*/

	auto atk = _attackDelegation->attack(this, attackReq, attackDir, numAttacks);

	if (attackReq && _turnOnAttack && atk->validAttack) {
		_idleTime.mark();
		setTexture(attackDir);
	}
	return atk;

}
