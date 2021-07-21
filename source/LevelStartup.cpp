#include <cugl/assets/CUJsonLoader.h>
#include "LevelStartup.h"
#include "Constants.h"
#include "Levels.h"

#include "BasicPhysics.h"
#include "SplinePhysics.h"
#include "TouchSplinePhysics.h"
#include "VelocityPhysics.h"
#include "Camera2d.h"
#include "Polyzone.h"
#include "ProjectileAttack.h"
#include "Explode.h"
#include "Shock.h"
#include "PassCollision.h"
#include "DOTCollision.h"
#include "HighSpeedCollision.h"
#include "StandardDefense.h"
#include "ShieldDefense.h"
#include "SpeedArmor.h"
#include "ChangeSpeedCollision.h"
#include "Animation.h"
#include "NewtonAnimation.h"
#include "AttackIdleAnimation.h"
#include "AttackMoveAnimation.h"
#include "ChaseBossAnim.h"
#include "HealthBarAnimation.h"
#include "ExclaimAnimation.h"
#include "RepeatAnimation.h"
#include "OnHitAnimation.h"
#include "ShotRejectAnimation.h"
#include "BasicAI.h"
#include "FencedAI.h"
#include "PathFollowAI.h"
#include "ShieldRegen.h"
#include "SwitchWeapons.h"
#include "NewtonUpdate.h"
#include "NewtonDamageAnimation.h"
#include "EliminateGoal.h"
#include "LockGoal.h"


/**
* Creates a new, empty level.
*/
LevelStartup::LevelStartup(void) : Asset(),
_root(nullptr),
_world(nullptr),
_worldnode(nullptr),
_debugnode(nullptr),
_player(nullptr),
_json(nullptr)
{
	_bounds.size.set(1.0f, 1.0f);
}

/**
* Destroys this level, releasing all resources.
*/
LevelStartup::~LevelStartup(void) {
	unload();
	clearRootNode();
}

void LevelStartup::clearRootNode() {
	if (_root == nullptr) {
		return;
	}
	_worldnode->removeFromParent();
	_worldnode->removeAllChildren();
	_worldnode = nullptr;

	_debugnode->removeFromParent();
	_debugnode->removeAllChildren();
	_debugnode = nullptr;

	_root = nullptr;
}

void LevelStartup::setRootNode(const std::shared_ptr<Node>& node) {
	if (_root != nullptr) {
		clearRootNode();
	}

	_root = node;

	_worldnode = Node::alloc();
	_worldnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	_worldnode->setPosition(Vec2::ZERO);

	_debugnode = Node::alloc();
	_debugnode->setScale(_scale); // Debug node draws in PHYSICS coordinates
	_debugnode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
	_debugnode->setPosition(Vec2::ZERO);

	_root->addChild(_worldnode, 0);
	_root->addChild(_debugnode, 1);
}

bool LevelStartup::preload(const std::string& file) {
	std::shared_ptr<JsonReader> reader = JsonReader::allocWithAsset(file);
	return preload(reader->readJson());
}

bool LevelStartup::preload(const std::shared_ptr<cugl::JsonValue>& json) {
	if (json == nullptr) {
		CUAssertLog(false, "Failed to load level file");
		return false;
	}

	_json = json;

	_scale = (1024.0f / 32.0f);

	//_world = ObstacleWorld::alloc(Rect(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT), Vec2(0, DEFAULT_GRAVITY));

    return true;
	
}

void LevelStartup::preinit() {
	auto imagejson = _json->get("level_image");
	std::string imageStr = imagejson->asString();
	std::shared_ptr<Texture> image = _assets->get<Texture>(imageStr.c_str());

	//std::shared_ptr<PolygonNode> bgSprite = PolygonNode::allocWithTexture(image);
	bgSprite = PolygonNode::allocWithTexture(image);

	// these variables are replacements for IMAGE_WIDTH and DEFAULT_WIDTH
	bgHeight = bgSprite->getTexture()->getSize().height;
	bgWidth = bgSprite->getTexture()->getSize().width;
	box2dHeight = bgHeight * 32.0f / 1024.0f;
	box2dWidth = bgWidth * 32.0f / 1024.0f;

	_world = ObstacleWorld::alloc(Rect(0, 0, box2dWidth, box2dHeight), Vec2(0, DEFAULT_GRAVITY));

	phys = std::make_shared<SplinePhysics>(_scale, Application::get()->getDisplaySize().getIHeight(), RECOIL_MOD, BOUNCE_MOD, true);
	phys->createSpline();
}

bool LevelStartup::initLevel() {
	CULog("in init level");
	_allObjects = std::shared_ptr<ObjectContainer>(new ObjectContainer(MAX_OBJECTS));

	return LevelStartup::initLevel(_allObjects);
}

bool LevelStartup::initLevel(std::shared_ptr<ObjectContainer> allObjects) {
	pathToBox2d = box2dWidth/PATH_WIDTH;
	_allObjects = allObjects;
	_goal = nullptr;

	// intialize num objects for tracking
	_numObjects = 0;

	initPauseMenu();

	//init player
	if (!loadPlayer(_json)) {
		CUAssertLog(false, "Failed to load player");
		return false;
	}

	CULog("managed to load player");

	//init enemies
	if (!loadEnemies(_json)) {
		CUAssertLog(false, "Failed to load enemies");
		return false;
	}

	CULog("managed to load enemies");

	//init boundaries
	if (!loadBoundaries(_json)) {
		CUAssertLog(false, "Failed to load boundaries");
		return false;
	}

	CULog("managed to load boundaries");

	//init hazards
	if (!loadHazards(_json)) {
		CUAssertLog(false, "Failed to load hazards");
		return false;
	}

	CULog("managed to load hazards");

	//init zones
	if (!loadCircleZones(_json)) {
		CUAssertLog(false, "Failed to load zones");
		return false;
	}

	CULog("managed to load zones");

	//init background
	if (!loadBackground(_json)) {
		CUAssertLog(false, "Failed to load background");
		return false;
	}

	CULog("managed to load background");

	//init goal
	if (!loadGoal(_json)) {
		CUAssertLog(false, "Failed to load goal");
		return false;
	}

	CULog("managed to load goal");

	return true;
}

void LevelStartup::initPauseMenu() {
	_pauseMenu = std::make_shared<PolygonNode>();
	_pauseMenu->initWithTexture(_assets->get<Texture>("pause_bg"));

	_worldnode->addChild(std::dynamic_pointer_cast<Node, PolygonNode>(_pauseMenu), 100);

	_pauseMenu->setVisible(false);
    
    _quitBut = std::make_shared<PolygonNode>();
    _quitBut->initWithTexture(_assets->get<Texture>("red_circle"));
    _quitBut->setPosition(0, 0);
        
    _quitButton = std::make_shared<cugl::Button>();
    _quitButton->init(_quitBut);

    _quitButton->setListener([=](const std::string& name, bool down) {
        CULog("kehbdkjn");
    });
    
    _pauseMenu->addChild(_quitButton);
    
//    _resumeButton = std::dynamic_pointer_cast<Button>(_assets->get<Node>("select_back_level1"));
//    _resetButton = std::dynamic_pointer_cast<Button>(_assets->get<Node>("select_back_level2"));
//    _quitButton = std::dynamic_pointer_cast<Button>(_assets->get<Node>("select_back_level3"));
    
}

#define TEMP_NEWTON_SCALE 0.9f

