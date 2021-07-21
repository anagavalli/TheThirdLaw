#ifndef __POLY_ZONE_H__
#define __POLY_ZONE_H__

#include "PhysicalObject.h"
#include "cugl/cugl.h"

class Polyzone : public PhysicalObject {
private:
	//std::shared_ptr<cugl::PolygonNode> _polyNode;

	// TODO
	//cugl::Vec2 tempPos;
	bool _lockImageToMesh;
	cugl::Poly2 _poly;

public:

	~Polyzone() { dispose(); }

	static std::shared_ptr<Polyzone> alloc(const cugl::Vec2& pos, const cugl::Size& size) {
		std::shared_ptr<Polyzone> result = std::make_shared<Polyzone>();
		return (result->init(pos, size) ? result : nullptr);
	}

	virtual bool init(const cugl::Vec2& pos, const cugl::Size& size) override;

	bool initWithPoly(const cugl::Poly2 poly, float scale, bool lockImageToMesh);

	void dispose() override;

	//cugl::Poly2 getPoly() override { return _poly; }
	//void update(float delta);

	// TODO
	//virtual cugl::Vec2 getPosition() override { return tempPos; }
	//virtual void setPosition(cugl::Vec2 pos) override { tempPos = pos; }
	//void createHitbox(cugl::Vec2 pos, const cugl::Size &size) override;
	std::shared_ptr<cugl::SimpleObstacle> createHitbox(cugl::Vec2 pos, const cugl::Size &size) override;

	void setSceneGraphNode(std::shared_ptr<cugl::AnimationNode>& node) override;
	//void setPolyObstacle(std::shared_ptr<cugl::PolygonObstacle> &obs);
	//void addSceneGraphNode(const std::shared_ptr<cugl::AnimationNode>& node, int hitboxIdx);
};


#endif
