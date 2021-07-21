/**
* Superclass for interacting with scene graph and box2d functionality
* Sort of an abstraction layer even above cugl
*/


#ifndef __PHYSICAL_OBJECT_H__
#define __PHYSICAL_OBJECT_H__
#include <cugl/cugl.h>
#include "Movement.h"
#include "Collision.h"
#include "LogicCollision.h"
#include "DefendCollision.h"
#include "MultiHitbox.h"
#include "Attack.h"
#include "Animation.h"
#include "AIControl.h"
#include "UpdateLogic.h"

#define DEFAULT_MOD 1000.0f;


typedef struct hitboxes {
	cugl::Vec2 center;
	std::vector<int> shapeType; // 0 - standard for class, 1 - ellipse
	std::vector<cugl::Size> sizes;
	std::vector<cugl::Vec2> offsets;
	std::vector<std::shared_ptr<Collision>> physCol;
	std::vector<std::shared_ptr<LogicCollision>> logicCol;
	std::vector<std::shared_ptr<DefendCollision>> defCol;

} hitboxes_t;



// Inherit BoxObstacle for physics (do we want a different shape?)
class PhysicalObject : public MultiHitbox { //just do all stuff as multihitbox!
private:
	/** This macro disables the copy constructor (not allowed on scene graphs) */
	CU_DISALLOW_COPY_AND_ASSIGN(PhysicalObject);

protected:
	/** The force to apply to this object */
	cugl::Vec2 _force;

	/** The scene graph node for the object */
	std::shared_ptr<cugl::AnimationNode> _objectNode;

	// hitboxes of object
	//std::shared_ptr<cugl::SimpleObstacle> _hitbox;

	bool isDestructable;

	//bool isEnemy = false; // mark removal
    //bool attacked = false; // mark removal

	/** Cache object for transforming the force according the object angle */
	cugl::Mat4 _affine;
	float _drawscale;

	int _collisionDamage;
	int _totalHealth;
	int _currentHealth;
	int _healthLostThisFrame;
	bool _permissionToDie;
	bool _permissionToMoveOn;
    
    PhysicalObject *_owner;


	// associate animation with some hitbox and some of its nodes
	typedef struct animPack {
		int hitboxIdx;
		std::vector<int> nodeIdxs;
		std::shared_ptr<Animation> animDelegate;
	} animPack_t;

	std::vector<animPack_t> _anims;

	std::shared_ptr<Movement> _movementDelegate;
	std::shared_ptr<Collision> _physCollisionDelegate;
	std::shared_ptr<LogicCollision> _logicCollisionDelegate;
	std::shared_ptr<DefendCollision> _defenseDelegate;
	//std::vector<std::shared_ptr<Animation>> _animDelegates;
	std::shared_ptr<AIControl> _AIDelegate;
	std::vector<std::shared_ptr<UpdateLogic>> _updateLogicDelegates;

	cugl::SimpleObstacle *_lastHitboxCol;

	//boolean for whether this physical object is the player or not
	bool _isPlayer;

	// TODO: maybe put a collision queue instead?
	PhysicalObject *_lastZone;
	cugl::Timestamp _lastCollisionTime;
    //cugl::Timestamp _idleTime;
    //cugl::Timestamp _lastAttackTime;
    
    //this is for projectile and melee attacks
    std::shared_ptr<Attack> _attackDelegation;

	unsigned long long _lastLoggedFrame = 0;
	PhysicalObject *_lastCollidedObject = nullptr;

	// TODO only used for newton b/c bad
	bool _rejectedActionThisFrame = false;
	

public:


	// delegate setters
	void delegateMovement(std::shared_ptr<Movement> del);
	void delegatePhysCollision(std::shared_ptr<Collision> del);
	void delegateLogicCollision(std::shared_ptr<LogicCollision> del);
	void delegateDefense(std::shared_ptr<DefendCollision> del);
	void delegateAnimation(std::shared_ptr<Animation> del, int hitboxIdx, int *associatedNodes, int numNodes);
	void delegateAI(std::shared_ptr<AIControl> del) { _AIDelegate = del; }
	void delegateUpdateLogic(std::shared_ptr<UpdateLogic> del);