bool LevelStartup::loadPlayer(const std::shared_ptr<JsonValue> json) {
	bool success;
	auto playerPos = json->get("player");

	if (success = (playerPos != nullptr)) {
		success = success && playerPos->isArray();
		Vec2 playerPosPhys = Vec2(playerPos->get(0)->asFloat()*pathToBox2d, playerPos->get(1)->asFloat()*pathToBox2d);
		Vec2 physScale = getPhysicsSize("newton_0", Vec2(TEMP_NEWTON_SCALE * 0.20f, TEMP_NEWTON_SCALE * 0.15f));
		// allocate newton with two hitboxes for testing (maybe this structure somehow specified in json)
		// TODO rotation support
		hitboxes_t newtonHitboxes;
		newtonHitboxes.center = playerPosPhys;
		newtonHitboxes.shapeType.push_back(ELLIPSE_HITBOX);
		newtonHitboxes.offsets.push_back(Vec2(0.0f, 0.0f));
		newtonHitboxes.sizes.push_back(physScale);
		newtonHitboxes.physCol.push_back(nullptr);
		newtonHitboxes.logicCol.push_back(nullptr);
		newtonHitboxes.defCol.push_back(nullptr);

		/*newtonHitboxes.offsets.push_back(Vec2(newtonHitboxes.sizes[0]) / 2.0f);
		newtonHitboxes.sizes.push_back(getPhysicsSize("newton_0", Vec2(NEWTON_SCALE / 2.0f, NEWTON_SCALE / 2.0f)));
		newtonHitboxes.physCol.push_back(nullptr);
		newtonHitboxes.logicCol.push_back(nullptr);
		newtonHitboxes.defCol.push_back(nullptr);*/


		_player = CharacterModel::alloc(&newtonHitboxes);

		//const char *texKeys[1] = { "newton_0" };
		/*const char *texKeys[1] = { "newton_idle_8" };
		int zOrders[1] = { CHARACTER_Z_ORDER };
		int hitboxIdxs[1] = { 0};
		int isFilmstrip[2] = { 8, 14 };
		//int isFilmstrip[2] = { 1, 1 };
		bool hasBody[2] = { true, true };
		int numNodes = 1;*/

		Vec2 scale = Vec2(TEMP_NEWTON_SCALE, TEMP_NEWTON_SCALE);


		

		// init nodes for hitbox 0
		std::vector<nodesInit_t> nodes;
		nodesInit_t node;
		node.hitboxId = 0;
		node.numNodes = 8;
		//weird order b/c anim framework not set up
		node.textures[0] = "newton_idle_8"; node.textures[1] = "speed_shadow";
			node.textures[2] = "newton_shoot"; //node.textures[3] = "newton_shoot"; node.textures[4] = "newton_shoot";
		node.filmStripRow[0] = 16; node.filmStripRow[1] = 1;
			node.filmStripRow[2] = 16; //node.filmStripRow[3] = 16; node.filmStripRow[4] = 16;
		node.filmStripCol[0] = 12; node.filmStripCol[1] = 13;
			node.filmStripCol[2] = 6; //node.filmStripCol[3] = 6; node.filmStripCol[4] = 6;
		node.zOrders[0] = CHARACTER_Z_ORDER; node.zOrders[1] = SHADOW_Z_ORDER;
			node.zOrders[2] = CHARACTER_Z_ORDER; //node.zOrders[3] = CHARACTER_Z_ORDER; node.zOrders[4] = CHARACTER_Z_ORDER;
		node.physOffsets[0] = Vec2(0.0f, 0.0f); node.physOffsets[1] = Vec2(0.0f, getPhysicsSize("newton_shadow", scale).height * 0.1f);
			node.physOffsets[2] = Vec2(0.0f,0.0f);  //node.physOffsets[3] = Vec2(0.0f, 0.0f); node.physOffsets[4] = Vec2(0.0f, 0.0f);
		node.relativeScale[0] = SAME_SCALE; node.relativeScale[1] = Vec2(0.8f,0.8f); node.relativeScale[2] = SAME_SCALE;
		//node.relativeScale[3] = SAME_SCALE; node.relativeScale[4] = SAME_SCALE; 

		node.textures[3] = "health_bar_top";
		node.filmStripRow[3] = 1;
		node.filmStripCol[3] = 1;
		node.zOrders[3] = HP_TOP_Z_ORDER;
		node.physOffsets[3] = Vec2(0.0f, physScale.y/2.0f + HITBOX_Y_OFFSET);
		node.relativeScale[3] = SAME_SCALE;

		node.textures[4] = "health_bar_bot";
		node.filmStripRow[4] = 1;
		node.filmStripCol[4] = 1;
		node.zOrders[4] = HP_BOT_Z_ORDER;
		node.physOffsets[4] = Vec2(0.0f, physScale.y / 2.0f + HITBOX_Y_OFFSET);
		node.relativeScale[4] = SAME_SCALE;

		// speed exclaimation
		node.textures[5] = "reject_x";
		node.filmStripRow[5] = 1;
		node.filmStripCol[5] = 2;
		node.zOrders[5] = HP_TOP_Z_ORDER;
		node.physOffsets[5] = Vec2(0.0f, physScale.y / 2.0f + 2.0f * HITBOX_Y_OFFSET);
		node.relativeScale[5] = SAME_SCALE / 5.0f;

		node.textures[6] = "newton_death";
		node.filmStripRow[6] = 16;
		node.filmStripCol[6] = 6;
		node.zOrders[6] = CHARACTER_Z_ORDER;
		node.physOffsets[6] = Vec2(0.0f, 0.0f);
		node.relativeScale[6] = SAME_SCALE;

		// newton hit flash
		node.textures[7] = "healthbar_flash";
		node.filmStripRow[7] = 1;
		node.filmStripCol[7] = 1;
		node.zOrders[7] = HP_FLASH_Z_ORDER;
		node.physOffsets[7] = Vec2(0.0f, physScale.y / 2.0f + HITBOX_Y_OFFSET);
		node.relativeScale[7] = SAME_SCALE;

		// correct for filmstrip being offcenter
		Vec2 nodeOffset = Vec2(0.0f, physScale.y * 0.5f);
		nodeOffset *= -0.35f;
		for (int i = 0; i < 8; i++) {
			node.physOffsets[i] += nodeOffset;
		}


		nodes.push_back(node);

		AddGameObject<CharacterModel>(_player, scale, nodes);

		/*auto anodes = _player->getAnimationNodes(0);
		CULog("wid %f %f\n", anodes[5]->getWidth(), anodes[5]->getHeight());
		anodes[5]->shiftPolygon(0.0f, -12.0f); // changes texture coordinates by given number of pixels (rounded)
		anodes[6]->shiftPolygon(0.0f, -12.0f);*/
		

		//_player->addAnimation("newton_", 1, ALLOWED_ORIENTATIONS/2, _assets);

		_player->setOrientations(ALLOWED_ORIENTATIONS);

		std::shared_ptr<Movement> moveDel = std::dynamic_pointer_cast<Movement, SplinePhysics>(phys);
		_player->delegateMovement(moveDel);

		std::shared_ptr<Movement> dummyMove = std::make_shared<Movement>();
		std::shared_ptr<Collision> dummyCol = std::make_shared<Collision>();
		std::shared_ptr<Explode> explode = std::make_shared<Explode>();

		std::shared_ptr<ProjectileAttack> attack = std::make_shared<ProjectileAttack>(NEWTON_PROJ_SPEED, NEWTON_FIRE_RATE, physScale.x/2.0f);
		attack->appendProjectile(NEWTON_PROJ_DMG, dummyMove, dummyCol, explode,
			Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, "newton_proj", 0.0f, 0.0f);
		//auto attackFP = [&](bool attackReq, PhysicalObject * obj, cugl::Vec2 attackDir) { return weapon->attack(attackReq, obj, attackDir); };
		_player->delegateAttack(std::dynamic_pointer_cast<Attack, ProjectileAttack>(attack));

		//auto collideFP = [&](PhysicalObject *obj, Vec2 normalVec) {phys->reflectResps(obj, normalVec); };
		//_rocket->delegateCollision(collideFP);
		_player->delegatePhysCollision(std::dynamic_pointer_cast<Collision, SplinePhysics>(phys));
		
		std::shared_ptr<HighSpeedCollision> speedDmg = std::make_shared<HighSpeedCollision>();
		_player->delegateLogicCollision(std::dynamic_pointer_cast<LogicCollision, HighSpeedCollision>(speedDmg));
		//_rocket->delegateDmgCollision(dummyCol);


		// give speed armor
		std::shared_ptr<SpeedArmor> speedDef = std::make_shared<SpeedArmor>();
		_player->delegateDefense(speedDef);


		std::shared_ptr<NewtonAnimation> anim = std::make_shared<NewtonAnimation>();
		anim->init("newton_idle", "newton_shoot", "newton_death", "newton_victory"); // unique name
		std::shared_ptr<Animation> hpAnim = std::make_shared<HealthBarAnimation>();
		std::shared_ptr<ExclaimAnimation> exAnim = std::make_shared<ExclaimAnimation>();
		exAnim->init("newton_exclaim");
		std::shared_ptr<NewtonDamageAnimation> hitFlashAnim = std::make_shared<NewtonDamageAnimation>(); 
		std::shared_ptr<ShotRejectAnimation> rejectAnim = std::make_shared<ShotRejectAnimation>();
		rejectAnim->init("newton_shot_reject");

		int assocNodes[10] = { 0, 2, 6 };
		_player->delegateAnimation(anim, 0, assocNodes, 3);
		assocNodes[0] = 3; assocNodes[1] = 4;
		_player->delegateAnimation(hpAnim, 0, assocNodes, 2);
		assocNodes[0] = 1;
		_player->delegateAnimation(exAnim, 0, assocNodes, 1);
		assocNodes[0] = 7;
		_player->delegateAnimation(hitFlashAnim, 0, assocNodes, 1);
		assocNodes[0] = 5;
		_player->delegateAnimation(rejectAnim, 0, assocNodes, 1);


		std::shared_ptr<NewtonUpdate> newtonUpdate = std::make_shared<NewtonUpdate>();
		_player->delegateUpdateLogic(newtonUpdate);

		_player->setPlayer(true);
		_player->setTotalHealth(NEWTON_HEALTH);
		_player->setCurrentHealth(NEWTON_HEALTH);
		_player->setPermissionToDie(false);
		_player->setPermissionToMoveOn(false);


		//auto animNodes = _player->getAnimationNodes(0);
		//CULog("animNodes %d %d\n", animNodes[0]->getZOrder(), animNodes[1]->getZOrder());
	}
	return success;
}

enum enemy_t {
	Seeker,
	ShieldSeeker,
	FixedShieldSeeker,
	FixedSeeker,
	Attack2Seeker,
	ShieldBoss,
	ChaseBoss
};

enemy_t evalEnemy(std::string enemyString) {
	if (enemyString == "seeker") return Seeker;
	if (enemyString == "shieldseeker") return ShieldSeeker;
	if (enemyString == "attack2seeker") return Attack2Seeker;
	if (enemyString == "fixed_shieldseeker") return FixedShieldSeeker;
	if (enemyString == "fixed_seeker") return FixedSeeker;
	if (enemyString == "shieldboss") return ShieldBoss;
	if (enemyString == "chaseboss") return ChaseBoss;
    return Seeker;
}

