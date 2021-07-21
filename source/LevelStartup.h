#ifndef __LEVEL_STARTUP_H__
#define __LEVEL_STARTUP_H__

#include <vector>
#include <cugl/cugl.h>
#include <cugl/assets/CUAsset.h>
#include "CharacterModel.h"
#include "Projectile.h"
#include "ObjectContainer.h"
#include "TouchSplinePhysics.h"
#include "GoalCollision.h"
#include "Constants.h"

using namespace cugl;

typedef struct objectinit {
	int hitboxId;
	int numNodes;
	const char *textures[MAX_NODES_PER_HITBOX];
	int zOrders[MAX_NODES_PER_HITBOX];
	int filmStripRow[MAX_NODES_PER_HITBOX];
	int filmStripCol[MAX_NODES_PER_HITBOX];
	Vec2 physOffsets[MAX_NODES_PER_HITBOX];
	Vec2 relativeScale[MAX_NODES_PER_HITBOX];
} nodesInit_t;

class LevelStartup : public Asset {
private:
	

	/** root node of level*/
	std::shared_ptr<Node> _root;

	std::shared_ptr<cugl::JsonValue> _json;

	float pathToBox2d;

	Rect _bounds;
	Vec2 _gravity;
	float _scale;

	/** reference to the physics root of the scene graph */
	std::shared_ptr<Node> _worldnode;
	/** reference to the debug root of the scene graph*/
	std::shared_ptr<Node> _debugnode;

	std::shared_ptr<ObstacleWorld> _world;
	std::shared_ptr<CharacterModel> _player;
	std::shared_ptr<GoalCollision> _goal;

	std::shared_ptr<PolygonNode> _pauseMenu;
    
    std::shared_ptr<PolygonNode> _resumeBut;
    std::shared_ptr<PolygonNode> _resetBut;
    std::shared_ptr<PolygonNode> _quitBut;

	/*width and height of the background image, in pixels*/
	float bgWidth;
	float bgHeight;

	int _numObjects;

	/*width and height of the box2d world, in box2d units*/
	float box2dWidth;
	float box2dHeight;

	std::shared_ptr<cugl::AssetManager> _assets;

	bool loadPlayer(const std::shared_ptr<JsonValue> json);
	bool loadEnemies(const std::shared_ptr<JsonValue> json);
	bool loadBoundaries(const std::shared_ptr<JsonValue> json);
	bool loadHazards(const std::shared_ptr<JsonValue> json);
	bool loadCircleZones(const std::shared_ptr<JsonValue> json);
	bool loadBackground(const std::shared_ptr<JsonValue> json);
	bool loadGoal(const std::shared_ptr<JsonValue> json);

	void addObstacle(const std::shared_ptr<cugl::Obstacle>& obj,
		const std::shared_ptr<cugl::Node>& node,
		int zOrder);

	void clearRootNode();

	bool _debug;

	std::shared_ptr<SplinePhysics> phys;

public:
	//TODO: remove these references
	std::shared_ptr<PolygonNode> bgSprite;
	std::shared_ptr<CharacterModel> _enemy;
	std::shared_ptr<CharacterModel> _enemy2;

	std::shared_ptr<ObjectContainer> _allObjects;
    
    std::shared_ptr<cugl::Button> _resumeButton;
    std::shared_ptr<cugl::Button> _resetButton;
    std::shared_ptr<cugl::Button> _quitButton;


#pragma mark : Getters
	const std::shared_ptr<ObstacleWorld>& getWorld() { return _world; }

	std::shared_ptr<CharacterModel> getPlayer() { return _player; }

	std::shared_ptr<GoalCollision> getGoal() { _goal->updateGoal(); return _goal; }

	std::shared_ptr<Node> getDebugNode() { return _debugnode; }

	std::shared_ptr<Node> getWorldNode() { return _worldnode; }

	std::shared_ptr<PolygonNode> getPauseMenu() { return _pauseMenu; }
    
    std::shared_ptr<cugl::Button> getquitButton() { return _quitButton; }

	bool isDebug() { return _debug; }

	float getBgWidth() { return bgWidth; }
	float getBgHeight() { return bgHeight; }
	float getBox2dWidth() { return box2dWidth; }
	float getBox2dHeight() { return box2dHeight; }

	// delet this
	std::shared_ptr<cugl::JsonValue> getJSON() { return _json; }

#pragma mark : Setters
	void setAssets(const std::shared_ptr<AssetManager>& assets) { _assets = assets; }
	
	void setRootNode(const std::shared_ptr<Node>& root);

	void setDebug(bool value) { _debug = value; _debugnode->setVisible(value); }

#pragma mark : Helpers
	template <class T>
	void AddGameObject(std::shared_ptr<T> object, Vec2 objScale,
		std::vector<nodesInit_t> hitboxWithNodes);
	//void AddGameObject(std::shared_ptr<T> object, const char *texKey, int zOrder, Vec2 objScale);
	/*template <class T>
	void AddGameObject(std::shared_ptr<T> object, const char *texKey, int zOrder, Vec2 objScale);*/
	
	void AppendNodeToObject(PhysicalObject *object, Vec2 objScale, nodesInit_t hitboxesWithNodes, std::string name);


	bool AddProjectile(const attackInfo_t projInfo, cugl::Vec2 dispVec);

	Size getPhysicsSize(const char *texKey, Vec2 objScale);
	
#pragma mark : Constructors/Destructors

	LevelStartup(void);

	virtual ~LevelStartup(void);

	virtual bool preload(const std::string& file) override;

	virtual bool preload(const std::shared_ptr<cugl::JsonValue>& json) override;

	void preinit();

	/**
	* Creates a new game level with no source file.
	*
	* The source file can be set at any time via the setFile() method. This method
	* does NOT load the asset.  You must call the load() method to do that.
	*
	* @return  an autoreleased level file
	*/
	static std::shared_ptr<LevelStartup> alloc() {
		std::shared_ptr<LevelStartup> result = std::make_shared<LevelStartup>();
		return (result->init("") ? result : nullptr);
	}

	/**
	* Creates a new game level with the given source file.
	*
	* This method does NOT load the level. You must call the load() method to do that.
	* This method returns false if file does not exist.
	*
	* @return  an autoreleased level file
	*/
	static std::shared_ptr<LevelStartup> alloc(std::string file) {
		std::shared_ptr<LevelStartup> result = std::make_shared<LevelStartup>();
		return (result->init(file) ? result : nullptr);
	}

	bool initLevel();

	bool initLevel(std::shared_ptr<ObjectContainer> allObjects);

	void initPauseMenu();

	void unload();

	void clear() {
		_world->clear();
		_player->dispose();
		_worldnode->removeAllChildren();
		_debugnode->removeAllChildren();
	}

};

#endif
