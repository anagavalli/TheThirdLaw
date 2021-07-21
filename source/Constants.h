#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024.0f
//width and height of the illustrator path reference, in pixels
#define PATH_WIDTH 2001.59961f
#define PATH_HEIGHT 1166.3999f

//#define IMAGE_WIDTH 8340.0f / 2.0f
//#define IMAGE_HEIGHT 4860.0f / 2.0f

/** Width of the game world in Box2d units */
//#define DEFAULT_WIDTH   (IMAGE_WIDTH*32.0f/1024.0f)
/** Height of the game world in Box2d units */
//#define DEFAULT_HEIGHT  (IMAGE_HEIGHT*32.0f/1024.0f)

/** The default value of gravity (going down) */
//#define DEFAULT_GRAVITY -4.9f

/** PBB no gravity */
#define DEFAULT_GRAVITY 0.0f

/** PBB max number of objects allowed in scene */
#define MAX_OBJECTS 250

#define NEWTON_SCALE        0.25f
//#define NEWTON_SCALE        0.125f
#define ALLOWED_ORIENTATIONS     16

#define PROJECTILE_SIZE 1.0f

// shot velocity magnitude 7.875f
#define RECOIL_MOD 8.0f
// velocity loss upon collision
#define BOUNCE_MOD 0.80f

#define NEWTON_HEALTH 10

#define RECOIL_SPLINE 0
#define SPEED_UP_SPLINE 1

#define CHARACTER_ELLIPSE_SEGS 15

#define STANDARD_HITBOX 0
#define ELLIPSE_HITBOX 1

#define IDLE_DURATION_MS 2000

#define VICTORY_PUMPS 3

#define GOAL_TEXTURE        "goal"


// z-orders
#define MAP_Z_ORDER 0
#define LAVA_Z_ORDER 1
#define SHADOW_Z_ORDER 2
#define ZONE_Z_ORDER 3
#define GOAL_Z_ORDER 3
#define CHARACTER_Z_ORDER 4
#define SHIELD_Z_ORDER 5
#define BULLET_Z_ORDER 6
#define OBSTACLE_Z_ORDER 7
#define HP_FLASH_Z_ORDER 8
#define HP_BOT_Z_ORDER 9
#define HP_TOP_Z_ORDER 10
#define ELEVATED_Z_ORDER 11
//#define HITBOX_BOT_Z_ORDER 11
//#define HITBOX_TOP_Z_ORDER 12

#define HITBOX_Y_OFFSET 0.78f

#define SAME_SCALE cugl::Vec2(1.0f,1.0f)

/** Color to outline the physics nodes */
#define STATIC_COLOR    Color4::YELLOW
/** Opacity of the physics outlines */
#define DYNAMIC_COLOR   Color4::GREEN



// -------- AUDIO CONSTANTS -------
/** The key for collisions sounds */
#define COLLISION_SOUND     "bump"
/** The key for the main afterburner sound */
#define MAIN_FIRE_SOUND     "burn"
/** The key for the right afterburner sound */
#define RGHT_FIRE_SOUND     "right-burn"
/** The key for the left afterburner sound */
#define LEFT_FIRE_SOUND     "left-burn"

/** ---- The keys for Level Music ---- */
/** The key for collisions sounds */
#define LVL_TUT_MUSIC       "tutorial"
#define LVL_ONE_MUSIC       "level1"
#define LVL_TWO_MUSIC       "level2"
#define LVL_FUR_MUSIC       "level4"
#define LVL_FVE_MUSIC       "level5"
#define LVL_BSS_MUSIC       "boss"


/** ---- The keys for Newton ---- */
/** The key for Newton getting hit by a projectile */
#define NEWTON_HIT          "newton-hit"
/** The key for Newton shooting a projectile */
#define NEWTON_SHOOT        "newton-shoot"

/** ---- The keys for Enemies ---- */
/** The key for Enemies getting hit by a projectile */
#define ENEMY_HIT            "enemy-hit"
/** The key for Enemies shooting a projectile */
#define ENEMY_SHOOT          "enemy-shoot"
/** The key for Enemies shooting a projectile */
#define ENEMY_MOVE1          "enemy-move1"
/** The key for Enemies shooting a projectile */
#define ENEMY_MOVE2          "enemy-move2"
/** The key for Enemies dying */
#define ENEMY_DEATH1        "enemy-death1"
/** The key for Enemies dying */
#define ENEMY_DEATH2        "enemy-death2"

