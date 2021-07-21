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
#include "RDGameScene.h"
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>
#include <Box2D/Collision/b2Collision.h>

#include <ctime>
#include <string>
#include <iostream>
#include <sstream>

#include "LevelStartup.h"
#include "Constants.h"
#include "Levels.h"

#include "BasicPhysics.h"
#include "SplinePhysics.h"
#include "TouchSplinePhysics.h"
#include "Camera2d.h"
#include "Polyzone.h"
#include "ProjectileAttack.h"
#include "Explode.h"

//#include "MapPath.h"

using namespace cugl;


#pragma mark -

float clamp(float n, float lower, float upper) {
    return std::max(lower, std::min(n, upper));
}

//bool enemy1_dead = false;
//bool enemy2_dead = false;


// linked list queue to mark deletion of items
std::queue<type_idx_t> deadQueue;

//std::shared_ptr<TouchSplinePhysics> phys;

// temp newton textures
//std::string newtonTextures[8];

std::shared_ptr<Camera2d> camera;


#pragma mark -
#pragma mark Constructors
/**
 * Creates a new game world with the default values.
 *
 * This constructor does not allocate any objects or start the controller.
 * This allows us to use a controller without a heap pointer.
 */
GameScene::GameScene() : Scene(),
_complete(false),
_paused(false)
{    
}

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
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, int level) {
    return init(assets,Rect(),Vec2(0,DEFAULT_GRAVITY), level);
}

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
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, const Rect& rect, int level) {
    return init(assets,rect,Vec2(0,DEFAULT_GRAVITY), level);
}

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
bool GameScene::init(const std::shared_ptr<AssetManager>& assets, const Rect& rect, const Vec2& gravity, int level) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    }
    else if (!Scene::init(dimen)) {
        return false;
    }
    
	_LostLevel = false;
    _QuitToMenu = false;
    _QuitToMenuGameover = false;
    
    // start at frame 0
    _numFrames = 0;
    
    // search boxes
    /*for (int i = 0; i < 10; i++) {
     searchBoxes[i] = BoxObstacle::alloc(Vec2::ZERO, Vec2(0.5f,0.5f));
     searchBoxes[i]->setSensor(true);
     }*/
    
    // Start up the input handler
    _assets = assets;
    
    //// Init action manager
    //_actions = ActionManager::alloc();
    
    bool success = loadLevel(level);
    

    // XNA nostalgia
    Application::get()->setClearColor(Color4f::CORNFLOWER);
    return success;
}