	virtual void doMovement(bool isNewResp, cugl::Vec2 actionVec, cugl::Vec2 *returnVec);
	void adjustSpeed(float speedMult) { _movementDelegate->adjustSpeed(this, speedMult); }
	PhysicalObject *getLastZone() const { return _lastZone; }
	void setLastZone(PhysicalObject *lastZone) { _lastZone = lastZone; }


	virtual void onCollide(PhysicalObject *otherObj, cugl::SimpleObstacle *thisHitbox,
							cugl::SimpleObstacle *otherHitbox, cugl::Vec2 normalVec, 
							bool noPhysCollision, unsigned long long frameNum);
	bool doCollide(PhysicalObject *otherObj);
	//void procPhysCollisions();
	virtual void onAttacked(int damage, cugl::Vec2 velocity, bool shieldBreak);
	Uint64 getTimeSinceLastAttack() const { return _attackDelegation->getElapsedTime(); }
	virtual void processBehavior(AI_args_t *args);
	void setHitboxCol(cugl::SimpleObstacle *hitbox);
	void deactivateLastHitbox();
	bool isBeserker() { return _physCollisionDelegate->IsBerserker(); }
	void updateLogic(do_logic_args_t *args);


	//virtual cugl::Poly2 getPoly() { return nullptr; }

	//getter/setter for player attribute 
	bool isPlayer() { return _isPlayer; }
	void setPlayer(bool val) { _isPlayer = val; }
    
    //get and set owner
    void setOwner(PhysicalObject *_newOwner) { _owner = _newOwner; }
    PhysicalObject * getOwner(){ return _owner; }

	PhysicalObject *getLastCollidedObject() const { return _lastCollidedObject; }
	bool getRejectionThisFrame() const { return _rejectedActionThisFrame; }
	void setRejectionThisFrame(bool reject) { _rejectedActionThisFrame = reject; }

	// update routine for object
	virtual void updateObject(float dt);
    virtual void updateTexture(bool newAttack, bool levelExit, std::shared_ptr<ActionManager> actions, float dt);
    
    //cugl::Timestamp getLastAttack() { return _lastAttackTime; } //push into AI

public:
#pragma mark Constructors
	/**
	* Creates a new projectile at the origin.
	*
	* NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate a model on
	* the heap, use one of the static constructors instead.
	*/
	PhysicalObject(void) { }

	/**
	* Destroys this projectile, releasing all resources.
	*/
	virtual ~PhysicalObject(void) { dispose(); }

	/**
	* Disposes all resources and assets of this projectile
	*
	* Any assets owned by this object will be immediately released.  Once
	* disposed, a projectile may not be used until it is initialized again.
	*/
	virtual void dispose();

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
	virtual bool init(const cugl::Vec2& pos, const cugl::Size& size);

	/*virtual bool initWithTexture(const std::shared_ptr<cugl::Texture>& texture,
									const cugl::Poly2& poly); //?*/


	virtual bool initFromList(hitboxes_t *hitboxes);

	bool initFunctionality();


#pragma mark Static Constructors
	/**
	* Returns a newly allocated projectile with the given position and size
	*
	* The rocket size is specified in world coordinates.
	*
	* The scene graph is completely decoupled from the physics system.
	* The node does not have to be the same size as the physics body. We
	* only guarantee that the scene graph node is positioned correctly
	* according to the drawing scale.
	*
	* @param pos   Initial position in world coordinates
	* @param size  The dimensions of the box.
	*
	* @return a newly allocated projectile with the given position and size
	*/
	/*template <class T>
	static std::shared_ptr<T> alloc(const cugl::Vec2& pos, const cugl::Size& size) {
		std::shared_ptr<T> result = std::make_shared<T>();
		return (result->init(pos, size) ? result : nullptr);
	} //alloc weird b/c can't virtual it b/c static. Recommend removal*/

