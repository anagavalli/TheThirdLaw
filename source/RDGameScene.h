//
//  RDGameMode.h
//  Rocket Demo
//
//  This is the most important class in this demo.  This class manages the
//  gameplay for this demo.  It also handles collision detection. There is not
//  much to do for collisions; our ObstacleWorld class takes care of all
//  of that for us.  This controller mainly transforms input into gameplay.
//
//  You will notice that we do not use a Scene asset this time.  While we could
//  have done this, we wanted to highlight the issues of connecting physics
//  objects to scene graph objects.  Hence we include all of the API calls.
//
//  WARNING: There are a lot of shortcuts in this design that will do not adapt
//  well to data driven design.  This demo has a lot of simplifications to make
//  it a bit easier to see how everything fits together.  However, the model
//  classes and how they are initialized will need to be changed if you add
//  dynamic level loading.
//
//  This file is based on the CS 3152 PhysicsDemo Lab by Don Holden, 2007
//
//  Author: Walker White
//  Version: 1/10/17
//
#ifndef __RD_GAME_SCENE_H__
#define __RD_GAME_SCENE_H__
#include <cugl/cugl.h>
#include <Box2D/Dynamics/b2WorldCallbacks.h>
#include <vector>
#include "CharacterModel.h"
#include "RDInput.h"
#include "Projectile.h"
#include "ObjectContainer.h"
#include "LevelStartup.h"


// set possible types of things in the scene (these will be the classes that we slot in components for)
// types will not be things like "archer" or "warrior" because we use component design for that not type based
// instead these will be for clearly segmented things suchs as projectiles, enemies, obstacles, etc.
// count always needs to be last item
enum class PossibleTypes : int
{
	Rocket = 0,
	Projectile,
	Obstacle,
	Wall,
	Count
};

typedef struct type_idx {
	PossibleTypes type;
	int idx;
	int ver;
} type_idx_t;


/**
 * This class is the primary gameplay constroller for the demo.
 *
 * A world has its own objects, assets, and input controller.  Thus this is
 * really a mini-GameEngine in its own right.  As in 3152, we separate it out
 * so that we can have a separate mode for the loading screen.
 */
class GameScene : public cugl::Scene {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;
    
    // CONTROLLERS
    /** Controller for abstracting out input across multiple platforms */
    RocketInput _input;
	//animation manager
	std::shared_ptr<ActionManager> _actions;
    
    // VIEW
    /** Reference to the physics root of the scene graph */
    std::shared_ptr<cugl::Node> _rootnode;

	/** reference to the physics root of the scene graph */
	std::shared_ptr<Node> _worldnode;

	std::shared_ptr<Node> _debugnode;

	std::shared_ptr<ObstacleWorld> _world;

    //std::shared_ptr<cugl::Node> _loadnode;
    /** Reference to the win message label */
    std::shared_ptr<cugl::Label> _winnode;

    /** The scale between the physics world and the screen (MUST BE UNIFORM) */
    float _scale;

	/*level startup/model*/
	std::shared_ptr<LevelStartup> _level;
	int _curLevel = 0;
	std::string getLevelKey(int levelno);
	bool loadLevel(int levelno);

	/** The y shift between physics world and the screen */
	float _physVertShift;

    // Physics objects for the game
    /** Reference to the goalDoor (for collision detection) */
    std::shared_ptr<cugl::BoxObstacle> _goalDoor;
    /** Reference to the rocket/player avatar */
    std::shared_ptr<CharacterModel> _rocket;
	//std::shared_ptr<CharacterModel> _enemy;
	//std::shared_ptr<CharacterModel> _enemy2;

	// replacements for IMAGE_WIDTH and DEFAULT_WIDTH
	float bgWidth;
	float bgHeight;
	float box2dWidth;
	float box2dHeight;

	/** Reference to all objects in the scene */
	std::shared_ptr<ObjectContainer> _allObjects;


    /** Whether we have completed this "game" */
    bool _complete;

	// true if game is paused
	bool _paused;
	
	void switchPauseState();

	/** Number of frames spent in this scene */
	unsigned long long _numFrames;

    // This is for the buttons shown in the menu
    std::shared_ptr<PolygonNode> _menuBut;
    std::shared_ptr<PolygonNode> _resetBut;
    std::shared_ptr<PolygonNode> _quitBut;
    std::shared_ptr<PolygonNode> _gameoverBut;
    
    std::shared_ptr<cugl::Button> _menuButton;
    std::shared_ptr<cugl::Button> _resetButton;
    std::shared_ptr<cugl::Button> _quitButton;
    
    std::shared_ptr<PolygonNode> _loseMenu;
    