bool GameScene::loadLevel(int levelno) {
	_curLevel = levelno;

	if (_curLevel > NUM_LEVELS) _curLevel = _curLevel % NUM_LEVELS;

	if (_rootnode != nullptr) {
		dispose();
	}

	_input.init();

	level_info_t levelInfo;
	getLevelInfo(_curLevel, &levelInfo);
	_level = _assets->get<LevelStartup>(levelInfo.key);
	if (_level == nullptr) {
		CULog("Failed to load level in GameScene!");
		return false;
	}
    
	Size dimen = Application::get()->getDisplaySize();
	dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution

	// setup object list (in the future this will have multiple types
	_allObjects = std::shared_ptr<ObjectContainer>(new ObjectContainer(MAX_OBJECTS));

	//do some loading that has to be done after the loading screen but before everything else
	//auto imagejson = _level->getJSON()->get("level_image");
	//std::string imageStr = imagejson->asString();
	//std::shared_ptr<Texture> image = _assets->get<Texture>(imageStr.c_str());
	_level->setAssets(_assets);
	_level->preinit();

	bgWidth = _level->getBgWidth();
	bgHeight = _level->getBgHeight();
	box2dWidth = _level->getBox2dWidth();
	box2dHeight = _level->getBox2dHeight();

	// Create the world and attach the listeners.
	_world = _level->getWorld();
	activateWorldCollisions(_world);

	// IMPORTANT: SCALING MUST BE UNIFORM
	// This means that we cannot change the aspect ratio of the physics world
	_scale = 1024 / 32;//dimen.width / rect.size.width;

	// PBB handle discrepancy between different y axis of physics and screen space
	_physVertShift = Application::get()->getDisplayHeight(); //dimen.height;

	 // Create the scene graph
	_rootnode = Node::alloc();
	_rootnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	_rootnode->setPosition(Vec2::ZERO);

	addChild(_rootnode, 0);

	_rootnode->setContentSize(dimen);
	_level->setRootNode(_rootnode); // Obtains ownership of root.

	// initialize objects in level, pass in 
	_level->initLevel(_allObjects);
	_rocket = _level->getPlayer();
	//_enemy = _level->_enemy;
	//_enemy2 = _level->_enemy2;

	_worldnode = _level->getWorldNode();
	_debugnode = _level->getDebugNode();

	// Need to tell the game "hey, actually pay attention to zOrder, bozo"
	_rootnode->sortZOrder();

	// setup custom physics
	//phys = std::make_shared<TouchSplinePhysics>(_scale, _physVertShift, RECOIL_MOD, BOUNCE_MOD, true);
	//phys->createSpline();

	//populate();
	_active = true;
	_complete = false;
	_paused = false;
    _QuitToMenu = false;
    _Retry = false;
	setDebug(false);

	//setup the camera
	//Vec2 cameraStart = Vec2(-1.0f * bgWidth * 0.33, -1.0f * bgHeight * 0.7);
	Vec2 cameraStart = _rocket->getPosition();
	camera = std::make_shared<Camera2d>(_scale, dimen / 2.0f, dimen.width / 4.5f, 0.11f); //0.11f is really good
	Vec2 initialCameraPos = cameraStart; //HARDCODED
	camera->pan(initialCameraPos, _allObjects, _level->bgSprite);

	// hitboxes don't join if don't do this (before physics starts up)
	for (int i = 0; i < _allObjects->getSize(); i++) {
		_allObjects->get(i)->adjustHitboxes();
	}

	// Init action manager
	if (_actions != nullptr) {
		_actions->dispose();
	}
	_actions = ActionManager::alloc();

    // ADD BACKGROUND MUSIC HERE
    if (_curLevel == 1 || _curLevel == 2 ) {
        std::shared_ptr<Music> source = _assets->get<Music>(LVL_TUT_MUSIC);
        AudioEngine::get()->playMusic(source, true, 0.7f);
    } else if(_curLevel == 3){
        std::shared_ptr<Music> source = _assets->get<Music>(LVL_ONE_MUSIC);
        AudioEngine::get()->playMusic(source, true, 0.7f);
    } else if(_curLevel == 4){
        std::shared_ptr<Music> source = _assets->get<Music>(LVL_TWO_MUSIC);
        AudioEngine::get()->playMusic(source, true, 0.7f);
    }else if(_curLevel == 6){
        std::shared_ptr<Music> source = _assets->get<Music>(LVL_FUR_MUSIC);
        AudioEngine::get()->playMusic(source, true, 0.7f);
    }else if(_curLevel == 7){
        std::shared_ptr<Music> source = _assets->get<Music>(LVL_FVE_MUSIC);
        AudioEngine::get()->playMusic(source, true, 0.7f);
    }else {
        std::shared_ptr<Music> source = _assets->get<Music>(LVL_BSS_MUSIC);
        AudioEngine::get()->playMusic(source, true, 0.7f);
    }
    
    // ----------- Adding Pause Menu Buttons --------------
    
    // QUIT TO MENU BUTTON
	float scale = 0.25f;
    _quitBut = std::make_shared<PolygonNode>();
    _quitBut->initWithTexture(_assets->get<Texture>("quit"));
    
    _quitButton = std::make_shared<cugl::Button>();
    _quitButton->init(_quitBut);
	_quitButton->setScale(Vec2(scale, scale));

    _rootnode->addChild(_quitButton);
    
    int height = dimen.getIHeight();
    int width = dimen.getIWidth();
	
    int butWidth = _assets->get<Texture>("quit")->getWidth() * scale;
    int butHeight = _assets->get<Texture>("quit")->getHeight() * scale;

    _quitButton->setPosition(butWidth*0.5, height-butHeight*1.5);
    
    _quitButton->setListener([=](const std::string& name, bool down) {
        _QuitToMenu = true;
    });
    _quitButton->activate(17);
    
    // --------------- Adding Game Over Buttons -----------------
    
    // YOU LOSE MENU
    _loseMenu = std::make_shared<PolygonNode>();
    _loseMenu->initWithTexture(_assets->get<Texture>("pause_bg"));
    
    _worldnode->addChild(std::dynamic_pointer_cast<Node, PolygonNode>(_loseMenu), 100);
    
    _loseMenu->setVisible(false);
    
    // GAME OVER TEXT
    _gameoverBut = std::make_shared<PolygonNode>();
    _gameoverBut->initWithTexture(_assets->get<Texture>("game over"));
    _gameoverBut->setPosition(width*0.5, height*0.7);
    _gameoverBut->setScale(0.5);
    
    _loseMenu->addChild(_gameoverBut);
    
    // MENU BUTTON
    float menuscale = 0.6f;
    _menuBut = std::make_shared<PolygonNode>();
    _menuBut->initWithTexture(_assets->get<Texture>("menu"));
    
    _menuButton = std::make_shared<cugl::Button>();
    _menuButton->init(_menuBut);
    _menuButton->setScale(Vec2(menuscale, menuscale));
    
    _loseMenu->addChild(_menuButton);
    
    _menuButton->setPosition(width*0.6, height*0.2);
    
    _menuButton->setListener([=](const std::string& name, bool down) {
        _QuitToMenuGameover = true;
    });
    
    // RETRY BUTTON
    _resetBut = std::make_shared<PolygonNode>();
    _resetBut->initWithTexture(_assets->get<Texture>("reset"));
    
    _resetButton = std::make_shared<cugl::Button>();
    _resetButton->init(_resetBut);
    _resetButton->setScale(Vec2(menuscale, menuscale));
    
    _loseMenu->addChild(_resetButton);
    
    _resetButton->setPosition(width*0.15, height*0.2);
    
    _resetButton->setListener([=](const std::string& name, bool down) {
        _Retry = true;
    });
    
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _input.dispose();
        _rootnode = nullptr;
        _worldnode = nullptr;
        _debugnode = nullptr;
        _winnode = nullptr;
        _active = false;
        _complete = false;
		_paused = false;
		_level = nullptr;
		_allObjects = nullptr;
        _QuitToMenu = false;

        _quitButton->deactivate();
        _menuButton->deactivate();
        _resetButton->deactivate();
		// PBB object array cleanup -- error!
		/*if (_projectiles != nullptr) {
			delete(_projectiles);
		}*/
    }
}