bool LevelStartup::loadEnemies(const std::shared_ptr<JsonValue> json) {
	bool success = false; 
	auto enemyList = json->get("enemy");
	
	if (success = (enemyList != nullptr)) {
		success = success && enemyList->isArray();
		for (int i = 0; i < enemyList->size(); i++) {
			std::shared_ptr<JsonValue> enemy = enemyList->get(i);
			
			enemy_t enemyType = evalEnemy(enemy->get("type")->asString());
			Vec2 enemyPos = Vec2(enemy->get("pos")->get(0)->asFloat()*pathToBox2d, enemy->get("pos")->get(1)->asFloat()*pathToBox2d);
			
			std::shared_ptr<CharacterModel> curEnemy;
			std::shared_ptr<BasicPhysics> enemyPhys;
			hitboxes_t newtonHitboxes;
			auto shieldDef = std::make_shared<ShieldDefense>();

			_numObjects++;

			if (enemyType == Seeker || enemyType == ShieldSeeker || enemyType == FixedSeeker || enemyType == FixedShieldSeeker || enemyType == Attack2Seeker ) {
				auto verts = enemy->get("pos")->asFloatArray();

				for (int i = 0; i < verts.size(); i += 2) {
					enemyPos = Vec2(verts[i] * pathToBox2d, verts[i + 1] * pathToBox2d);
					Vec2 physScale = getPhysicsSize("newton_0", Vec2(NEWTON_SCALE, NEWTON_SCALE));

					newtonHitboxes.center = enemyPos;
					newtonHitboxes.shapeType.push_back(ELLIPSE_HITBOX);
					newtonHitboxes.offsets.push_back(Vec2(0.0f, 0.0f));
					newtonHitboxes.sizes.push_back(physScale);
					newtonHitboxes.physCol.push_back(nullptr);
					newtonHitboxes.logicCol.push_back(nullptr);
					newtonHitboxes.defCol.push_back(nullptr);

					if (enemyType == ShieldSeeker || enemyType == FixedShieldSeeker) {
						Vec2 shieldPhysScale = getPhysicsSize("newton_0", Vec2(NEWTON_SCALE, NEWTON_SCALE) * 1.75f);
						newtonHitboxes.offsets.push_back(Vec2(0.0f, 0.0f));
						newtonHitboxes.shapeType.push_back(ELLIPSE_HITBOX);
						newtonHitboxes.sizes.push_back(shieldPhysScale);
						newtonHitboxes.physCol.push_back(nullptr); newtonHitboxes.logicCol.push_back(nullptr);
						newtonHitboxes.defCol.push_back(std::dynamic_pointer_cast<DefendCollision, ShieldDefense>(shieldDef));

					}

					curEnemy = CharacterModel::alloc(&newtonHitboxes);
					//numNodes = 1;
					Vec2 scale = Vec2(SEEKER_SCALE, SEEKER_SCALE);

					std::vector<nodesInit_t> nodes;
					nodesInit_t node;
					node.hitboxId = 0;
					node.numNodes = 4;
					node.textures[0] = "seeker_idle";
					node.filmStripRow[0] = 8;
					node.filmStripCol[0] = 10;
					node.zOrders[0] = CHARACTER_Z_ORDER;
					node.physOffsets[0] = Vec2(0.0f, 0.0f);
					node.relativeScale[0] = SAME_SCALE;


					node.textures[1] = "seeker_shoot";
					node.filmStripRow[1] = 8;
					node.filmStripCol[1] = 7;
					node.zOrders[1] = CHARACTER_Z_ORDER;
					node.physOffsets[1] = Vec2(0.0f, 0.0f);
					node.relativeScale[1] = SAME_SCALE;

					node.textures[2] = "health_bar_top";
					node.filmStripRow[2] = 1;
					node.filmStripCol[2] = 1;
					node.zOrders[2] = HP_TOP_Z_ORDER;
					node.physOffsets[2] = Vec2(0.0f, physScale.y / 2.0f + HITBOX_Y_OFFSET);
					node.relativeScale[2] = Vec2(1.0f, 1.0f);

					node.textures[3] = "health_bar_bot";
					node.filmStripRow[3] = 1;
					node.filmStripCol[3] = 1;
					node.zOrders[3] = HP_BOT_Z_ORDER;
					node.physOffsets[3] = Vec2(0.0f, physScale.y / 2.0f + HITBOX_Y_OFFSET);
					node.relativeScale[3] = Vec2(1.0f, 1.0f);

					if (enemyType == Attack2Seeker) {
						node.numNodes = 5;
						node.textures[4] = "shootme";
						node.filmStripRow[4] = 1;
						node.filmStripCol[4] = 1;
						node.zOrders[4] = HP_TOP_Z_ORDER;
						node.physOffsets[4] = Vec2(0.0f, 0.0f); // Vec2(0.0f, physScale.y / 2.0f + 2.0f * HITBOX_Y_OFFSET);
						node.relativeScale[4] = Vec2(0.5f, 0.5f);
					}

					nodes.push_back(node);

					if (enemyType == ShieldSeeker || enemyType == FixedShieldSeeker) {
						nodesInit_t node1; node1.hitboxId = 1; node1.numNodes = 1; node1.textures[0] = "shield"; node1.filmStripRow[0] = 1; node1.filmStripCol[0] = 1; node1.zOrders[0] = SHIELD_Z_ORDER; node1.physOffsets[0] = Vec2(0.0f, 0.0f); node1.relativeScale[0] = SAME_SCALE * 0.23f;
						nodes.push_back(node1);
					}

					AddGameObject<CharacterModel>(curEnemy, scale, nodes);



					// add animations
					curEnemy->setOrientations(ALLOWED_ORIENTATIONS / 2);

					std::shared_ptr<AttackMoveAnimation> anim = std::make_shared<AttackMoveAnimation>();
					anim->init("seeker_idle" + std::to_string(_numObjects), "seeker_shoot" + std::to_string(_numObjects)); // unique name
																		 //std::shared_ptr<Animation> hpAnim = std::make_shared<HealthBarAnimation>();
					int assocNodes0[10] = { 0, 1 };
					curEnemy->delegateAnimation(anim, 0, assocNodes0, 2);


					std::shared_ptr<Animation> hpAnim = std::make_shared<HealthBarAnimation>();
					int assocNodes1[10] = { 2, 3 };
					curEnemy->delegateAnimation(hpAnim, 0, assocNodes1, 2);


					//curEnemy = CharacterModel::alloc(enemyPos, getPhysicsSize("newton_0", Vec2(NEWTON_SCALE, NEWTON_SCALE))); // TODO: decouple enemy logic from individual references in GameScene 
					//AddGameObject<CharacterModel>(curEnemy, "enemy_0", 3, Vec2(NEWTON_SCALE, NEWTON_SCALE));
					//AddGameObject<CharacterModel>(curEnemy, texKeys, zOrders, Vec2(NEWTON_SCALE, NEWTON_SCALE),
					//	hitboxIdxs, numNodes, isFilmstrip, hasBody);

					//curEnemy->addAnimation("enemy_", 1, ALLOWED_ORIENTATIONS, _assets);

					enemyPhys = std::make_shared<BasicPhysics>();
					//enemyPhys->init(SEEKER_SPEED);
					curEnemy->delegateMovement(std::dynamic_pointer_cast<Movement, BasicPhysics>(enemyPhys));


					std::shared_ptr<Movement> dummyMove = std::make_shared<Movement>();
					std::shared_ptr<Collision> dummyCol = std::make_shared<Collision>();
					std::shared_ptr<Explode> explode = std::make_shared<Explode>();
					explode->init(false, true); // enemies can't shoot each other

					//std::shared_ptr<ProjectileAttack> attack = std::make_shared<ProjectileAttack>(1, dummyMove, dummyCol, explode,//TODO: these functions are freed!
					//	Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, 3.0f, SEEKER_FIRE_RATE, physScale.x/2.0f);
					std::shared_ptr<ProjectileAttack> attack = std::make_shared<ProjectileAttack>(SEEKER_PROJ_SPEED, SEEKER_FIRE_RATE, physScale.x / 2.0f);
					attack->appendProjectile(SEEKER_PROJ_DMG, dummyMove, dummyCol, explode,
						Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, "enemy_proj_1", 0.0f, 0.0f);
					curEnemy->delegateAttack(std::dynamic_pointer_cast<Attack, ProjectileAttack>(attack));

					if (enemyType == Attack2Seeker) {
						std::shared_ptr<ProjectileAttack> attack2 = std::make_shared<ProjectileAttack>(SEEKER_PROJ_SPEED, SHIELD_BOSS_FIRE_RATE, physScale.x / 2.0f);
						attack2->appendProjectile(SHIELD_BOSS_PROJ_DMG, dummyMove, dummyCol, explode,
							Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, "enemy_proj_1", 0.0f, 0.0f);
						attack2->appendProjectile(SHIELD_BOSS_PROJ_DMG, dummyMove, dummyCol, explode,
							Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, "enemy_proj_1", 0.75f * physScale.x, 0);
						attack2->appendProjectile(SHIELD_BOSS_PROJ_DMG, dummyMove, dummyCol, explode,
							Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, "enemy_proj_1", -0.75f * physScale.x, 0);
						std::vector<int> switchHp = { 1 };
						std::vector<std::shared_ptr<Attack>> switchAtk = { attack2 };
						std::shared_ptr<SwitchWeapons> switchWeapDel = std::make_shared<SwitchWeapons>();
						switchWeapDel->init(switchHp, switchAtk);
						curEnemy->delegateUpdateLogic(switchWeapDel);
					}


					if (enemyType == Attack2Seeker) {
						std::shared_ptr<StandardDefense> stddef = std::make_shared<StandardDefense>();
						stddef->init(10000.0f); // can't be killed by speed
						curEnemy->delegateDefense(stddef);
					}

					// TODO should be delegate
					//curEnemy->setIsEnemy(true);

					if (enemyType == FixedShieldSeeker || enemyType == FixedSeeker) {
						std::shared_ptr<FencedAI> ai = std::make_shared<FencedAI>();
						ai->init(0.0f, 0.0f, 0.0f, enemyPos, 0.0f);
						curEnemy->delegateAI(ai);
						curEnemy->setBodyType(b2_staticBody);
					}
					else {
						std::shared_ptr<BasicAI> ai = std::make_shared<BasicAI>();
						ai->init(SEEKER_AGGRO_DISTANCE, SEEKER_UN_AGGRO_DISTANCE, SEEKER_SHOOT_DISTANCE);
						curEnemy->delegateAI(ai);
					}

					curEnemy->setTotalHealth(SEEKER_HEALTH);
					curEnemy->setCurrentHealth(SEEKER_HEALTH);

					//_numEnemies++;

					//if (i == 0) _enemy = curEnemy;
					//else if (i == 1) _enemy2 = curEnemy;
					//break;
				}
			}
			else if (enemyType == ShieldBoss) {


				Vec2 physSizeCenter = getPhysicsSize("shield_boss_idle", Vec2(0.1f, 0.04f));
				Vec2 physSizeOut = getPhysicsSize("shield", Vec2(NEWTON_SCALE * 1.7f, NEWTON_SCALE * 1.7f));

				//case ShieldBoss:
				newtonHitboxes.center = enemyPos;
				newtonHitboxes.offsets.push_back(Vec2(0.0f, 0.0f));
				newtonHitboxes.shapeType.push_back(ELLIPSE_HITBOX);
				newtonHitboxes.sizes.push_back(physSizeCenter);
				newtonHitboxes.physCol.push_back(nullptr);
				newtonHitboxes.logicCol.push_back(nullptr);
				newtonHitboxes.defCol.push_back(nullptr);

				
				newtonHitboxes.offsets.push_back(Vec2(0.0f, 0.0f));
				newtonHitboxes.shapeType.push_back(ELLIPSE_HITBOX);
				newtonHitboxes.sizes.push_back(physSizeOut);
				newtonHitboxes.physCol.push_back(nullptr); newtonHitboxes.logicCol.push_back(nullptr);
				newtonHitboxes.defCol.push_back(std::dynamic_pointer_cast<DefendCollision, ShieldDefense>(shieldDef));
				

				curEnemy = CharacterModel::alloc(&newtonHitboxes);
				//curEnemy = CharacterModel::alloc(enemyPos, getPhysicsSize("newton_0", Vec2(NEWTON_SCALE, NEWTON_SCALE))); // TODO: decouple enemy logic from individual references in GameScene 


				std::vector<nodesInit_t> nodes;
				nodesInit_t node0;node0.hitboxId = 0;node0.numNodes = 4;
				node0.textures[0] = "shield_boss_idle";node0.filmStripRow[0] = 8;node0.filmStripCol[0] = 8;node0.zOrders[0] = CHARACTER_Z_ORDER;node0.physOffsets[0] = Vec2(0.0f, 0.0f); node0.relativeScale[0] = SAME_SCALE *4;
				node0.textures[1] = "shield_boss_shoot"; node0.filmStripRow[1] = 8; node0.filmStripCol[1] = 8; node0.zOrders[1] = CHARACTER_Z_ORDER; node0.physOffsets[1] = Vec2(0.0f, 0.0f); node0.relativeScale[1] = SAME_SCALE * 4;

				node0.textures[2] = "health_bar_top";
				node0.filmStripRow[2] = 1;
				node0.filmStripCol[2] = 1;
				node0.zOrders[2] = HP_TOP_Z_ORDER;
				node0.physOffsets[2] = Vec2(0.0f, physSizeOut.y / 2.0f + SHIELD_BOSS_HITBAR_Y_OFFSET);
				node0.relativeScale[2] = Vec2(4.0f, 4.0f);

				node0.textures[3] = "health_bar_bot";
				node0.filmStripRow[3] = 1;
				node0.filmStripCol[3] = 1;
				node0.zOrders[3] = HP_BOT_Z_ORDER;
				node0.physOffsets[3] = Vec2(0.0f, physSizeOut.y / 2.0f + SHIELD_BOSS_HITBAR_Y_OFFSET);
				node0.relativeScale[3] = Vec2(4.0f, 4.0f);


				nodesInit_t node1;node1.hitboxId = 1;node1.numNodes = 1;node1.textures[0] = "shield";node1.filmStripRow[0] = 1;node1.filmStripCol[0] = 1;node1.zOrders[0] = SHIELD_Z_ORDER;node1.physOffsets[0] = Vec2(0.0f, 0.0f); node1.relativeScale[0] = SAME_SCALE * 2;
				//nodesInit_t node2;node2.hitboxId = 2;node2.numNodes = 1;node2.textures[0] = "enemy_0";node2.filmStripRow[0] = 1;node2.filmStripCol[0] = 1;node2.zOrders[0] = SHIELD_Z_ORDER;node2.physOffsets[0] = Vec2(0.0f, 0.0f); node2.relativeScale[0] = SAME_SCALE;
				//nodesInit_t node3;node3.hitboxId = 3;node3.numNodes = 1;node3.textures[0] = "enemy_0";node3.filmStripRow[0] = 1;node3.filmStripCol[0] = 1;node3.zOrders[0] = SHIELD_Z_ORDER;node3.physOffsets[0] = Vec2(0.0f, 0.0f); node3.relativeScale[0] = SAME_SCALE;
				//nodesInit_t node4;node4.hitboxId = 4;node4.numNodes = 1;node4.textures[0] = "enemy_0";node4.filmStripRow[0] = 1;node4.filmStripCol[0] = 1;node4.zOrders[0] = SHIELD_Z_ORDER;node4.physOffsets[0] = Vec2(0.0f, 0.0f); node4.relativeScale[0] = SAME_SCALE;

				
				Vec2 scale = Vec2(NEWTON_SCALE, NEWTON_SCALE);

				nodes.push_back(node0);
				nodes.push_back(node1);
				//nodes.push_back(node2);
				//nodes.push_back(node3);
				//nodes.push_back(node4);
				AddGameObject<CharacterModel>(curEnemy, scale, nodes);

				std::shared_ptr<AttackMoveAnimation> anim = std::make_shared<AttackMoveAnimation>();
				anim->init("shield_boss_idle", "shield_boss_shoot"); // unique name
				//std::shared_ptr<Animation> hpAnim = std::make_shared<HealthBarAnimation>();
				int assocNodes[10] = { 0, 1 };
				curEnemy->delegateAnimation(anim, 0, assocNodes, 2);

				std::shared_ptr<Animation> hpAnim = std::make_shared<HealthBarAnimation>();
				assocNodes[0] = 2; assocNodes[1] = 3;
				curEnemy->delegateAnimation(hpAnim, 0, assocNodes, 2);


				curEnemy->setOrientations(ALLOWED_ORIENTATIONS / 2);
				curEnemy->setTurnOnMove(false);
				//assocNodes[0] = 3; assocNodes[1] = 4;
				//_player->delegateAnimation(hpAnim, 0, assocNodes, 2);

				//AddGameObject<CharacterModel>(curEnemy, texKeys, zOrders, Vec2(NEWTON_SCALE, NEWTON_SCALE),
				//	hitboxIdxs, numNodes, isFilmstrip, hasBody);

				//AddGameObject<CharacterModel>(curEnemy, "enemy_0", 3, Vec2(NEWTON_SCALE, NEWTON_SCALE));
				//curEnemy->addAnimation("enemy_", 1, ALLOWED_ORIENTATIONS, _assets);

				enemyPhys = std::make_shared<BasicPhysics>();
				curEnemy->delegateMovement(std::dynamic_pointer_cast<Movement, BasicPhysics>(enemyPhys));

				std::shared_ptr<Movement> dummyMove = std::make_shared<Movement>();
				std::shared_ptr<Collision> dummyCol = std::make_shared<Collision>();
				std::shared_ptr<Explode> explode = std::make_shared<Explode>();

				//std::shared_ptr<ProjectileAttack> attack = std::make_shared<ProjectileAttack>(1, dummyMove, dummyCol, explode,//TODO: these functions are freed!
				//	Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, 3.0f, SEEKER_FIRE_RATE, physSizeCenter.x/2.0f);
				std::shared_ptr<ProjectileAttack> attack = std::make_shared<ProjectileAttack>(SHIELD_BOSS_PROJ_SPEED, SHIELD_BOSS_FIRE_RATE, physSizeCenter.x / 2.0f);
				attack->appendProjectile(SHIELD_BOSS_PROJ_DMG, dummyMove, dummyCol, explode,
					Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, "enemy_proj_1", 0.0f, 0.0f);
				attack->appendProjectile(SHIELD_BOSS_PROJ_DMG, dummyMove, dummyCol, explode,
					Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, "enemy_proj_1", 0.3f * physSizeCenter.x, - PI / 4);
				attack->appendProjectile(SHIELD_BOSS_PROJ_DMG, dummyMove, dummyCol, explode,
					Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, "enemy_proj_1", -0.3 * physSizeCenter.x,  PI / 4);
				curEnemy->delegateAttack(std::dynamic_pointer_cast<Attack, ProjectileAttack>(attack));



				std::shared_ptr<StandardDefense> stddef = std::make_shared<StandardDefense>();
				stddef->init(10000.0f); // can't be killed by speed
				curEnemy->delegateDefense(stddef);


				//std::shared_ptr<BasicAI> ai = std::make_shared<BasicAI>();
				//ai->init(SHIELD_BOSS_AGGRO_DISTANCE, SHIELD_BOSS_UN_AGGRO_DISTANCE, SHIELD_BOSS_SHOOT_DISTANCE);
				std::shared_ptr<FencedAI> ai = std::make_shared<FencedAI>();
				ai->init(SHIELD_BOSS_AGGRO_DISTANCE, SHIELD_BOSS_UN_AGGRO_DISTANCE, SHIELD_BOSS_SHOOT_DISTANCE, enemyPos, SHIELD_BOSS_FENCE_RADIUS);
				curEnemy->delegateAI(ai);
				curEnemy->setBodyType(b2_staticBody);

				std::shared_ptr<ShieldRegen> shieldRegen = std::make_shared<ShieldRegen>();
				std::vector<int> regenHealths = { 3 };
				shieldRegen->init(_world.get(), _debugnode, this, regenHealths, scale, "shield_boss_shield",
					nodes[1], cugl::Vec2::ZERO, ELLIPSE_HITBOX, physSizeOut, nullptr, nullptr, shieldDef);
				curEnemy->delegateUpdateLogic(shieldRegen);

				curEnemy->setTotalHealth(SHIELD_BOSS_HEALTH);
				curEnemy->setCurrentHealth(SHIELD_BOSS_HEALTH);

				auto elimGoal = std::make_shared<EliminateGoal>();
				elimGoal->init(curEnemy);
				_goal = std::dynamic_pointer_cast<GoalCollision, EliminateGoal>(elimGoal);

				// TODO should be delegate
				//curEnemy->setIsEnemy(true);

				//if (i == 0) _enemy = curEnemy;
				//else if (i == 1) _enemy2 = curEnemy;
				//break;
			}
			else if (enemyType == ChaseBoss) {
				//Vec2 physScale = getPhysicsSize("newton_0", Vec2(NEWTON_SCALE, NEWTON_SCALE));
				Vec2 physScale = getPhysicsSize("chase_boss_p1_shoot", Vec2(0.05f, 0.05f));


				Vec2 widenBox = Vec2(2.5f, 0.1f);
				Vec2 killBoxSize = physScale * widenBox;

				newtonHitboxes.center = enemyPos;
				newtonHitboxes.shapeType.push_back(ELLIPSE_HITBOX);
				newtonHitboxes.offsets.push_back(Vec2(0.0f, 0.0f));
				newtonHitboxes.sizes.push_back(physScale);
				newtonHitboxes.physCol.push_back(nullptr);
				newtonHitboxes.logicCol.push_back(nullptr);
				newtonHitboxes.defCol.push_back(nullptr);


				std::shared_ptr<PassCollision> ignoreBullets = std::make_shared<PassCollision>();
				ignoreBullets->init(true, false);

				newtonHitboxes.shapeType.push_back(STANDARD_HITBOX);
				newtonHitboxes.offsets.push_back(Vec2(0.0f, 0.0f));
				newtonHitboxes.sizes.push_back(killBoxSize);
				newtonHitboxes.physCol.push_back(ignoreBullets);
				newtonHitboxes.logicCol.push_back(nullptr);
				newtonHitboxes.defCol.push_back(nullptr);

				curEnemy = CharacterModel::alloc(&newtonHitboxes);
				//numNodes = 1;
				Vec2 scale = Vec2(SEEKER_SCALE, SEEKER_SCALE);

				std::vector<nodesInit_t> nodes;
				nodesInit_t node;
				node.hitboxId = 0;
				node.numNodes = 7;
				node.textures[0] = "chase_boss_p1_move";
				node.filmStripRow[0] = 8;
				node.filmStripCol[0] = 8;
				node.zOrders[0] = CHARACTER_Z_ORDER;
				node.physOffsets[0] = Vec2(0.0f, 0.0f);
				node.relativeScale[0] = SAME_SCALE;


				node.textures[1] = "chase_boss_p1_shoot";
				node.filmStripRow[1] = 8;
				node.filmStripCol[1] = 11;
				node.zOrders[1] = CHARACTER_Z_ORDER;
				node.physOffsets[1] = Vec2(0.0f, 0.0f);
				node.relativeScale[1] = SAME_SCALE;

				node.textures[4] = "chase_boss_transform";
				node.filmStripRow[4] = 2;
				node.filmStripCol[4] = 15;
				node.zOrders[4] = CHARACTER_Z_ORDER;
				node.physOffsets[4] = Vec2(0.0f, 0.0f);
				node.relativeScale[4] = SAME_SCALE;

				node.textures[5] = "chase_boss_p2_move";
				node.filmStripRow[5] = 8;
				node.filmStripCol[5] = 8;
				node.zOrders[5] = CHARACTER_Z_ORDER;
				node.physOffsets[5] = Vec2(0.0f, 0.0f);
				node.relativeScale[5] = SAME_SCALE;

				node.textures[6] = "chase_boss_p2_shoot";
				node.filmStripRow[6] = 8;
				node.filmStripCol[6] = 9;
				node.zOrders[6] = CHARACTER_Z_ORDER;
				node.physOffsets[6] = Vec2(0.0f, 0.0f);
				node.relativeScale[6] = SAME_SCALE;

				node.textures[2] = "health_bar_top";
				node.filmStripRow[2] = 1;
				node.filmStripCol[2] = 1;
				node.zOrders[2] = HP_TOP_Z_ORDER;
				node.physOffsets[2] = Vec2(0.0f, physScale.y / 2.0f + HITBOX_Y_OFFSET);
				node.relativeScale[2] = SAME_SCALE;

				node.textures[3] = "health_bar_bot";
				node.filmStripRow[3] = 1;
				node.filmStripCol[3] = 1;
				node.zOrders[3] = HP_BOT_Z_ORDER;
				node.physOffsets[3] = Vec2(0.0f, physScale.y / 2.0f + HITBOX_Y_OFFSET);
				node.relativeScale[3] = SAME_SCALE;


				nodes.push_back(node);
				AddGameObject<CharacterModel>(curEnemy, scale, nodes);

				// add animations
				curEnemy->setOrientations(ALLOWED_ORIENTATIONS / 2);
				curEnemy->setOrientationMode(ROUND_UP);
				curEnemy->setTurnOnAttack(false);

				std::shared_ptr<ChaseBossAnim> anim = std::make_shared<ChaseBossAnim>();
				anim->init("chase_boss_idle", "chase_boss_shoot", "chase_boss_transform"); // unique name
																													   //std::shared_ptr<Animation> hpAnim = std::make_shared<HealthBarAnimation>();
				int assocNodes0[10] = { 0, 1, 4, 5, 6 };
				curEnemy->delegateAnimation(anim, 0, assocNodes0, 5);


				std::shared_ptr<Animation> hpAnim = std::make_shared<HealthBarAnimation>();
				int assocNodes1[10] = { 2, 3 };
				curEnemy->delegateAnimation(hpAnim, 0, assocNodes1, 2);


				// movement delegate
				std::shared_ptr<VelocityPhysics> velPhys = std::make_shared<VelocityPhysics>();
				velPhys->init(CHASE_BOSS_SPEED);
				curEnemy->delegateMovement(velPhys);

				// physical collision delegate. do not collide with anything 
				std::shared_ptr<PassCollision> ghost = std::make_shared<PassCollision>();
				ghost->init(false, false);
				curEnemy->delegatePhysCollision(ghost);

				// logic collision delegate kill player
				std::shared_ptr<DOTCollision> instaKill = std::make_shared<DOTCollision>();
				curEnemy->delegateLogicCollision(instaKill);

				std::shared_ptr<StandardDefense> def = std::make_shared<StandardDefense>();
				def->init(10000.0f); // can't die to collisions
				curEnemy->delegateDefense(def);


				// setup attack
				std::shared_ptr<Movement> dummyMove = std::make_shared<Movement>();
				std::shared_ptr<Collision> dummyCol = std::make_shared<Collision>();
				std::shared_ptr<Explode> explode = std::make_shared<Explode>();
				explode->init(false, true);


				std::shared_ptr<ProjectileAttack> attack = std::make_shared<ProjectileAttack>(CHASE_BOSS_PROJ_SPEED, CHASE_BOSS_FIRE_RATE, physScale.x / 2.0f);
				attack->appendProjectile(CHASE_BOSS_PROJ_DMG, dummyMove, dummyCol, explode,
					Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, "enemy_proj_1", 0.0f, 0.0f);
				curEnemy->delegateAttack(std::dynamic_pointer_cast<Attack, ProjectileAttack>(attack));


				// add a second attack phase
				std::shared_ptr<ProjectileAttack> attack2 = std::make_shared<ProjectileAttack>(CHASE_BOSS_PROJ_SPEED, CHASE_BOSS_FIRE_RATE, physScale.x / 2.0f);
				attack2->appendProjectile(CHASE_BOSS_PROJ_DMG, dummyMove, dummyCol, explode,
					Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, "enemy_proj_1", 0.0f, 0.0f);
				attack2->appendProjectile(CHASE_BOSS_PROJ_DMG, dummyMove, dummyCol, explode,
					Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, "enemy_proj_1", 0.85f * physScale.x, 0);
				attack2->appendProjectile(CHASE_BOSS_PROJ_DMG, dummyMove, dummyCol, explode,
					Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE), 1, 0, "enemy_proj_1", -0.85f * physScale.x, 0);
				std::vector<int> switchHp = { CHASE_BOSS_HEALTH / 2 };
				std::vector<std::shared_ptr<Attack>> switchAtk = { attack2 };
				std::shared_ptr<SwitchWeapons> switchWeapDel = std::make_shared<SwitchWeapons>();
				switchWeapDel->init(switchHp, switchAtk);
				curEnemy->delegateUpdateLogic(switchWeapDel);

				// setup ai delegate
					//std::vector<cugl::Vec2> &pathPts, float shootDistance, float catchUpDistThresh, float chasePlayerMod)
				std::shared_ptr<PathFollowAI> ai = std::make_shared<PathFollowAI>();
				//ai->init(SEEKER_AGGRO_DISTANCE, SEEKER_UN_AGGRO_DISTANCE, SEEKER_SHOOT_DISTANCE);
				auto floatVertArray = enemy->get("pos")->asFloatArray();
				std::vector<Vec2> patrolPoints;
				for (int i = 0; i < floatVertArray.size(); i = i + 2) {
					patrolPoints.push_back(Vec2(floatVertArray[i] * pathToBox2d, floatVertArray[i + 1] * pathToBox2d));
				}
				ai->init(patrolPoints, CHASE_BOSS_SHOOT_DISTANCE, CHASE_BOSS_CATCH_UP_DIST, CHASE_BOSS_CATCH_UP_SPEED_MOD);
				curEnemy->delegateAI(ai);

				// set starting health
				curEnemy->setTotalHealth(CHASE_BOSS_HEALTH);
				curEnemy->setCurrentHealth(CHASE_BOSS_HEALTH);

				auto elimGoal = std::make_shared<EliminateGoal>();
				elimGoal->init(curEnemy);
				_goal = std::dynamic_pointer_cast<GoalCollision, EliminateGoal>(elimGoal);

			}
			/*case Shooter:
				break;
			}*/
		}
	}
	return success;
}