	static std::shared_ptr<PhysicalObject> alloc(const cugl::Vec2& pos, const cugl::Size& size) {
		std::shared_ptr<PhysicalObject> result = std::make_shared<PhysicalObject>();
		return (result->init(pos, size) ? result : nullptr);
	}



	// A few of these functions should always be made when have physics object in cugl.
	// Consider making a parent class
#pragma mark -
#pragma mark Animation
	/**
	* Returns the scene graph node representing this rocket.
	*
	* By storing a reference to the scene graph node, the model can update
	* the node to be in sync with the physics info. It does this via the
	* {@link Obstacle#update(float)} method.
	*
	* @return the scene graph node representing this rocket.
	*/
	const std::shared_ptr<cugl::AnimationNode>& getSceneGraphNode() const { return _objectNode; }

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
	virtual void setSceneGraphNode(std::shared_ptr<cugl::AnimationNode>& node);
	virtual void addSceneGraphNode(std::shared_ptr<cugl::AnimationNode>& node, cugl::Vec2 physOffset, int hitboxIdx);



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
	void setDrawScale(float scale);

#pragma mark -
#pragma mark Physics
	/**
	* Applies the force to the body of this ship
	*
	* This method should be called after the force attribute is set.
	*/
	void applyForce();

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
	//virtual void update(float delta) override;

	


#pragma mark -
#pragma mark Accessors
	void setDestructable(bool is) { isDestructable = is; }

	bool getDestructable() const { return isDestructable; }

	//void setIsEnemy(bool is) { isEnemy = is; }

	bool getIsEnemy() const { return (_AIDelegate != nullptr); }

	void setDamage(int damage) { _collisionDamage = damage; }

	int getDamage() const { return _collisionDamage; }

	void setTotalHealth(int health) { _totalHealth = health; }

	int getTotalHealth() const { return _totalHealth; }

	void setCurrentHealth(int health) { _currentHealth = health; }

	int getCurrentHealth() const { return _currentHealth; }

	bool isDead() { return _currentHealth <= 0; }

	void kill() { _healthLostThisFrame = _currentHealth; _currentHealth = 0; }

	bool canRemove() { return isDead() && isDestructable && _permissionToDie; }
    
	int getHealthLostThisFrame() const { return _healthLostThisFrame; }
	void addHealthLostThisFrame(int damage) { _healthLostThisFrame += damage; }
	void resetHealthLostThisFrame() { _healthLostThisFrame = 0; }

	void setPermissionToDie(bool perm) { _permissionToDie = perm; }

	void setPermissionToMoveOn(bool perm) { _permissionToMoveOn = perm; }
	bool getPermissionToMoveOn() const { return _permissionToMoveOn; }

    /*bool getAttacked() {return attacked; }
    
    void setAttacked() {attacked = true; }
    
    const cugl::Timestamp getAttackTime() const { return _lastAttackTime; }
    
    void setAttackTime() { _lastAttackTime.mark(); }*/

	/**
	* Returns the force applied to this rocket.
	*
	* Remember to modify the input values by the thrust amount before assigning
	* the value to force.
	*
	* @return the force applied to this rocket.
	*/
	const cugl::Vec2& getForce() const { return _force; }

	/**
	* Sets the force applied to this rocket.
	*
	* Remember to modify the input values by the thrust amount before assigning
	* the value to force.
	*
	* @param value  the force applied to this rocket.
	*/
	void setForce(const cugl::Vec2& value) { _force.set(value); }

	/**
	* Returns the x-component of the force applied to this rocket.
	*
	* Remember to modify the input values by the thrust amount before assigning
	* the value to force.
	*
	* @return the x-component of the force applied to this rocket.
	*/
	float getFX() const { return _force.x; }

	/**
	* Sets the x-component of the force applied to this rocket.
	*
	* Remember to modify the input values by the thrust amount before assigning
	* the value to force.
	*
	* @param value the x-component of the force applied to this rocket.
	*/
	void setFX(float value) { _force.x = value; }

