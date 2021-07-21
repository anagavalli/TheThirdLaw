#include "Camera2d.h"

Camera2d::Camera2d(float objectScale, cugl::Vec2 screenCenter, float lazyMovementThresh, float cameraSpeed) {
	_objectScale = objectScale;
	_screenCenter = screenCenter;
	_lazyMovementThresh = lazyMovementThresh;
	_cameraSpeed = cameraSpeed;
	_timeSinceLastPan.mark();
	_movementVec = cugl::Vec2::ZERO;
	_cameraPosPhys = cugl::Vec2::ZERO;
}

void Camera2d::dispose() {
	return;
}

void Camera2d::pan(cugl::Vec2 panDir, std::shared_ptr<ObjectContainer> objects, std::shared_ptr<cugl::PolygonNode> mapSprite) {
	cugl::Vec2 currMapPos = mapSprite->getPosition(); //add into it's own object list?
	mapSprite->setPosition(currMapPos + panDir);


	cugl::Vec2 physMove = panDir / _objectScale;
	for (int i = 0; i < objects->getSize(); i++) {
		std::shared_ptr<PhysicalObject> obj = objects->get(i);
		cugl::Vec2 objPos = obj->getPosition();
		obj->setPosition(objPos + physMove); //(panDir/_objectScale));
	}

	_cameraPosPhys += physMove;
}

// follows object perfectly every frame
void Camera2d::follow(PhysicalObject *followObj, 
	std::shared_ptr<ObjectContainer> objects, std::shared_ptr<cugl::PolygonNode> mapSprite) {

	cugl::Vec2 followObjScreenSpace = followObj->getSceneGraphNode()->getPosition();
	cugl::Vec2 diffVec = _screenCenter - followObjScreenSpace;

	//CULog("%f %f - %f %f\n", newtonScreenSpace.x, newtonScreenSpace.y, cameraPos.x, cameraPos.y);
	pan(diffVec, objects, mapSprite);

	//_worldnode->setPosition(_worldnode->getPosition() + panDir); // physics world does not move if just do this
}

void Camera2d::lazyFollow(PhysicalObject *followObj, 
	std::shared_ptr<ObjectContainer> objects, std::shared_ptr<cugl::PolygonNode> mapSprite) {
	// calculate distance between object and center
	cugl::Vec2 followObjScreenSpace = followObj->getSceneGraphNode()->getPosition();
	cugl::Vec2 diffVec = _screenCenter - followObjScreenSpace;

	//CULog("%f %f - %f %f, %f\n", followObjScreenSpace.x, followObjScreenSpace.y, diffVec.x, diffVec.y, diffVec.length());
	// if far enough away, then start moving the camera
	if (diffVec.length() > _lazyMovementThresh) { // need hysterisis!
		//_movementVec = diffVec * _cameraSpeed; // should be constant speed?
		_movementVec = diffVec.getNormalization() * _cameraSpeed; // player should not be able to go faster than the camera if do this!
	}
	else { // exponential decay
		_movementVec *= 0.5f;
		if (_movementVec.isNearZero()) {
			_movementVec = cugl::Vec2::ZERO;
		}
	}

	// apply panning based on current motion
	cugl::Timestamp currTime;
	currTime.mark();
	float timeBetweenPans = (float)cugl::Timestamp::ellapsedMillis(_timeSinceLastPan, currTime);
	_timeSinceLastPan.mark();

	cugl::Vec2 framePan = _movementVec * timeBetweenPans;

	float maxDistX = _lazyMovementThresh*0.5f;
	float maxDistY = _lazyMovementThresh*0.25f;

	if (abs(diffVec.x) > maxDistX) {
		float deltaX = (diffVec.x > 0) ? maxDistX : ((diffVec.x < 0) ? -1*maxDistX : 0);
		framePan.set(diffVec.x - deltaX, framePan.y);
	}
	if (abs(diffVec.y) > maxDistY) {
		float deltaY = (diffVec.y > 0) ? maxDistY : ((diffVec.y < 0) ? -1 * maxDistY : 0);
		framePan.set(framePan.x, diffVec.y - deltaY);
	}

	pan(framePan, objects, mapSprite);
}

