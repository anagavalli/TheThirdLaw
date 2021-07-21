
#ifndef __CHARACTER_MODEL_H__
#define __CHARACTER_MODEL_H__
#include <cugl/cugl.h>
#include "PhysicalObject.h"
#include "Attack.h"
#include "Animation.h"

/** The thrust factor to convert player input into thrust */
#define DEFAULT_THRUST 1000.0f

typedef enum orientation_mode {
	NORMAL,
	ROUND_DOWN,
	ROUND_UP
} orientation_mode_e;

/**
 * This class is the player avatar for the rocket lander game.
 *
 * There are two ways to link a physics object to a scene graph node on the
 * screen.  One way is with callback functions, like we did with the crates.
 * The other way is to create a subclass of a physics object and have it track
 * its own scene graph node.  This class is an example of the latter.
 */
class CharacterModel : public PhysicalObject {
protected:

    /** The scene graph node for the rocket ship */
    //std::shared_ptr<cugl::AnimationNode> _characterNode;
    
	// [animation][orientation]. should have this in a seperate class, so don't have to remake for multiple instances of same enemy
	//std::vector<std::vector<std::shared_ptr<cugl::Texture>>> _textures;

	// current frame in animation
	//int _currFrame = 0; //when have animation nodes will not need this. want page of animation for single orientation
	int _currOrientation = 0;
	int _lastOrientation = 0; 
	//int _currAnimation = 0;
	cugl::Timestamp _idleTime;

    int numOrientations = 0;
	orientation_mode_e _omode = NORMAL;
	bool _turnOnAttack = true;
	bool _turnOnMove = true;
	// delagation for physics
	/*bool _orientationOnImpulse;

	// handles force impulse to the character
	std::function<void(cugl::Vec2,PhysicalObject *,cugl::Vec2 *)> _impulseHandle;
	// handles movement of the character (physics update function)
	std::function<void(cugl::Vec2, PhysicalObject *)> _velocityHandle;*/


	// handles how weapon works (i.e. create projectile, melee attack?)
	//std::function<const attackInfo_t(bool, PhysicalObject *, cugl::Vec2)> _attackHandler;
	//std::shared_ptr<Attack> _attackDelegation;

    ///** The animation node for the main afterburner */
    //std::shared_ptr<cugl::AnimationNode> _mainBurner;

	//std::shared_ptr<cugl::WheelObstacle> circle;
    
    
public:
#pragma mark Getters
    int getOrientations() { return numOrientations; }
    void setOrientations(int num) { numOrientations = num; }
    
#pragma mark Constructors
    /**
     * Creates a new rocket at the origin.
     *
     * NEVER USE A CONSTRUCTOR WITH NEW. If you want to allocate a model on
     * the heap, use one of the static constructors instead.
     */
    CharacterModel(void) : PhysicalObject() { }
    
    /**
     * Destroys this rocket, releasing all resources.
     */
    virtual ~CharacterModel(void) { dispose(); }
    
    /**
     * Disposes all resources and assets of this rocket
     *
     * Any assets owned by this object will be immediately released.  Once
     * disposed, a rocket may not be used until it is initialized again.\
	 *
	 * Note inheritance in C++. Will take the lowest level implemention of a method that exists in parent and child class
	 * If want to override must declare in h file and give implementation
     */
    void dispose() override;
    

	static std::shared_ptr<CharacterModel> alloc(const cugl::Vec2& pos, const cugl::Size& size) {
		std::shared_ptr<CharacterModel> result = std::make_shared<CharacterModel>();
		return (result->init(pos, size) ? result : nullptr);
	}

	static std::shared_ptr<CharacterModel> alloc(hitboxes_t *hitboxes) {
		std::shared_ptr<CharacterModel> result = std::make_shared<CharacterModel>();
		return (result->initFromList(hitboxes) ? result : nullptr);
	}



    /**
     * Initializes a new rocket with the given position and size.
     *
     * The rocket size is specified in world coordinates.
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
    virtual bool init(const cugl::Vec2& pos, const cugl::Size& size) override;

	bool initFunctionality();

	virtual bool initFromList(hitboxes_t *hitboxes) override;
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
    //const std::shared_ptr<cugl::AnimationNode> getSceneGraphNode() const { return _objectNode; } 

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
    //void setSceneGraphNode(const std::shared_ptr<cugl::AnimationNode>& node);
	//void addSceneGraphNode(const std::shared_ptr<cugl::AnimationNode>& node, int hitboxIdx);

	//void addAnimation(std::string texName, int numFrames, int numOrientations, std::shared_ptr<cugl::AssetManager> &assets);
	void setTexture(cugl::Vec2 movementVec);
	int getOrientation(cugl::Vec2 directionVec, int animationID);
	void updateTexture(bool newAttack, bool levelExit, std::shared_ptr<ActionManager> actions, float dt) override; //todo no override
	void updateObject(float dt) override;
	void setOrientationMode(orientation_mode_e omode) { _omode = omode; }
	void setTurnOnAttack(bool turn) { _turnOnAttack = turn; }
	void setTurnOnMove(bool turn) { _turnOnMove = turn; }
#pragma mark -
#pragma mark Physics



public:

	void doMovement(bool isNewResp, cugl::Vec2 actionVec, cugl::Vec2 *returnVec) override;
	/*void delegateImpulse(bool setOrientationHere,
		std::function<void(cugl::Vec2, PhysicalObject *, cugl::Vec2 *)> impulseDel);

	void delegateMovement(bool setOrientationHere,
		std::function<void(cugl::Vec2, PhysicalObject *)> movementDel);

	void applyImpulse(cugl::Vec2 actionVec, cugl::Vec2 *returnVec);
	void setVelocity(cugl::Vec2 movement = cugl::Vec2::ZERO);*/


	// TODO: move to physical object.h
	

#pragma mark -
#pragma mark Attack


public:
	//void delegateAttack(std::shared_ptr<Attack> del);

	const attackInfo_t *doAttack(bool attackReq, cugl::Vec2 attackDir, int *numAttacks) override;

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
};

#endif /* __RD_ROCKET_MODEL_H__ */