enum boundary_t {
	Solid
};

boundary_t evalBoundary(std::string boundaryString) {
	if (boundaryString == "solid") return Solid;
    return Solid;
}

bool LevelStartup::loadBoundaries(const std::shared_ptr<JsonValue> json) {
	bool success = false;
	auto boundaryList = json->get("boundary");

	if (success = (boundaryList != nullptr)) {
		success = success && boundaryList->isArray();
		for (int i = 0; i < boundaryList->size(); i++) {
			std::shared_ptr<JsonValue> boundary = boundaryList->get(i);

			boundary_t boundaryType = evalBoundary(boundary->get("type")->asString());
			std::vector<float> verts = boundary->get("verts")->asFloatArray();
			for (int j = 0; j < verts.size(); j++) {
				verts[j] *= pathToBox2d;
			}

			switch (boundaryType) {
			case Solid:
				std::shared_ptr<Texture> image;

				// All walls share the same texture
				image = _assets->get<Texture>("blank");
				std::string wname = "wall";

				Poly2 wall1(&verts[0], verts.size());
				SimpleTriangulator triangulator;
				triangulator.set(wall1);
				triangulator.calculate();
				wall1.setIndices(triangulator.getTriangulation());
				wall1.setType(Poly2::Type::SOLID);

				/*std::shared_ptr<PolygonObstacle> wallobj = PolygonObstacle::alloc(wall1);
				wallobj->setDebugColor(STATIC_COLOR);
				wallobj->setName(wname);

				// Set the physics attributes
				wallobj->setBodyType(b2_staticBody);
				wallobj->setDensity(BASIC_DENSITY);
				wallobj->setFriction(BASIC_FRICTION);
				wallobj->setRestitution(BASIC_RESTITUTION);

				// Add the scene graph nodes to this object
				wall1 *= _scale;
				std::shared_ptr<PolygonNode> sprite = PolygonNode::allocWithTexture(image, wall1);
				addObstacle(wallobj, sprite, 1);  // All walls share the same texture


				// remember this so we can move later on camera pan
				std::shared_ptr<Polyzone> polywall1 = Polyzone::alloc(Vec2::ZERO, Size::ZERO);
				polywall1->setSceneGraphNode(sprite); // for scene graph update
				polywall1->setPolyObstacle(wallobj); // for physics
				int idx1, version1;
				std::shared_ptr<PhysicalObject> p1 = std::dynamic_pointer_cast<PhysicalObject, Polyzone>(polywall1);
				_allObjects->put(p1, &idx1, &version1);
				wallobj->setIdx(idx1);
				wallobj->setVersion(version1);*/

				std::shared_ptr<Polyzone> polywall1 = std::make_shared<Polyzone>();
				polywall1->initWithPoly(wall1, 1.0f, true);
				//AddGameObject<Polyzone>(polywall1, "blank", 1, Vec2(1.0f, 1.0f));

				/*const char *texKeys[1] = { "blank" };
				int zOrders[1] = { 1 };
				int hitboxIdxs[1] = { 0 };
				int filmstripDim[2] = { 1, 1 };
				bool hasBody[1] = { true };
				int numNodes = 1;
				Vec2 scale = Vec2(1.0f, 1.0f);*/

				std::vector<nodesInit_t> nodes;
				nodesInit_t node;
				node.hitboxId = 0;
				node.numNodes = 1;
				node.textures[0] = "blank";
				node.filmStripRow[0] = 1;
				node.filmStripCol[0] = 1;
				node.zOrders[0] = CHARACTER_Z_ORDER;
				node.physOffsets[0] = Vec2(0.0f, 0.0f);
				node.relativeScale[0] = SAME_SCALE;
				Vec2 scale = Vec2(1.0f, 1.0f);

				nodes.push_back(node);
				AddGameObject<Polyzone>(polywall1, scale, nodes);

				//AddGameObject<Polyzone>(polywall1, texKeys, zOrders, scale, hitboxIdxs, numNodes, filmstripDim, hasBody);

				break;
			}
		}
	}
	return success;
}