	/**
	* Returns the y-component of the force applied to this rocket.
	*
	* Remember to modify the input values by the thrust amount before assigning
	* the value to force.
	*
	* @return the y-component of the force applied to this rocket.
	*/
	float getFY() const { return _force.y; }

	/**
	* Sets the x-component of the force applied to this rocket.
	*
	* Remember to modify the input values by the thrust amount before assigning
	* the value to force.
	*
	* @param value the x-component of the force applied to this rocket.
	*/
	void setFY(float value) { _force.y = value; }

	/**
	* Returns the amount of thrust that this rocket has.
	*
	* Multiply this value times the horizontal and vertical values in the
	* input controller to get the force.
	*
	* @return the amount of thrust that this rocket has.
	*/
	float getForceMod() const { return DEFAULT_MOD; }


#pragma mark -
#pragma mark Hitbox
	//b2Body *getBody() const { return _body; }
	//std::shared_ptr<cugl::Obstacle> createHitbox(cugl::Vec2 pos, const cugl::Size &size, int idx, int version, std::shared_ptr<cugl::Node> debugnode);
	//virtual void createHitbox(cugl::Vec2 pos, const cugl::Size &size);
	virtual std::shared_ptr<cugl::SimpleObstacle> createHitbox(cugl::Vec2 pos, const cugl::Size &size);
	virtual std::shared_ptr<cugl::SimpleObstacle> createHitEllipse(cugl::Vec2 pos, const cugl::Size &size);

	void createAddHitbox(cugl::Vec2 offsetPos, const cugl::Size &size, int shapeId,
		std::shared_ptr<Collision> physCol, std::shared_ptr<LogicCollision> logicCol,
		std::shared_ptr<DefendCollision> defCol);

	void activateNewHitbox(int id, cugl::ObstacleWorld *world) { appendHitbox(id, world); }
	bool reactivateHitbox(int id) { return MultiHitbox::reactivateHitbox(id); }
	//virtual bool createJoints(b2World& world) override;

	// define physics functions by accessing hitbox
	/*void setDensity(float density) { _hitbox->setDensity(density); }
	void setFriction(float friction) { _hitbox->setFriction(friction); }
	void setRestitution(float rest) { _hitbox->setRestitution(rest); }
	void setFixedRotation(bool isFixed) { _hitbox->setFixedRotation(isFixed); }
	bool isActive() { return _hitbox->isActive(); }

	// TODO
	cugl::Vec2 getPosition() { return _hitbox->getPosition(); }
	void setPosition(cugl::Vec2 pos) { _hitbox->setPosition(pos); }

	float getAngle() { return _hitbox->getAngle(); }
	//std::shared_ptr<cugl::SimpleObstacle> getObstacle() { return _hitbox; }
	void setLinearVelocity(cugl::Vec2 vel) { _hitbox->setLinearVelocity(vel); }
	void setAngle(float angle) { _hitbox->setAngle(angle); }
	void setBullet(bool isBullet) { _hitbox->setBullet(isBullet); }

	int getIdx() { return _hitbox->getIdx(); }
	int getVersion() { return _hitbox->getVersion(); }
	void setIdx(int idx) { _hitbox->setIdx(idx); }
	void setVersion(int version) { _hitbox->setVersion(version); }

	std::string getName() { return _hitbox->getName(); }
	void setName(std::string name) { _hitbox->setName(name); }

	void setDebugScene(const std::shared_ptr<cugl::Node> &node) { _hitbox->setDebugScene(node); }
	void setDebugColor(cugl::Color4 color) { _hitbox->setDebugColor(color); }

	std::shared_ptr<cugl::SimpleObstacle> getHitbox() { return _hitbox; }*/
    
    
#pragma mark -
#pragma mark Attack
    
    
public:
    void delegateAttack(std::shared_ptr<Attack> del);
    
    virtual const attackInfo_t *doAttack(bool attackReq, cugl::Vec2 attackDir, int *numAttack);

};



#endif