    bool _QuitToMenu;
    bool _Retry;
    bool _LostLevel;
    bool _QuitToMenuGameover;
    
#pragma mark Internal Object Management
	//void AddToScene(std::shared_ptr<PhysicalObject> object, cugl::Vec2 objScale, const char *texKey, int zOrder);
	
	std::shared_ptr<cugl::Obstacle> AddHitbox(std::shared_ptr<PhysicalObject> object, cugl::Vec2 initPos, cugl::Size &size,
		int idx, int version, std::shared_ptr<cugl::Node> debugnode);
    
	void removeFromScreen(std::shared_ptr<PhysicalObject> object);
	void physicsRemoval(std::shared_ptr<PhysicalObject> object);
	//bool GameScene::processShoot(cugl::Vec2 *movement);
	bool checkTouch(cugl::Vec2 *touchLoc);
	//void patrol(CharacterModel::enemy);

	void activateWorldCollisions(const std::shared_ptr<ObstacleWorld>& world);


public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new game world with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    GameScene();
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameScene() { dispose(); }
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    void dispose();
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  This initializer uses the default scale.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, int level);

    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param rect      The game bounds in Box2d coordinates
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const cugl::Rect& rect, int level);
    
    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * The game world is scaled so that the screen coordinates do not agree
     * with the Box2d coordinates.  The bounds are in terms of the Box2d
     * world, not the screen.
     *
     * @param assets    The (loaded) assets for this game mode
     * @param rect      The game bounds in Box2d coordinates
     * @param gravity   The gravitational force on this Box2d world
     *
     * @return  true if the controller is initialized properly, false otherwise.
     */
    bool init(const std::shared_ptr<cugl::AssetManager>& assets, const cugl::Rect& rect, const cugl::Vec2& gravity, int level);
    
    
#pragma mark -
#pragma mark State Access
    /**
     * Returns true if the gameplay controller is currently active
     *
     * @return true if the gameplay controller is currently active
     */
    bool isActive( ) const { return _active; }

    /**
     * Returns true if debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @return true if debug mode is active.
     */
	bool isDebug() const { return _level->isDebug(); }
    
    /**
     * Sets whether debug mode is active.
     *
     * If true, all objects will display their physics bodies.
     *
     * @param value whether debug mode is active.
     */
	void setDebug(bool value) { _level->setDebug(value); }
    
    /**
     * Returns true if the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @return true if the level is completed.
     */
    bool isComplete( ) const { return _complete; }
    
    /**
     * Sets whether the level is completed.
     *
     * If true, the level will advance after a countdown
     *
     * @param value whether the level is completed.
     */
    void setComplete(bool value) { _complete = value; }

	int getCurLevel() { return _curLevel; }
    void setCurLevel(int level){_curLevel = level;}

    bool setPaused(bool val) { _paused = val; return true;}
	bool getPaused() { return _paused; }
    
    bool isQuit(){ return _QuitToMenu; }
    
	bool rayCast(cugl::Vec2 from, cugl::Vec2 towards, int idx, int ver /*b2Fixture *targFixture*/);
    
#pragma mark -
#pragma mark Gameplay Handling
    /**
     * The method called to update the game mode.
     *
     * This method contains any gameplay code that is not an OpenGL call.
     *
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep);
    
    /**
     * Updates that animation for a single burner
     *
     * This method is here instead of the the rocket model because of our 
     * philosophy that models should always be lightweight.  Animation includes 
     * sounds and other assets that we do not want to process in the model.
     *
     * @param burner    The rocket burner to animate
     * @param on        Whether to turn the animation on or off
     */
    //void updateBurner(RocketModel::Burner burner, bool on);

    /**
     * Resets the status of the game so that we can play again.
     */
    void reset();

    
    
#pragma mark -
#pragma mark Collision Handling
    /**
     * Processes the start of a collision
     *
     * This method is called when we first get a collision between two objects. 
     * We use this method to test if it is the "right" kind of collision.  In 
     * particular, we use it to test if we make it to the win door.
     *
     * @param  contact  The two bodies that collided
     */
    void beginContact(b2Contact* contact);

    /**
     * Handles any modifications necessary before collision resolution
     *
     * This method is called just before Box2D resolves a collision.  We use 
     * this method to implement sound on contact, using the algorithms outlined 
     * in Ian Parberry's "Introduction to Game Physics with Box2D".
     *
     * @param  contact  The two bodies that collided
     * @param  contact  The collision manifold before contact
     */
    void beforeSolve(b2Contact* contact, const b2Manifold* oldManifold);


	void markObjRemoval(bool removeCondition, int idx, int version, PossibleTypes objType);

};

#endif /* __RD_GAME_MODE_H__ */