enum hazard_t {
	Lava,
};

hazard_t evalHazard(std::string hazardString) {
	if (hazardString == "lava") return Lava;
    return Lava;
}

bool LevelStartup::loadHazards(const std::shared_ptr<JsonValue> json) {
	bool success = false;
	auto boundaryList = json->get("hazard");

	if (success = (boundaryList != nullptr)) {
		success = success && boundaryList->isArray();
		for (int i = 0; i < boundaryList->size(); i++) {
			std::shared_ptr<JsonValue> boundary = boundaryList->get(i);

			hazard_t hazardType = evalHazard(boundary->get("type")->asString());
			std::vector<float> verts = boundary->get("verts")->asFloatArray();
			for (int j = 0; j < verts.size(); j++) {
				verts[j] *= pathToBox2d;
			}

			std::string wname;
			Poly2 wall1;
			std::shared_ptr<Polyzone> polywall1;
			std::shared_ptr<PassCollision> physCol; 
			std::shared_ptr<DOTCollision> logicCol;
			SimpleTriangulator triangulator;

			_numObjects++;

			switch (hazardType) {
			case Lava:
				//std::shared_ptr<Texture> image;

				// All walls share the same texture
				//image = _assets->get<Texture>(GOAL_TEXTURE);
				wname = "lava";

				wall1 = Poly2(&verts[0], verts.size());
				
				triangulator.set(wall1);
				triangulator.calculate();
				wall1.setIndices(triangulator.getTriangulation());
				wall1.setType(Poly2::Type::SOLID);
				

				polywall1 = std::make_shared<Polyzone>();
				polywall1->initWithPoly(wall1, 1.0f, true);

				//physCol = std::make_shared<PassCollision>();
				//polywall1->delegatePhysCollision(physCol);

				/*logicCol = std::make_shared<DOTCollision>();
				//int msBetweenDmg = 6000;
				logicCol->init(6000);
				polywall1->delegateLogicCollision(logicCol);*/
				/*std::shared_ptr<Shock> shock = std::make_shared<Shock>();
				shock->init(false, true);
				polywall1->setDamage(BOUNDARY_DAMAGE);
				polywall1->delegateLogicCollision(shock);*/

				/*const char *texKeys[1] = { EARTH_TEXTURE };
				int zOrders[1] = { 1 };
				int hitboxIdxs[1] = { 0 };
				int filmstripDim[2] = { 1, 1 };
				bool hasBody[1] = { true };
				int numNodes = 1;
				Vec2 scale = Vec2(1.0f, 1.0f);*/

				std::vector<nodesInit_t> nodes;
				nodesInit_t node;
				node.hitboxId = 0;
				node.numNodes = 1;
				node.textures[0] = "blank";
				node.filmStripRow[0] = 1;
				node.filmStripCol[0] = 1;
				node.zOrders[0] = LAVA_Z_ORDER;
				node.physOffsets[0] = Vec2(0.0f, 0.0f);
				node.relativeScale[0] = SAME_SCALE;
				Vec2 scale = Vec2(1.0f, 1.0f);

				nodes.push_back(node);
				AddGameObject<Polyzone>(polywall1, scale, nodes);



				//AddGameObject<Polyzone>(polywall1, texKeys, zOrders, scale, hitboxIdxs, numNodes, filmstripDim, hasBody);


				//AddGameObject<Polyzone>(polywall1, EARTH_TEXTURE, 1, Vec2(1.0f, 1.0f));
				break;
			}

			
		}
	}
	else {
		success = true;
	}
	return success;
}