void GameScene::activateWorldCollisions(const std::shared_ptr<ObstacleWorld>& world) {
    world->activateCollisionCallbacks(true);
    world->onBeginContact = [this](b2Contact* contact) {
        beginContact(contact);
    };
    world->beforeSolve = [this](b2Contact* contact, const b2Manifold* oldManifold) {
        beforeSolve(contact, oldManifold);
    };
}


#pragma mark -
#pragma mark Level Layout

/**
 * Resets the status of the game so that we can play again.
 *
 * This method disposes of the world and creates a new one.
 */
void GameScene::reset() {
    
    _level->clear();
    
    /*if (_actions->isActive("newton_idle")) { // TODO need to clear action manager on reset or level switch!
     _actions->remove("newton_idle");
     }
     if (_actions->isActive("newton_shoot"))
     _actions->remove("newton_shoot");*/
    //_actions->dispose();
    
    setComplete(false);
    //_level->initLevel(_allObjects);
    loadLevel(_curLevel);
}

std::shared_ptr<Obstacle> GameScene::AddHitbox(std::shared_ptr<PhysicalObject> object, Vec2 initPos, Size &size,
                                               int idx, int version, std::shared_ptr<Node> debugnode) {
    object->createHitbox(initPos, size);
    return nullptr;
}

#pragma mark -
#pragma mark Physics Handling

void GameScene::switchPauseState() {
	_paused = !_paused;

	if (!_paused) _input.resetTouch();

	if (!_paused) { //if we are now unpaused
		auto pauseMenu = _level->getPauseMenu();
        _level->getquitButton()->deactivate();

		pauseMenu->setVisible(false);
	}
	else { // if we are now paused
		auto pauseMenu = _level->getPauseMenu();
        _level->getquitButton()->activate(16);

		pauseMenu->setVisible(true);
	}
}

/**
 * Executes the core gameplay loop of this world.
 *
 * This method contains the specific update code for this mini-game. It does
 * not handle collisions, as those are managed by the parent class WorldController.
 * This method is called after input is read, but before collisions are resolved.
 * The very last thing that it should do is apply forces to the appropriate objects.
 *
 * @param  delta    Number of seconds since last animation frame
 */