/** The key for the font reference */
#define PRIMARY_FONT        "retro"

#pragma mark Physics Constants

#define PI    3.14159265359f

// Physics constants for initialization
/** Density of non-crate objects */
#define BASIC_DENSITY       0.0f
/** Friction of non-crate objects */
#define BASIC_FRICTION      0.1f
/** Collision restitution for all objects */
#define BASIC_RESTITUTION   0.1f
/** Threshold for generating sound on collision */
#define SOUND_THRESHOLD     3
/** Default speed object needs to be traveling to kill an object*/
#define KILL_SPEED 9.5f
/** Since we remember collision to avoid re-collision, need to forgot to allow later collision*/
#define COLLISION_RESET_TIME 200
/** Speed up zone modifier */
#define SPEED_UP_MULT 1.1f
/** Speed down zone modifier*/
#define SPEED_DOWN_MULT 0.5f
/** Number of nodes we can attach to a single hitbox*/
#define MAX_NODES_PER_HITBOX 8

/** Damage dealt by shock wall*/
#define SHOCK_WALL_DAMAGE 2

#define BOUNDARY_DAMAGE 1

#define NEWTON_FIRE_RATE 350
#define NEWTON_PROJ_SPEED 5.25f
#define NEWTON_PROJ_DMG 1
/** Stop newton from shooting behind him when going above a certain speed */
#define NEWTON_STOP_TURN_SPEED 3.5f
// ~(8.0f * PI) / 9.0f - 0.1f
#define NEWTON_BLOCK_ANGLE 2.775f




// AI constants

/** coarseness of A* search */
#define SEARCH_BLOCK_SIZE 2.5f


#define BLOCKED_TERRAIN 9

#define UNBLOCKED_TERRAIN 1

/** How closely a path needs to be followed */
#define PATH_GRANULARITY 0.01f

/** Speed of seeker enemy (vel = this speed * chosen direction)*/
#define SEEKER_SPEED 4.0f
/** Define this along with gun fire-rate (maybe shouldn't have both)*/
#define SEEKER_FIRE_RATE 2000
#define SEEKER_PROJ_SPEED 4.5f
#define SEEKER_PROJ_DMG 2
/** How many physics units away before seeker aggros */
#define SEEKER_AGGRO_DISTANCE 12.0f
/** If an enemy is already aggroed, how many physics units before it is unaggroed*/
#define SEEKER_UN_AGGRO_DISTANCE 20.0f
/** How many physics units away before seeker starts to shoot*/
#define SEEKER_SHOOT_DISTANCE 10.0f
/** Determines how fast can turn towards newton */
#define SEEKER_TURNING_RATE 3.0f
/** Speed up if enemy is far away*/
#define SEEKER_DISTANCE_SPEED_UP 0.25f

#define SEEKER_HEALTH 2

#define SEEKER_SCALE 1.0f

#define SHIELD_BOSS_SPEED 4.0f
#define SHIELD_BOSS_FIRE_RATE 2250
#define SHIELD_BOSS_PROJ_SPEED 4.5f
#define SHIELD_BOSS_PROJ_DMG 2
#define SHIELD_BOSS_AGGRO_DISTANCE 20.0f
#define SHIELD_BOSS_UN_AGGRO_DISTANCE 20.0f
#define SHIELD_BOSS_SHOOT_DISTANCE 20.0f
#define SHIELD_BOSS_FENCE_RADIUS 0.0f
#define SHIELD_BOSS_HEALTH 5
#define SHIELD_BOSS_HITBAR_Y_OFFSET 0.7f


#define CHASE_BOSS_SPEED 10.25f
#define CHASE_BOSS_FIRE_RATE 2250
#define CHASE_BOSS_PROJ_SPEED 14.0f
#define CHASE_BOSS_PROJ_DMG 4
#define CHASE_BOSS_SHOOT_DISTANCE 40.0f
#define CHASE_BOSS_CATCH_UP_DIST 18.5f
#define CHASE_BOSS_CATCH_UP_SPEED_MOD 0.1f
#define CHASE_BOSS_HEALTH 16
#define CHASE_BOSS_HITBAR_Y_OFFSET 0.7f


// colors for a star visualization
#define MISS_COLOR Color4::BLUE
#define HIT_COLOR Color4::RED
#define GOAL_COLOR Color4::PAPYRUS
/** Maximum allowed search boxes in grid */
#define MAX_SEARCH_BOXES 100

#endif