enum circleZone_t {
	SpeedUp,
	SpeedDown,
	BounceWallLeft,
	BounceWallRight,
	ShockWallLeft,
	ShockWallRight
};

circleZone_t evalZone(std::string zoneString) {
	if (zoneString == "speedup") return SpeedUp;
	if (zoneString == "speeddown") return SpeedDown;
	if (zoneString == "bwleft") return BounceWallLeft;
	if (zoneString == "bwright") return BounceWallRight;
	if (zoneString == "swleft") return ShockWallLeft;
	if (zoneString == "swright") return ShockWallRight;
	return SpeedUp;
}

bool LevelStartup::loadCircleZones(const std::shared_ptr<JsonValue> json) {
	bool success = false;
	auto boundaryList = json->get("circlezones");

	if (success = (boundaryList != nullptr)) {
		success = success && boundaryList->isArray();
		for (int i = 0; i < boundaryList->size(); i++) {
			std::shared_ptr<JsonValue> boundary = boundaryList->get(i);

			_numObjects++;

			circleZone_t zoneType = evalZone(boundary->get("type")->asString());
			cugl::Vec2 pos = Vec2(boundary->get("pos")->get(0)->asFloat()*pathToBox2d, boundary->get("pos")->get(1)->asFloat()*pathToBox2d);
			
			/*const char *texKeys[1][2] = { { "speed_up_zone", "speed_up_ball" } };
			int zOrders[2] = { CHARACTER_Z_ORDER - 1, CHARACTER_Z_ORDER + 1 };
			int hitboxIdxs[2] = { 0, 1 };
			int isFilmstrip[4] = { 1, 1,
								   1, 1};
			//int isFilmstrip[2] = { 1, 1 };
			bool hasBody[2] = { true, false };
			int numNodes = 2;*/


			std::string wname;
			Poly2 wall1;
			std::shared_ptr<Polyzone> polywall1;
			std::shared_ptr<PassCollision> physCol;
			std::shared_ptr<DOTCollision> logicCol;
			std::shared_ptr<ChangeSpeedCollision> speedLogicCol;


			//switch (zoneType) {
			
			//case SpeedUp:
			if (zoneType == SpeedUp) {
				wname = "speedUpZone";


				wall1 = Poly2::createEllipse(pos, getPhysicsSize("speed_up_zone", Vec2(1.0f, 1.0f)), 10, true);


				polywall1 = std::make_shared<Polyzone>();
				polywall1->initWithPoly(wall1, 1.0f, false);

				physCol = std::make_shared<PassCollision>();
				polywall1->delegatePhysCollision(physCol);

				speedLogicCol = std::make_shared<ChangeSpeedCollision>();
				//int msBetweenDmg = 6000;
				speedLogicCol->init(SPEED_UP_MULT);
				polywall1->delegateLogicCollision(speedLogicCol);

				Vec2 scale = Vec2(1.0f, 1.0f);

				std::vector<nodesInit_t> nodes;
				nodesInit_t node;
				node.hitboxId = 0;
				node.numNodes = 2;
				node.textures[0] = "speed_up_zone"; node.textures[1] = "discoball";
				node.filmStripRow[0] = 1; node.filmStripRow[1] = 2;
				node.filmStripCol[0] = 1; node.filmStripCol[1] = 14;
				node.zOrders[0] = ZONE_Z_ORDER; node.zOrders[1] = ELEVATED_Z_ORDER;
				node.relativeScale[0] = SAME_SCALE; node.relativeScale[1] = SAME_SCALE;


				node.physOffsets[0] = Vec2(0.0f, 0.0f); node.physOffsets[1] = Vec2(0.0f, getPhysicsSize("discoball", scale).height/4.0f);
				

				nodes.push_back(node);
				AddGameObject<Polyzone>(polywall1, scale, nodes);

				std::shared_ptr<RepeatAnimation> spin = std::make_shared<RepeatAnimation>();
				spin->init("discoball_anim" + std::to_string(_numObjects));
				int assocNodes[10] = { 1 };
				polywall1->delegateAnimation(spin, 0, assocNodes, 1);

				//AddGameObject<Polyzone>(polywall1, "speed_up_zone", 1, Vec2(1.0f, 1.0f));
				//AddGameObject<Polyzone>(polywall1, texKeys, zOrders, Vec2(1.0f, 1.0f), hitboxIdxs,
				//	numNodes, isFilmstrip, hasBody);

				//break;
			}
			else if (zoneType == SpeedDown){
			//case SpeedDown:

				wname = "speedDownZone";

			

			}
			else if (zoneType == BounceWallLeft || zoneType == BounceWallRight) {
				wname = "bw_left";

				Vec2 scale = Vec2(0.6f, 0.6f);
				Vec2 physScale = getPhysicsSize("bounce_wall", scale / 4.0f);

				//Vec2 scale = Vec2(0.3f, 0.3f);
				//Vec2 physScale = getPhysicsSize("bounce_wall_left", scale);

				hitboxes_t newtonHitboxes;
				newtonHitboxes.center = pos;
				newtonHitboxes.shapeType.push_back(ELLIPSE_HITBOX);
				newtonHitboxes.offsets.push_back(Vec2(0.0f, 0.0f));
				newtonHitboxes.sizes.push_back(Vec2(physScale.x * 0.225f, physScale.y * 0.20f));
				newtonHitboxes.physCol.push_back(nullptr);
				newtonHitboxes.logicCol.push_back(nullptr);
				newtonHitboxes.defCol.push_back(nullptr);


				std::shared_ptr<CharacterModel> tentacles = CharacterModel::alloc(&newtonHitboxes);

				if (zoneType == BounceWallLeft)
					tentacles->setAngle(-PI / 10.0f);
				else
					tentacles->setAngle(PI / 10.0f);
				//std::shared_ptr<BasicPhysics> dummyPhys = std::make_shared<BasicPhysics>();
				//polywall1->delegatePhysCollision(dummyPhys);


				

				std::vector<nodesInit_t> nodes;
				nodesInit_t node;
				node.hitboxId = 0;
				node.numNodes = 1;
				node.textures[0] = "bounce_wall";
				node.filmStripRow[0] = 4;
				node.filmStripCol[0] = 9;
				node.zOrders[0] = OBSTACLE_Z_ORDER;
				node.physOffsets[0] = Vec2(0.0f, physScale.y * 0.275f);
				node.relativeScale[0] = SAME_SCALE;


				nodes.push_back(node);
				AddGameObject<CharacterModel>(tentacles, scale, nodes);

				// don't want this obstacle to move (set center body to static)
				tentacles->setBodyType(b2_staticBody);
				tentacles->setDestructable(false);

				std::shared_ptr<OnHitAnimation> onhit = std::make_shared<OnHitAnimation>();
				if (zoneType == BounceWallLeft)
					onhit->init("bouncewall_anim" + std::to_string(_numObjects), 0, 0.7f);
				else
					onhit->init("bouncewall_anim" + std::to_string(_numObjects), 3, 0.7f);
				int assocNodes[10] = { 0 };
				tentacles->delegateAnimation(onhit, 0, assocNodes, 1);
			}
			else if (zoneType == ShockWallLeft || zoneType == ShockWallRight) { // TODO change back and add to level editor
				wname = "sw_left";

				Vec2 scale = Vec2(0.5f, 0.5f);
				Vec2 physScale = getPhysicsSize("shock_wall", scale / 4.0f);

				hitboxes_t newtonHitboxes;
				newtonHitboxes.center = pos;
				newtonHitboxes.shapeType.push_back(ELLIPSE_HITBOX);
				newtonHitboxes.offsets.push_back(Vec2(0.0f, 0.0f));
				newtonHitboxes.sizes.push_back(Vec2(physScale.x * 0.5f , physScale.y * 0.225f));
				newtonHitboxes.physCol.push_back(nullptr);
				newtonHitboxes.logicCol.push_back(nullptr);
				newtonHitboxes.defCol.push_back(nullptr);


				std::shared_ptr<CharacterModel> tentacles = CharacterModel::alloc(&newtonHitboxes);
				if (zoneType == ShockWallLeft)
					tentacles->setAngle(-PI / 12.0f);
				else 
					tentacles->setAngle(PI / 12.0f);
				// this wall deals damage upon collision
				std::shared_ptr<Shock> shock = std::make_shared<Shock>();
				shock->init(false, true);
				tentacles->setDamage(SHOCK_WALL_DAMAGE);
				tentacles->delegateLogicCollision(shock);


				std::vector<nodesInit_t> nodes;
				nodesInit_t node;
				node.hitboxId = 0;
				node.numNodes = 1;
				node.textures[0] = "shock_wall";
				node.filmStripRow[0] = 4;
				node.filmStripCol[0] = 5;
				node.zOrders[0] = OBSTACLE_Z_ORDER;
				node.physOffsets[0] = Vec2(0.0f, physScale.y * 0.35f);
				node.relativeScale[0] = SAME_SCALE;


				nodes.push_back(node);
				AddGameObject<CharacterModel>(tentacles, scale, nodes);

				// don't want this obstacle to move (set center body to static)
				tentacles->setBodyType(b2_staticBody);
				tentacles->setDestructable(false);

				std::shared_ptr<OnHitAnimation> onhit = std::make_shared<OnHitAnimation>();
				if (zoneType == ShockWallLeft)
					onhit->init("shockwall_anim" + std::to_string(_numObjects), 0, 0.55f);
				else
					onhit->init("shockwall_anim" + std::to_string(_numObjects), 3, 0.55f);
				int assocNodes[10] = { 0 };
				tentacles->delegateAnimation(onhit, 0, assocNodes, 1);

			}
			else {
				success = true;
			}


		}
	}
	else {
		success = true;
	}
	return success;
}