void GameScene::update(float dt) {
	//CULog("game scene update\n");
    // increment the number of frames
    _numFrames++;
    
    // start by querying if the level was finished.
    // we do this first so the effect is: level finished -> draw frame -> end level logic next frame
    
    //if (_complete) {
    // level complete logic
    //    loadLevel(_curLevel + 1);
    //}
    
    _input.update(dt);
    
	// handle pausing
	if (_input.didPause()) { CULog("did pause\n"); switchPauseState(); }
	if (_paused) { CULog("paused\n"); return; }
    if (_LostLevel) {
		CULog("lost level\n");
        if (_Retry){ //retry
            _quitButton->setVisible(true);
            reset();
            _LostLevel = false;
            return;
        } else if (_QuitToMenuGameover){ //main menu
            CULog("Quit To Menu");
            _QuitToMenu = true;
            _loseMenu->setVisible(false);
            _resetButton ->deactivate();
            _menuButton->deactivate();
            _LostLevel = false;
            return;
        } else {
            return;
        }
    }

    // Process the toggled key commands
    if (_input.didDebug()) { setDebug(!isDebug()); } //THIS MAY CAUSE THE ISSUE FOR OUR GAME
    if (_input.didReset()) { reset(); return; }
    if (_input.didExit())  {
        CULog("Shutting down");
        Application::get()->quit();
    }
    
	// follow character
    camera->lazyFollow(_rocket.get(), _allObjects, _level->bgSprite);
    
    
    // Base impulse
    cugl::Vec2 dispVec = cugl::Vec2::ZERO;
    //create projectile
    bool isTouch = checkTouch(&dispVec);
    
    // prep update args    
	do_logic_args_t args;
	args.actions = _actions;
	args.assets = _assets.get();
	args.dt = dt;
	args.isTouch = isTouch;
	args.touchLoc = dispVec;
	args.level = _level.get();
    
    // process enemy actions
    for (int i = 0; i < _allObjects->getSize(); i++){
        std::shared_ptr<PhysicalObject> object = _allObjects->get(i);
        
		object->updateLogic(&args);

        if (object->getIsEnemy()) {
			
            AI_args_t args;
            args.assets = _assets.get();
            args.enemy = (void*)object.get();
            args.player = (void*)_rocket.get();
            args.objContainer = (void*)_allObjects.get();
            args.world = _world.get();
            args.debugNode = _debugnode;
            args.staticPosChange = camera->getPhysPos();
            
			object->processBehavior(&args);
            
            if (args.doAttack) {
                for (int j = 0; j < args.numAttacks; j++) {
                    _level->AddProjectile(args.atk[j], args.atk[j].velocity);
                    auto source = _assets->get<Sound>(ENEMY_SHOOT);
                    std::string key = "ENEMY_SHOOT";
                    if (!AudioEngine::get()->isActiveEffect(key)) {
                        AudioEngine::get()->playEffect(key, source, false, 0.1);
                    }
                }
            }
            
			object->updateTexture(args.doAttack, _level->getGoal()->isComplete(), _actions, dt);
        }
		else if (!object->isPlayer()) { // objects
			object->updateTexture(false, false, _actions, dt);
		}

		// check removal
		markObjRemoval(object->canRemove(), object->getIdx(), object->getVersion(), PossibleTypes::Projectile);

		object->procDeactivations(_world, _worldnode); // deactivate bonus hitboxes. TODO: bad

    }
    
	// finish level, don't move
	if (_level->getGoal()->isComplete()) {
		_rocket->setLinearVelocity(cugl::Vec2::ZERO);
	}
    
    // Turn the physics engine crank.
    _level->getWorld()->update(dt);
    

	//_rocket->procPhysCollisions();


    // Step animations
    _actions->update(dt);
    

	// remove objects that were marked for removal
    while (deadQueue.size() != 0) {
        
        type_idx_t toRemove = deadQueue.front();
        deadQueue.pop();
        
        // check that we haven't removed object due to another collision in this frame
        // note that this can happen somewhat frequently and will cause an exception
        std::shared_ptr<PhysicalObject> objToRemove = _allObjects->lookup(toRemove.idx, toRemove.ver);
        if (objToRemove != nullptr){
            // reset game if newton dies
            if (objToRemove->isPlayer()) {
                // TODO: ADD YOU LOSE SCENE
                _loseMenu->setVisible(true);
                _quitButton->deactivate();
                _quitButton->setVisible(false);
                _menuButton->activate(18);
                _resetButton->activate(19);
                _LostLevel = true;
//                reset();
//                _loseMenu->setVisible(false);
//                return;
            }
			
			
            
            physicsRemoval(objToRemove);
            
            removeFromScreen(objToRemove);
            
            _allObjects->remove(toRemove.idx, toRemove.ver);
            //_allObjects[toRemove.idx] = nullptr;
        }
        
    }
    


	if (_rocket->getPermissionToMoveOn())
		_complete = _level->getGoal()->isComplete();
    
}


