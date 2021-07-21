#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__
#include <cugl/cugl.h>
#include "PhysicalObject.h"
#include "Attack.h"

class Projectile : public PhysicalObject {
private:
	

public:
	~Projectile() { dispose(); }

	void dispose() override;

	static std::shared_ptr<Projectile> alloc(const cugl::Vec2& pos, const cugl::Size& size) {
		std::shared_ptr<Projectile> result = std::make_shared<Projectile>();
		return (result->init(pos, size) ? result : nullptr);
	}

	virtual bool init(const cugl::Vec2& pos, const cugl::Size& size) override;

	static std::shared_ptr<Projectile> alloc(const attackInfo_t attackInfo) {
		std::shared_ptr<Projectile> result = std::make_shared<Projectile>();
		return (result->init(attackInfo) ? result : nullptr);
	}

	virtual bool init(const attackInfo_t attackInfo);
    
};

#endif
