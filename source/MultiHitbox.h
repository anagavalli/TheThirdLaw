/*
* Handles multihitboxes on one object (node)
*/

#ifndef __MULTI_HITBOX_H__
#define __MULTI_HITBOX_H__

#include "cugl/cugl.h"
#include "Box2D/Dynamics/Joints/b2DistanceJoint.h"
#include "Box2D/Dynamics/Joints/b2WeldJoint.h"
#include "Collision.h"
#include "LogicCollision.h"
#include "DefendCollision.h"
//#include "PhysicalObject.h"

class MultiHitbox {
protected:
	// phys body with no shape. hitboxes are specfied relative to this.
	//std::shared_ptr<cugl::SimpleObstacle> _physBody;
	//std::weak_ptr<cugl::SimpleObstacle> _centerHitbox;

	typedef struct hitboxlog {
		std::shared_ptr<cugl::SimpleObstacle> hitbox;
		cugl::Vec2 offset;
		b2Joint *joint;
		std::shared_ptr<Collision> physCol;
		std::shared_ptr<LogicCollision> logicCol;
		std::shared_ptr<DefendCollision> defendCol;
		//std::shared_ptr<cugl::PolygonNode> gfxNode;
		std::vector<std::shared_ptr<cugl::AnimationNode>> gfxNodes;
		std::vector<cugl::Vec2> nodePhysOffsets;
	} hitbox_t;

	std::vector<std::shared_ptr<hitbox_t>> _hitboxes;
	std::queue<int> _hitboxDeactQueue;


	// hitboxes of object
	//std::vector<std::shared_ptr<cugl::SimpleObstacle>> _hitboxes;
	/*std::vector<cugl::Vec2> _offsets;
	std::vector<std::shared_ptr<b2FixtureDef>> _fixtures;
	std::vector<std::shared_ptr<b2PolygonShape>> _shapes;*/
	//std::vector<std::shared_ptr<b2DistanceJointDef

public:
	void dispose();
	~MultiHitbox() { dispose(); }
	//virtual void init(cugl::Vec2 pos);
	virtual void init(std::shared_ptr<cugl::SimpleObstacle> initHitbox);
	//void addHitbox(std::shared_ptr<cugl::SimpleObstacle> newHitbox);
	//void addHitbox(b2PolygonShape *shape, std::shared_ptr<cugl::SimpleObstacle> newHitbox, cugl::Vec2 hitboxOffset);
	void addHitbox(std::shared_ptr<cugl::SimpleObstacle> newHitbox, cugl::Vec2 hitboxOffset,
		std::shared_ptr<Collision> physCol, std::shared_ptr<LogicCollision> logicCol,
		std::shared_ptr<DefendCollision> defCol);


	void appendSceneGraphNode(int hitboxIdx, std::shared_ptr<cugl::AnimationNode> node, cugl::Vec2 offset, float drawscale);
	std::vector<std::shared_ptr<cugl::AnimationNode>> getAnimationNodes(int hitboxIdx);

	void setupHitboxes(std::shared_ptr<cugl::ObstacleWorld> world);
	void appendHitbox(int id, cugl::ObstacleWorld *world);
	void adjustHitboxes();
	void removeHitboxes(std::shared_ptr<cugl::ObstacleWorld> world, std::shared_ptr<cugl::Node> worldNode);
	void deactivateHitbox(cugl::SimpleObstacle *hitbox);
	bool reactivateHitbox(int hitboxIdx);

	int findHitboxIdx(cugl::SimpleObstacle *hitbox);
	void procDeactivations(std::shared_ptr<cugl::ObstacleWorld> world, std::shared_ptr<cugl::Node> worldNode);


	//void onCollide(int hitboxIdx, void *obj, void *otherObj, cugl::Vec2 normalVec);
	void onCollide(cugl::SimpleObstacle *hitbox, void *obj, void *otherObj, cugl::Vec2 normalVec, bool noPhysCollision);
	bool onAttacked(void* obj, cugl::SimpleObstacle *hitbox, int damage, cugl::Vec2 velocity, bool shieldBreak);

	// define physics functions by accessing hitbox
	void setDensity(float density);
	void setFriction(float friction);
	void setRestitution(float rest);
	void setFixedRotation(bool isFixed);
	bool isActive();

	// TODO
	cugl::Vec2 getPosition();
	void setPosition(cugl::Vec2 pos);

	float getAngle();
	void setLinearVelocity(cugl::Vec2 vel);
	cugl::Vec2 getVelocity();
	void setAngle(float angle);
	void setBullet(bool isBullet);
	void ApplyForceToCenter(b2Vec2 forceVec, bool wakeup);

	int getIdx();
	int getVersion();
	void setIdx(int idx);
	void setVersion(int version);

	std::string getName();
	void setName(std::string name);

	void setDebugScene(const std::shared_ptr<cugl::Node> &node);
	void setDebugColor(cugl::Color4 color);
	void setBodyType(b2BodyType type);

	//std::vector<std::shared_ptr<cugl::SimpleObstacle>> getHitbox();

	//std::shared_ptr<cugl::SimpleObstacle> getMainBody() { return _hitboxes[0]; }
	std::shared_ptr<cugl::SimpleObstacle> getBody(int i) { return _hitboxes[i]->hitbox; }


	
};



#endif