bool GameScene::checkTouch(cugl::Vec2 *touchLoc) {
    if (_input.didTouch()) {
        *touchLoc = _input.getTouchLoc();
        _input.resetTouch();
        return true;
    }
    else {
        return false;
    }
}



/**
 * Processes the start of a collision
 *
 * This method is called when we first get a collision between two objects.  We use
 * this method to test if it is the "right" kind of collision.  In particular, we
 * use it to test if we make it to the win door. (this does not handle overlap, just the first time collision happens
 *
 * @param  contact  The two bodies that collided
 */
void GameScene::beginContact(b2Contact* contact) {
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();
    /*cugl::SimpleObstacle *obj1 = (cugl::SimpleObstacle *)body1->GetUserData();
     cugl::SimpleObstacle *obj2 = (cugl::SimpleObstacle *)body2->GetUserData();
     int idx1 = obj1->getIdx();
     int idx2 = obj2->getIdx();
     int ver1 = obj1->getVersion();
     int ver2 = obj2->getVersion();
     
     
     // if hitboxes apart of same object don't do collision
     if ((idx1 == idx2) && (ver1 == ver2)) {
     //CULog("reject collision\n");
     contact->SetEnabled(false);
     return;
     }*/
    
    // want another collision method to check other stuff here?
    
    
    
    // If we hit the "win" door, we are done
    /*if((body1->GetUserData() == _rocket.get() && body2->GetUserData() == _goalDoor.get()) ||
     (body1->GetUserData() == _goalDoor.get() && body2->GetUserData() == _rocket.get())) {
     setComplete(true);
     }*/
    
    
    
    
    
}

bool rayCastHit;
cugl::Vec2 edgeNormal;
cugl::Vec2 edgePoint;
float frac;
bool tryAgain;
bool GameScene::rayCast(cugl::Vec2 from, cugl::Vec2 towards, int targIdx, int targVer/*b2Fixture *targFixture*/){
	// ray cast to check if anything in between, otherwise do pathfind (note lambda cannot modify local variable)
	rayCastHit = false;
	edgePoint = from;
	

	do {
		frac = 10.0f;	
		tryAgain = false;

		_world->rayCast([=](b2Fixture *fixture, const Vec2 &point, const Vec2 &normal, float fraction) {
			b2Body* body = fixture->GetBody();
			cugl::SimpleObstacle *obj = (cugl::SimpleObstacle *)body->GetUserData();
			int idx = obj->getIdx();
			int ver = obj->getVersion();
			CULog("hit %d %d %f pt %f %f", idx, ver, frac, point.x, point.y);
			edgePoint.x = point.x + 0.01f;
			edgePoint.y = point.y + 0.01f;

			if (idx == targIdx && ver == targVer && fraction < frac) {
				//if (fixture == targFixture && fraction < frac){
					// hard copy
				edgeNormal.x = normal.x;
				edgeNormal.y = normal.y;		
				rayCastHit = true;
				frac = fraction;
				return 0.0f;
			}
			// we hit another object so try to raycast again
			else {
				//tryAgain = true;
				return -1.0f;
			}

			//return fraction;
		}, edgePoint, towards);
	} while (tryAgain);

	//CULog("frac %f\n", frac);
	return rayCastHit;
}


/**
 * Handles any modifications necessary before collision resolution
 *
 * This method is called just before Box2D resolves a collision.  We use this method
 * to implement sound on contact, using the algorithms outlined in Ian Parberry's
 * "Introduction to Game Physics with Box2D".
 *
 * @param  contact      The two bodies that collided
 * @param  oldManfold      The collision manifold before contact
 */