bool LevelStartup::loadBackground(const std::shared_ptr<JsonValue> json) {
	Size bgSize = bgSprite->getTexture()->getSize() / _scale;

	float bgpos[] = { box2dWidth / 2, box2dHeight / 2 };//+ 0.75f };
	Vec2 bgPos = ((Vec2)bgpos);

	std::shared_ptr<BoxObstacle> bg = BoxObstacle::alloc(bgPos, bgSize);
	bg->setName("bg");
	bg->setDebugColor(STATIC_COLOR);

	bg->setBodyType(b2_staticBody);
	bg->setDensity(0.0f);
	bg->setFriction(0.0f);
	bg->setRestitution(0.0f);
	bg->setSensor(true);

	addObstacle(bg, bgSprite, MAP_Z_ORDER);

	return true;
}

bool LevelStartup::loadGoal(const std::shared_ptr<JsonValue> json) {
	bool success;
	auto goalPos = json->get("goal");

	if (success = (goalPos != nullptr && _goal == nullptr)) {
		success = success && goalPos->isArray();
		Vec2 goalPosPhys = Vec2(goalPos->get(0)->asFloat()*pathToBox2d, goalPos->get(1)->asFloat()*pathToBox2d);

		//std::shared_ptr<Texture> goalImage = _assets->get<Texture>(GOAL_TEXTURE);
		//Size goalSize = goalImage->getSize() / _scale;


		std::string wname = "sw_left";

		Vec2 scale = Vec2(0.5f, 0.5f);
		Vec2 physScale = getPhysicsSize(GOAL_TEXTURE, scale);

		hitboxes_t newtonHitboxes;
		newtonHitboxes.center = goalPosPhys;
		newtonHitboxes.shapeType.push_back(ELLIPSE_HITBOX);
		newtonHitboxes.offsets.push_back(Vec2(0.0f, 0.0f));
		newtonHitboxes.sizes.push_back(physScale * 0.75f);
		newtonHitboxes.physCol.push_back(nullptr);
		newtonHitboxes.logicCol.push_back(nullptr);
		newtonHitboxes.defCol.push_back(nullptr);


		std::shared_ptr<CharacterModel> goal = CharacterModel::alloc(&newtonHitboxes);

		
		std::vector<nodesInit_t> nodes;
		nodesInit_t node;
		node.hitboxId = 0;
		node.numNodes = 2;
		node.textures[0] = GOAL_TEXTURE;
		node.filmStripRow[0] = 1;
		node.filmStripCol[0] = 1;
		node.zOrders[0] = GOAL_Z_ORDER;
		node.physOffsets[0] = Vec2(0.0f, 0.0f);
		node.relativeScale[0] = SAME_SCALE;

		node.textures[1] = "lock_goal";
		node.filmStripRow[1] = 1;
		node.filmStripCol[1] = 1;
		node.zOrders[1] = GOAL_Z_ORDER;
		node.physOffsets[1] = Vec2(0.0f, 0.0f);
		node.relativeScale[1] = SAME_SCALE;

		nodes.push_back(node);


		AddGameObject<CharacterModel>(goal, scale, nodes);

		// don't want this obstacle to move (set center body to static)
		goal->setBodyType(b2_staticBody);
		goal->setDestructable(false);


		/*std::vector<float> verts;
		//top left corner
		verts.push_back(goalPosPhys.x - goalSize.width / 2);
		verts.push_back(goalPosPhys.y + goalSize.width / 2);

		//top right corner
		verts.push_back(goalPosPhys.x + goalSize.width / 2);
		verts.push_back(goalPosPhys.y + goalSize.width / 2);

		//bottom right corner
		verts.push_back(goalPosPhys.x + goalSize.width / 2);
		verts.push_back(goalPosPhys.y - goalSize.width / 2);

		//bottom left corner
		verts.push_back(goalPosPhys.x - goalSize.width / 2);
		verts.push_back(goalPosPhys.y - goalSize.width / 2);

		Poly2 wall1(&verts[0], verts.size());
		SimpleTriangulator triangulator;
		triangulator.set(wall1);
		triangulator.calculate();
		wall1.setIndices(triangulator.getTriangulation());
		wall1.setType(Poly2::Type::SOLID);
		
		std::shared_ptr<Polyzone> polywall1 = std::make_shared<Polyzone>();
		polywall1->initWithPoly(wall1, 1.0f, true);*/

		std::shared_ptr<LockGoal> goalCollision = std::make_shared<LockGoal>();
		goalCollision->init(_allObjects.get(), goal.get());
		goal->delegateLogicCollision(goalCollision);
		_goal = std::dynamic_pointer_cast<GoalCollision, LockGoal>(goalCollision);


		//const char *texKeys[1][1] = { { GOAL_TEXTURE } };
		/*int zOrders[1][1] = { { 10 } };
		int hitboxIdxs[1] = { 0 };
		int numHitboxNodes[1] = { 1 };
		int filmstripDim[2][1] = { { 1 }, { 1 } };
		Vec2 offsets[1][1] = { { Vec2(0.0f,0.0f) } };*/
		//const char ***texKeys;
		/*std::vector<nodesInit_t> nodes;
		nodesInit_t node;
		node.hitboxId = 0;
		node.numNodes = 2;
		node.textures[0] = GOAL_TEXTURE;
		node.filmStripRow[0] = 1;
		node.filmStripCol[0] = 1;
		node.zOrders[0] = GOAL_Z_ORDER;
		node.physOffsets[0] = Vec2(0.0f, 0.0f);
		node.relativeScale[0] = SAME_SCALE;
		Vec2 scale = Vec2(1.0f, 1.0f);

		node.textures[1] = "lock_goal";
		node.filmStripRow[1] = 1;
		node.filmStripCol[1] = 1;
		node.zOrders[1] = GOAL_Z_ORDER;
		node.physOffsets[1] = Vec2(0.0f, 0.0f);
		node.relativeScale[1] = SAME_SCALE;

		nodes.push_back(node);

		AddGameObject<Polyzone>(polywall1, scale, nodes);*/

		//AddGameObject<Polyzone>(polywall1, GOAL_TEXTURE, 15, Vec2(1.0f, 1.0f));
	}
	else {
		success = true;
		CULog("Goal already loaded");
	}

	return success;
}

void LevelStartup::addObstacle(const std::shared_ptr<cugl::Obstacle>& obj,
								const std::shared_ptr<cugl::Node>& node,
								int zOrder) {
	_world->addObstacle(obj);
	obj->setDebugScene(_debugnode);

	// Position the scene graph node (enough for static objects)
	node->setPosition(obj->getPosition()*_scale);
	_worldnode->addChild(node, zOrder);

	// Dynamic objects need constant updating
	if (obj->getBodyType() == b2_dynamicBody) {
		Node* weak = node.get(); // No need for smart pointer in callback
		obj->setListener([=](Obstacle* obs) {
			weak->setPosition(obs->getPosition()*_scale);
			weak->setAngle(obs->getAngle());
		});
	}
}


