/* 
* Used for camera panning. Camera is not a real thing so cannot actually move.
* To simulate camera movement we move every object (camera, map, invisible zones/boundaries) in the opposite direction of the camera.
* For example, if the camera moves to the right, then characters should move off the left side of the screen.
*/

#ifndef __CAMERA2D_H__
#define __CAMERA2D_H__

#include "cugl/cugl.h"
#include "PhysicalObject.h"
#include "ObjectContainer.h"

class Camera2d {
private:
	float _objectScale;
	cugl::Vec2 _screenCenter;
	cugl::Vec2 _movementVec;
	float _lazyMovementThresh;
	float _cameraSpeed;
	cugl::Timestamp _timeSinceLastPan;
	cugl::Vec2 _cameraPosPhys; // camera position in physics space (so can have fixed values)
public:
	Camera2d(float objectScale, cugl::Vec2 screenCenter, float lazyMovementThresh, float cameraSpeed);
	~Camera2d() { dispose(); }

	void pan(cugl::Vec2 panDir, std::shared_ptr<ObjectContainer> objects, std::shared_ptr<cugl::PolygonNode> mapSprite);
	void follow(PhysicalObject *followObj, std::shared_ptr<ObjectContainer> objects, std::shared_ptr<cugl::PolygonNode> mapSprite);
	void lazyFollow(PhysicalObject *followObj, std::shared_ptr<ObjectContainer> objects, std::shared_ptr<cugl::PolygonNode> mapSprite);
	void dispose();

	cugl::Vec2 getPosition() const { return _screenCenter; }
	cugl::Vec2 getPhysPos() const { return _cameraPosPhys; }
};



#endif