void GameScene::beforeSolve(b2Contact* contact, const b2Manifold* oldManifold) {
    // PhysicalObject is a child class to cugl::Obstacle so can't do this cast
    b2Body* body1 = contact->GetFixtureA()->GetBody();
    b2Body* body2 = contact->GetFixtureB()->GetBody();
    cugl::SimpleObstacle *obj1 = (cugl::SimpleObstacle *)body1->GetUserData();
    cugl::SimpleObstacle *obj2 = (cugl::SimpleObstacle *)body2->GetUserData();
    int idx1 = obj1->getIdx();
    int idx2 = obj2->getIdx();
    int ver1 = obj1->getVersion();
    int ver2 = obj2->getVersion();
    
    // if hitboxes apart of same object don't do collision
    if ((idx1 == idx2) && (ver1 == ver2)) {
        //CULog("reject collision\n");
        contact->SetEnabled(false);
        return;
    }
    
	/*Vec2 sf = Vec2(contact->GetManifold()->localNormal.x, contact->GetManifold()->localNormal.y);
	if (_rocket->getIdx() == idx1 || _rocket->getIdx() == idx2)
		CULog("netwon %d %d __ curr %d %d & %d %d w/ norm %f %f", _rocket->getIdx(), _rocket->getVersion(), idx1, ver1, idx2, ver2, sf.x, sf.y);
    */
    float speed = 0;
    
    // Use Ian Parberry's method to compute a speed threshold
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2PointState state1[2], state2[2];
    b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());
    for(int ii =0; ii < 2; ii++) {
        if (state2[ii] == b2_addState) {
            b2Vec2 wp = worldManifold.points[0];
            b2Vec2 v1 = body1->GetLinearVelocityFromWorldPoint(wp);
            b2Vec2 v2 = body2->GetLinearVelocityFromWorldPoint(wp);
            b2Vec2 dv = v1-v2;
            speed = b2Dot(dv,worldManifold.normal);
        }
    }
    
    // Play a sound if above threshold
    if (speed > SOUND_THRESHOLD) {
        // These keys result in a low number of sounds.  Too many == distortion.
        std::string key = ((Obstacle*)body1->GetUserData())->getName()+((Obstacle*)body2->GetUserData())->getName();
        auto source = _assets->get<Sound>(COLLISION_SOUND);
        if (!AudioEngine::get()->isActiveEffect(key)) {
            AudioEngine::get()->playEffect(key, source, false, source->getVolume());
        }
    }
    
    // this is NOT the surface normal (actually neither of the 2 surface normals) 
	// this is the shortest direction you should move so bodies aren't touching (i.e. resolve the collision)
    //Vec2 resolutionNormalLocal = Vec2(contact->GetManifold()->localNormal.x, contact->GetManifold()->localNormal.y);
	Vec2 resolutionNormal = Vec2(worldManifold.normal.x, worldManifold.normal.y);

	/*
	if (resolutionNormal.x != resolutionNormalLocal.x && resolutionNormal.y != resolutionNormalLocal.y) {
		CULog("normals not equal!\n");
	}*/

    if (idx1 >= 0 && idx2 >= 0) {
        PhysicalObject *physObj1 = (_allObjects->lookup(idx1, ver1)).get();
        PhysicalObject *physObj2 = (_allObjects->lookup(idx2, ver2)).get();

		// for onattacked callback
		physObj1->setHitboxCol(obj1);
		physObj2->setHitboxCol(obj2);

        bool resolvePhys1 = physObj1->doCollide(physObj2);
        bool resolvePhys2 = physObj2->doCollide(physObj1);
        
        // TODO won't remove here?
        // Also still want to call logic collision
        bool invalidCollision = !resolvePhys1 || !resolvePhys2;

		// bad angle (remove unrealistic collision)
		//float angle1 = acosf(physObj1->getVelocity().getNormalization().dot(normal)


        if (invalidCollision) {
            contact->SetEnabled(false);
            //return;
        }
        
        bool invalidCollision2 = (physObj1->getOwner() == physObj2) || (physObj2->getOwner() == physObj1); // bullet can't collide with creator
        bool bothBullets = (body1->IsBullet() && body2->IsBullet()); // todo
        if (invalidCollision2 || bothBullets) {
            contact->SetEnabled(false);
            return;
        }
        
        // get edge normals for collision (doesn't seem to have much effect and just do for newton)

		// try using collision pts to do
		Vec2 edgeNormObj1 = resolutionNormal;
		Vec2 edgeNormObj2 = resolutionNormal;
		////if ((physObj1->isPlayer() || physObj2->isPlayer()) /*&&
		////	(physObj1->getIsEnemy() || physObj2->getIsEnemy())*/) {
		//	//CULog("resolution normal %f %f\n", resolutionNormal.x, resolutionNormal.y);
		//	if (!physObj1->getVelocity().isNearZero() && physObj1->isPlayer() && false) {
		//		CULog("try raycast\n");
		//		//physObj1->getBody(0)->setSensor(true);
		//		if (rayCast(physObj1->getPosition(), physObj1->getPosition() + physObj1->getVelocity().getNormalization()/*physObj2->getPosition()*/, idx2, ver2/*contact->GetFixtureB()*/)) {
		//			//CULog("detect1 norm %f %f obj pos %f %f, col pos %f %f\n", edgeNormal.x, edgeNormal.y,
		//			//	physObj1->getPosition().x, physObj1->getPosition().y, edgePoint.x, edgePoint.y);
		//			
		//			float dot = physObj1->getVelocity().getNegation().getNormalization().dot(edgeNormal.getNormalization());
		//			float angle = acosf(dot);
		//			CULog("%f\n", angle);
		//			if (angle > PI / 2) edgeNormal *= -1.0f;

		//			edgeNormObj2 = edgeNormal;
		//			//if ()
		//		}
		//		//physObj1->getBody(0)->setSensor(false);
		//	}
		//	if (!physObj2->getVelocity().isNearZero() && physObj2->isPlayer() && false) {
		//		CULog("try raycast\n");
		//		//physObj2->getBody(0)->setSensor(true);
		//		if (rayCast(physObj2->getPosition(), physObj2->getPosition() + physObj2->getVelocity().getNormalization()/*physObj1->getPosition()*/, idx1, ver1/*contact->GetFixtureA()*/)) {
		//			//CULog("detect2 %f %f\n", edgeNormal.x, edgeNormal.y);
		//			//CULog("detect2 norm %f %f obj pos %f %f, col pos %f %f\n", edgeNormal.x, edgeNormal.y,
		//			//	physObj2->getPosition().x, physObj2->getPosition().y, edgePoint.x, edgePoint.y);
		//			float dot = physObj2->getVelocity().getNegation().getNormalization().dot(edgeNormal.getNormalization());
		//			float angle = acosf(dot);
		//			CULog("%f\n", angle);
		//			if (angle > PI / 2) edgeNormal *= -1.0f;

		//			edgeNormObj1 = edgeNormal;
		//		}
		//		//physObj2->getBody(0)->setSensor(false);
		//	}
		////}

		// can also try checking the collision point compared to start point and see if makes sense
		// add a method in physcollision to get surface normal if available (just out in direction of other object)

        physObj1->onCollide(physObj2, obj1, obj2, edgeNormObj2, invalidCollision, _numFrames);
        physObj2->onCollide(physObj1, obj1, obj2, edgeNormObj1, invalidCollision, _numFrames);
        
        
        
        /*markObjRemoval(physObj1->canRemove(), idx1, ver1, PossibleTypes::Projectile);
        markObjRemoval(physObj2->canRemove(), idx2, ver2, PossibleTypes::Projectile);*/
    }
    
}