template <class T>
void LevelStartup::AddGameObject(std::shared_ptr<T> object, Vec2 objScale, std::vector<nodesInit_t> hitboxesWithNodes){
	
	// setup object
	object->setDrawScale(_scale);
	object->setDebugColor(DYNAMIC_COLOR);


	// find open spot for object and remember it
	int idx, version;
	_allObjects->put(object, &idx, &version);

	// remember where we put this for easy deletion
	object->setIdx(idx);
	object->setVersion(version);

	std::string name = "projectile_" + std::to_string(idx);
	object->setName(name);


	// setup hitboxes
	object->setupHitboxes(_world);
	object->setDebugScene(_debugnode);


	// add nodes to hitboxes
	for (int i = 0; i < hitboxesWithNodes.size(); i++) {
		nodesInit_t hitboxWithNodes = hitboxesWithNodes[i];
		AppendNodeToObject(object.get(), objScale, hitboxWithNodes, name);


		//for (int n = 0; n < hitboxWithNodes.numNodes; n++) {
		//	//CULog("set body %s\n", hitboxWithNodes.textures[n]);

		//	// get scale for individual node (based on whole object scale and relative scale)
		//	Vec2 nodeScale = objScale * hitboxWithNodes.relativeScale[n];

		//	Size objSize = getPhysicsSize(hitboxWithNodes.textures[n], nodeScale);

		//	//make projectiles (want function for this (and other objects in general))
		//	std::shared_ptr<Texture> image = _assets->get<Texture>(hitboxWithNodes.textures[n]);

		//	//std::shared_ptr<PolygonNode> gameNode = PolygonNode::allocWithTexture(image);


		//	std::shared_ptr<AnimationNode> gameNode = std::make_shared<AnimationNode>();

		//	gameNode->initWithFilmstrip(image, hitboxWithNodes.filmStripRow[n], hitboxWithNodes.filmStripCol[n]);
		//	/*if (isFilmstrip[i]) {
		//		gameNode->initWithFilmstrip(image, 1, 1);
		//	}
		//	else {
		//		gameNode->initWithTexture(image);
		//	}*/


		//	// need to scale down image in screen space to match physics size
		//	gameNode->setScale(nodeScale);
		//	gameNode->setAnchor(Vec2::ANCHOR_CENTER);

		//	// TODO can't set same objects with same name
		//	gameNode->setName(name);
		//	//object->setSceneGraphNode(gameNode);


		//	_worldnode->addChild(std::dynamic_pointer_cast<Node, AnimationNode>(gameNode), hitboxWithNodes.zOrders[n]);

		//	//add second animation node for crossfading
		//	/*std::shared_ptr<AnimationNode> crossfadeNode = std::make_shared<AnimationNode>();
		//	crossfadeNode->initWithFilmstrip(image, hitboxWithNodes.filmStripRow[n], hitboxWithNodes.filmStripCol[n]);
		//	crossfadeNode->setScale(objScale);
		//	crossfadeNode->setAnchor(Vec2::ANCHOR_CENTER);
		//	crossfadeNode->setName(name + "_crossfade");
		//	_worldnode->addChild(std::dynamic_pointer_cast<Node, AnimationNode>(crossfadeNode), hitboxWithNodes.zOrders[n]);*/

		//	std::shared_ptr<cugl::SimpleObstacle> physBody = object->getBody(hitboxWithNodes.hitboxId);

		//	//gameNode->setPosition(obj->getPosition()*_scale);

		//	gameNode->setPosition(physBody->getPosition()*_scale);
		//	object->addSceneGraphNode(gameNode, hitboxWithNodes.physOffsets[n], hitboxWithNodes.hitboxId);

		//	//add second animation node to object
		//	//crossfadeNode->setPosition(gameNode->getPosition());
		//	//object->addSceneGraphNode(crossfadeNode, hitboxWithNodes.physOffsets[n], hitboxWithNodes.hitboxId);

		//	/*if (hasBody) {
		//		object->addSceneGraphNode(gameNode, Vec2(0.0f, 0.0f), idx);
		//	}
		//	else {
		//		object->addSceneGraphNode(gameNode, Vec2(0.0f, objSize.width / 2.0f), idx);
		//	}*/
		//}

		/*if (hasBody) {
			//_worldnode->addChild(std::dynamic_pointer_cast<Node, AnimationNode>(gameNode), zOrder[idx]);

			//if (physBody->getBodyType() == b2_dynamicBody) { // camera doesn't work then
			Node* weak = gameNode.get(); // No need for smart pointer in callback
			physBody->setListener([=](Obstacle* obs) {
				weak->setPosition(obs->getPosition()*_scale);
			});
		}
		else { // TODO only works when one extra node.
			// doesn't't work b/c can't increase z-order beyound parent
			//object->getSceneGraphNode()->addChild(std::dynamic_pointer_cast<Node, AnimationNode>(gameNode), zOrder[idx]);
			
			Node* mainweak = object->getSceneGraphNode().get();
			Node* weak = gameNode.get(); // No need for smart pointer in callback
			physBody->setListener([=](Obstacle* obs) {
				mainweak->setPosition(obs->getPosition()*_scale);
				weak->setPosition((obs->getPosition() + Vec2(0.0f, objSize.width / 2.0f))*_scale);
			});
		
		}*/

	}

	// add any child nodes

}

void LevelStartup::AppendNodeToObject(PhysicalObject *object, Vec2 objScale, nodesInit_t hitboxWithNodes, std::string name) {
	for (int n = 0; n < hitboxWithNodes.numNodes; n++) {
		//CULog("set body %s\n", hitboxWithNodes.textures[n]);

		// get scale for individual node (based on whole object scale and relative scale)
		Vec2 nodeScale = objScale * hitboxWithNodes.relativeScale[n];

		Size objSize = getPhysicsSize(hitboxWithNodes.textures[n], nodeScale);

		//make projectiles (want function for this (and other objects in general))
		std::shared_ptr<Texture> image = _assets->get<Texture>(hitboxWithNodes.textures[n]);
		std::shared_ptr<AnimationNode> gameNode = std::make_shared<AnimationNode>();

		gameNode->initWithFilmstrip(image, hitboxWithNodes.filmStripRow[n], hitboxWithNodes.filmStripCol[n]);


		// need to scale down image in screen space to match physics size
		gameNode->setScale(nodeScale);
		gameNode->setAnchor(Vec2::ANCHOR_CENTER);
		gameNode->setName(name);


		_worldnode->addChild(std::dynamic_pointer_cast<Node, AnimationNode>(gameNode), hitboxWithNodes.zOrders[n]);


		std::shared_ptr<cugl::SimpleObstacle> physBody = object->getBody(hitboxWithNodes.hitboxId);

		gameNode->setPosition(physBody->getPosition()*_scale);
		object->addSceneGraphNode(gameNode, hitboxWithNodes.physOffsets[n], hitboxWithNodes.hitboxId);
	}
}


/*template <class T>
void LevelStartup::AddGameObject(std::shared_ptr<T> object, const char *texKey, int zOrder, Vec2 objScale) {
	Size objSize = getPhysicsSize(texKey, objScale);

	//make projectiles (want function for this (and other objects in general))
	std::shared_ptr<Texture> image = _assets->get<Texture>(texKey);
	object->setDrawScale(_scale);
	object->setDebugColor(DYNAMIC_COLOR);


	// find open spot for object and remember it
	int idx, version;
	_allObjects->put(object, &idx, &version);

	// remember where we put this for easy deletion
	object->setIdx(idx);
	object->setVersion(version);

	std::string name = "projectile_" + std::to_string(idx);
	object->setName(name);

	std::shared_ptr<PolygonNode> gameNode = PolygonNode::allocWithTexture(image);

	// need to scale down image in screen space to match physics size
	gameNode->setScale(objScale);
	gameNode->setAnchor(Vec2::ANCHOR_CENTER);
	gameNode->setName(name);
	object->setSceneGraphNode(gameNode);

	object->setupHitboxes(_world);

	object->setDebugScene(_debugnode);

	auto physBody = object->getBody(0);

	gameNode->setPosition(physBody->getPosition()*_scale);
	_worldnode->addChild(std::dynamic_pointer_cast<Node, PolygonNode>(gameNode), zOrder);


	Node* weak = gameNode.get(); // No need for smart pointer in callback
	physBody->setListener([=](Obstacle* obs) {
		weak->setPosition(obs->getPosition()*_scale);
	});
}*/

bool LevelStartup::AddProjectile(const attackInfo_t projInfo, cugl::Vec2 dispVec) {
	//std::shared_ptr<Projectile> newProjectile = Projectile::alloc(Vec2::ZERO, Vec2(PROJECTILE_SIZE, PROJECTILE_SIZE));
	/*const char *texKeys[1] = { EARTH_TEXTURE };
	int zOrders[1] = { 10 };
	int hitboxIdxs[1] = { 0 };
	int filmstripDim[2] = { 1, 1 };
	bool hasBody[1] = { true };
	int numNodes = 1;*/

	std::vector<nodesInit_t> nodes;
	nodesInit_t node;
	node.hitboxId = 0;
	node.numNodes = 1;
	node.textures[0] = projInfo.texKey.c_str(); // TODO store in attack info
	node.filmStripRow[0] = 1;
	node.filmStripCol[0] = 1;
	node.zOrders[0] = BULLET_Z_ORDER;
	node.physOffsets[0] = Vec2(0.0f, 0.0f);
	node.relativeScale[0] = SAME_SCALE;

	nodes.push_back(node);


	std::shared_ptr<Projectile> newProjectile = Projectile::alloc(projInfo);
	AddGameObject<Projectile>(newProjectile, projInfo.scale, nodes);

	// make sure newly added projectile is rendered in order
	_root->sortZOrder();

	//Weapon::projectileSetup(_rocket.get(), newProjectile.get(), dispVec);
    newProjectile->setOwner((PhysicalObject*)projInfo.owner);
	return true;
}

Size LevelStartup::getPhysicsSize(const char *texKey, Vec2 objScale) {
	std::shared_ptr<Texture> image = _assets->get<Texture>(texKey);
	Size objSize(image->getSize() / _scale * objScale); // physics size

	return objSize;
}

void LevelStartup::unload() {
	return;
}