void GameScene::markObjRemoval(bool removeCondition, int idx, int version, PossibleTypes objType) {
    if (removeCondition) {
        type_idx_t toRemove;
        toRemove.idx = idx;
        toRemove.ver = version;
        toRemove.type = objType;
        deadQueue.push(toRemove);
        
        //std::string nodeName = "projectile_" + std::to_string(idx);
        //CULog("bullet collision %s\n", nodeName.c_str());
    }
}


// remove node from the scene, this remove image and object from scene graph, but does not disable physics
void GameScene::removeFromScreen(std::shared_ptr<PhysicalObject> object) {
    std::string nodeName = object->getSceneGraphNode()->getName();
    
    //_rootnode->removeChildByName(nodeName); // TODO: can't remove node by id
    //_debugnode->removeChildByName(nodeName); //? doesn't work
    _worldnode->removeChildByName(nodeName); // TODO: can't remove node by id
    //_worldnode->child
    //_worldnode->removeChild(object->getSceneGraphNode());
    
    object->dispose();
    object = nullptr;
    
    
}

void GameScene::physicsRemoval(std::shared_ptr<PhysicalObject> object) {
    //object->markRemoved(true);
    //_world->removeObstacle(object.get());
    /*auto hitboxes = object->getHitbox();
     for (int i = 0; i < hitboxes.size(); i++) {
     _world->removeObstacle(hitboxes[i].get());
     }*/
    
    object->removeHitboxes(_world, _worldnode);
    
    //_world->removeObstacle(object->getHitbox().get());
    object->setDebugScene(nullptr);
}
