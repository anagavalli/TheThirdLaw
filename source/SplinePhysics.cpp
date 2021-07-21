
#include "SplinePhysics.h"
#include <cugl/cugl.h>
#include "Constants.h"

using namespace cugl;

//#define PI	3.14159265359f
#define NUM_SAMPLES 10

// each response lasts for 3s
#define TIME_LENGTH_MS 2500


SplinePhysics::SplinePhysics(float scale, float physVertShift, float recoilMod, float bounceMod, bool berserker) {
	/*_scale = scale;
	_physVertShift = physVertShift;
	_recoilMod = recoilMod;*/
	init(scale, physVertShift, recoilMod, bounceMod, berserker);
}

void SplinePhysics::init(float scale, float physVertShift, float recoilMod, float bounceMod, bool beserker) {
	_scale = scale;
	_physVertShift = physVertShift;
	_recoilMod = recoilMod;
	_bounceMod = bounceMod;
	_isBeserker = beserker;
}


void SplinePhysics::recoil(cugl::Vec2 touchLoc, std::shared_ptr<PhysicalObject> obj, cugl::Vec2 *dispVec) {
	// get vector between character and touch location
	// touch (screen space) and rocket (physics space) are not in the same coordinate space!!!
	// Choose to do computation in screen space for now. 
	// Make sure to add known scaling factor AND invert y 
	// 0,0 is top left in screen space but 0,0 is bottom left in physics space
	// Need to flip y vec prob due to discrepancy between phys and screen space
	cugl::Vec2 pos = obj->getPosition() * _scale;
	pos.y = _physVertShift - pos.y;
	*dispVec = pos - touchLoc;
	dispVec->y = -1.0f * dispVec->y;

	// How to specify body parameter without using Box2D dynamics (mostly cugl wrappers)
	// If specify these, then can prob still use Box2D collision system
	//_rocket->setPosition(0.0f, 0.0f);
	//_rocket->setAngle(0.0f);
	//_rocket->setLinearVelocity(0.0f, 0.0f);
	//_rocket->setAngularVelocity(0.0f);
	//_rocket->setForce(cugl::Vec2(0.0f, 0.0f)); // Purely a RDRocketModel.cpp func, calls ApplyForceToCenter() set acceleration using impulse? F = ma bro!
	//_rocket->setFriction(0.5f); // coef of dynamic friction colliding with a body
	//_rocket->setBullet(true); // turns on ray cast collision detection (off by default)

	// some extra box2d functions (no cugl wrapper so need to use b2Vec2 not cuglVec2)!
	//_rocket->getBody()->ApplyForceToCenter(b2Vec2(0.0f, 0.0f), true);
	//_rocket->getBody()->ApplyLinearImpulseToCenter(b2Vec2(0.0f, 0.0f), true);
	//_rocket->getBody()->SetLinearDamping(0.0f); //try negative to get large resp in beginning?

	// We don't care about magnitude of vector. 
	// (Magnitude/scaling doesn't matter so doesn't matter if we use screen space or physics space)
	dispVec->normalize();

	// apply recoil to character
	obj->setFX(dispVec->x * obj->getForceMod());
	obj->setFY(dispVec->y * obj->getForceMod());
	obj->applyForce();

	CULog("Touch: %f %f Rocket: %f %f Disp: %f %f \n", touchLoc.x, touchLoc.y, pos.x, pos.y, dispVec->x, dispVec->y);

	// launch bullet!

}

/*void Physics::projectileSetup(PhysicalObject *creator, PhysicalObject *proj, cugl::Vec2 dispVec) {

	// start at edge of character with initial velocity
	// get angle of shot (dispVec) use trig to find where edge of hit box is
	// punt for now
	int angle = int(dispVec.getAngle() * 180.0 / 3.1454); //-pi - pi radians
	float distToEdge = 1.25f; //2.0f * sinf(angle % 4) * hitBox.y; //TODO: BAD



	cugl::Vec2 projPos = creator->getPosition() - (distToEdge * dispVec);
	proj->setPosition(projPos);
	proj->setLinearVelocity(dispVec * -3.0f);
}*/

/*int Physics::getOrientation(cugl::Vec2 dispVec, int numOrientations) {
	// getAngle() is measured from -x axis and goes clockwise. newton model 0 is on -y axis and goes counterclockwise
	float newtonAngle = (PI / 2.0f) - dispVec.getAngle(); //pi/2 - angle

	
	// get newton angle
	// make angle positive if negative
	if (newtonAngle < 0.0f) {
		float addon = PI + newtonAngle;
		newtonAngle = PI + addon; //2pi + newtonAngle?
	}

	// discretize
	float sector = (2.0f * PI) / numOrientations;

	// don't just round down (i.e. integer cast) b/c will favor certain orientations
	float div = newtonAngle / sector;
	int pic = int(round(div));

	//wrap around
	if (pic == numOrientations) {
		pic = 0;
	}

	CULog("Angle: %f -> %f %d\n", dispVec.getAngle(), newtonAngle, pic);

	return pic;
}*/

void SplinePhysics::createSpline() {
	//std::vector<float> knotVec = { 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.3f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f };
	std::vector<float> knotVec = { 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.6f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f };

	
	using namespace cugl;

	// 4 splines. attack(a) , decay(d), sustain(s), release(r) like in music
	// 4 pts used in each cubic spline
	// maximum should be 1.0 and we use a global scaling factor after
	/*std::vector<Vec4> ctrlPts = {
		Vec4(0.0,0,0,0.1),//a
		Vec4(1.0,0,0,3), //a,d		(i.e. used in attack and decay spline)
		Vec4(0.45,0,0,1), //a,d,s
		Vec4(0.4,0,0,1),  //a,d,s,r
		Vec4(0.375,0,0,1),  //d,s,r
		Vec4(0.0,0,0,3),  //s,r
		Vec4(0.0,0,0,1) };//r*/

	std::vector<Vec4> ctrlPts = {
		Vec4(0.0,0,0,0.1),//a
		Vec4(1.0,0,0,3), //a,d		(i.e. used in attack and decay spline)
		Vec4(0.35,0,0,1), //a,d,s
		Vec4(0.3,0,0,1),  //a,d,s,r
		Vec4(0.275,0,0,1),  //d,s,r
		Vec4(0.0,0,0,3),  //s,r
		Vec4(0.0,0,0,1) };//r

	int degree = 3;

	//_respCurve = std::make_shared<Nurbs>(knotVec, ctrlPts, degree);
	_respCurves.push_back(std::make_shared<Nurbs>(knotVec, ctrlPts, degree));


	std::vector<float> knotVecZone = { 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.2f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f };
	std::vector<Vec4> ctrlPtsZone = {
		Vec4(0.0,0,0,1),//a
		Vec4(0.75,0,0,1), //a,d		(i.e. used in attack and decay spline)
		Vec4(0.9,0,0,1), //a,d,s
		Vec4(0.9,0,0,1),  //a,d,s,r
		Vec4(0.9,0,0,1),  //d,s,r
		Vec4(0.2,0,0,1),  //s,r
		Vec4(0.0,0,0,1) };//r

	_respCurves.push_back(std::make_shared<Nurbs>(knotVecZone, ctrlPtsZone, degree));

}


//auto nurbs = std::make_shared<Nurbs>(knotVec, ctrlPts, degree);
/*std::vector<cugl::Vec2> pts;
pts.push_back(cugl::Vec2(0.0f, 0.0f));
pts.push_back(cugl::Vec2(0.25f, 0.25f));
pts.push_back(cugl::Vec2(0.0f, 0.5f));
pts.push_back(cugl::Vec2(0.5f, 0.5f));
pts.push_back(cugl::Vec2(0.75f, 0.5f));
pts.push_back(cugl::Vec2(0.6f, 0.6f));
pts.push_back(cugl::Vec2(0.1f, 0.6f));
pts.push_back(cugl::Vec2(1.0f, 1.0f)); //can't have four points for some reason
auto spline = std::make_shared<cugl::CubicSpline>(pts);
//std::vector<cugl::Vec2> ctrl = spline->getControlPoints(); //what is specified in pts

spline->getPoint(0.5f);*/


//void Physics::recoilSpline(cugl::Vec2 touchLoc, std::shared_ptr<PhysicalObject> obj, cugl::Vec2 *dispVec) {
void SplinePhysics::recoilSpline(cugl::Vec2 direction, PhysicalObject *obj) {
	/*cugl::Vec2 pos = obj->getPosition() * _scale;
	pos.y = _physVertShift - pos.y;
	*dispVec = pos - touchLoc;
	dispVec->y = -1.0f * dispVec->y;

	dispVec->normalize();*/
	//CULog("%f %f\n", direction.x, direction.y);
	//*dispVec = getDirectionFromTouch(touchLoc, obj->getPosition());
	
	// add new response instance
	auto resp = std::make_shared<resp_t>();
	resp->direction = direction.getNormalization(); // store direction of this shot
	resp->speedMod = direction.length();
	resp->startTime = cugl::Timestamp();
	resp->startTime.mark(); // store this time
	resp->lastTimeSampled = cugl::Timestamp();
	resp->lastTimeSampled.mark(); // store this time
	resp->splineIdx = RECOIL_SPLINE;
	_respStartTimes.push_back(resp);
	// should we apply resp here? or wait till next frame?

	// apply recoil to character
	//obj->setFX(dispVec->x * obj->getForceMod());
	//obj->setFY(dispVec->y * obj->getForceMod());
	//obj->applyForce();


}

// apply all valid responses to character
//void Physics::applyResps(std::shared_ptr<PhysicalObject> obj) {
void SplinePhysics::applyResps(PhysicalObject *obj) {
	// get current time
	cugl::Timestamp currTime = cugl::Timestamp();
	currTime.mark();

	cugl::Vec2 velocity = cugl::Vec2::ZERO;


	// "Drift effect" -> apply resp from multiple shots
	for (int i = 0; i < _respStartTimes.size(); i++) {
		
		// get direction 
		cugl::Vec2 respDir = _respStartTimes[i]->direction;
		cugl::Timestamp lastTimeSampled = _respStartTimes[i]->lastTimeSampled;

		// set velocity based on time in curve
		auto elapsedTimeInt = cugl::Timestamp::ellapsedMillis(_respStartTimes[i]->startTime, currTime);
		float t = (float)elapsedTimeInt / (float)TIME_LENGTH_MS;
		//CULog("processing %d - delta %f %lu\n", i, t, elapsedTimeInt);

		float speedMultiplier = _respStartTimes[i]->speedMod;
		int splineIdx = _respStartTimes[i]->splineIdx;


		// we no longer consider response because too old
		if (t > 1.0f) {
			// should also multi sample here
			_respStartTimes.erase(_respStartTimes.begin() + i);
			t = 1.0f;
		}
		else {
			_respStartTimes[i]->lastTimeSampled.mark(); // remember we started sample here
		}
		// get resp
		//else {
			// TODO: problem or sample at different points (aliasing)? Sometimes will get peak other times not...
			// sample many times. based on fps? 

			// TODO: should weight more recent samples higher! Otherwise don't capture attack!

			// get sample interval
			auto totalWidthMS = cugl::Timestamp::ellapsedMillis(lastTimeSampled, currTime);		
			float tWidth = (float)totalWidthMS / (float)TIME_LENGTH_MS;
			float tLast = t - tWidth;
			float sampleWidth = tWidth / (float)NUM_SAMPLES;

			if (tLast < 0.0f) { //needed?
				tLast = 0.0f;
			}

			float dispMag = 0.0f;

			int totalSamples = 0;
			for (int j = 0; j < NUM_SAMPLES; j++) { // can average, but blurs out features. just take largest value to get attack.
				float tSample = tLast + sampleWidth * j;
				
				float newMag = _respCurves[splineIdx]->get1DPt(tSample);
				/*if (dispMag < newMag) { // get max
					dispMag = newMag;
				}*/

				dispMag += _respCurves[splineIdx]->get1DPt(tSample) * (i + 1); //avg line
				totalSamples += i + 1;
			}
			dispMag /= (float)totalSamples; //NUM_SAMPLES; //avg line

			// no avg
			//dispMag = _respCurves[splineIdx]->get1DPt(t);




			// TODO write in terms of force (t - tLast) * (3 * v(t) + v(tLast)) / 4. use range-kutte or fancier integration method?
			//dispMag = (t - tLast) * (3.0f * _respCurve->get1DPt(t) + _respCurve->get1DPt(tLast)) / 4.0f;

			 
			cugl::Vec2 singleVelocity = dispMag * respDir * speedMultiplier;
			//cugl::Vec2 singleVelocity = (dispMag + speedMultiplier) * respDir;
			velocity += singleVelocity;
		//}
	}

	obj->setLinearVelocity(velocity * _recoilMod);
	//velocity *= _recoilMod * 100.0f;
	//obj->ApplyForceToCenter(b2Vec2(velocity.x, velocity.y), true);
}

void SplinePhysics::reflectResps(PhysicalObject *obj, cugl::Vec2 normalVec) {
	// normalize normal vector (direction vec already normalized)
	cugl::Vec2 normalizedNormal = normalVec.getNormalization();

	//CULog("Angle from normal %s \n", normalizedNormal.toString().c_str());
	// reflect all displacement vectors upon wall bounce ABOUT the other objects normal vector!
	for (int i = 0; i < _respStartTimes.size(); i++) {
		// flip direction so going outwards from the wall
		cugl::Vec2 flipDirection = _respStartTimes[i]->direction * -1.0f;

		//get angle between vectors cost = u . v / (|u| * |v|). |u| = |v| = 1
		//float dotProd = flipDirection.cross(normalizedNormal);
		//float angleBetweenVec = acosf(dotProd);
		//float angleToRotateBy

		//CULog("Normal %s Angle %f \n", flipDirection.toString().c_str(), angleBetweenVec * (180.0f/PI));

		cugl::Quaternion rot = cugl::Quaternion();
		//rot.set(cugl::Vec3(normalizedNormal), -2.0f * angleBetweenVec);
		rot.set(cugl::Vec3(normalizedNormal), PI); // rotate into z-axis by 180 degrees around normal vec
		cugl::Vec3 newRespDir3 = rot.getRotation(cugl::Vec3(flipDirection));
		cugl::Vec2 newRespDir2 = cugl::Vec2(newRespDir3.x, newRespDir3.y);
		_respStartTimes[i]->direction = newRespDir2.getNormalization();
		_respStartTimes[i]->speedMod = _bounceMod;

	}

}

void SplinePhysics::adjustRespSpeed(cugl::Vec2 direction, float speedMult) {
	// maybe just create new response in the same direction as current respsonse (just scaled a certain amount?

	// get speed
	auto resp = std::make_shared<resp_t>();
	resp->direction = direction; // store direction of this shot
	resp->speedMod = SPEED_UP_MULT;
	resp->startTime = cugl::Timestamp();
	resp->startTime.mark(); // store this time
	resp->lastTimeSampled = cugl::Timestamp();
	resp->lastTimeSampled.mark(); // store this time
	resp->splineIdx = SPEED_UP_SPLINE;
	_respStartTimes.push_back(resp);

	// modify speed of all current resps
	/*for (int i = 0; i < _respStartTimes.size(); i++) {
		_respStartTimes[i]->speedMod *= speedMult;
		// increase time response lassts too?
		//_respStartTimes[i]->
	}*/
}

// normalization should be removed from recoil spline
//void Physics::splineMovement(bool isNewResp, cugl::Vec2 direction, PhysicalObject * obj, cugl::Vec2 *dispVec) {
void SplinePhysics::move(void *obj, bool isNewResp, cugl::Vec2 direction, cugl::Vec2 *dispVec) {
	PhysicalObject *physObj = (PhysicalObject*)obj;
	
	if (isNewResp) {
		*dispVec = direction;
		CULog("2nd %f %f\n", direction.x, direction.y);
		recoilSpline(direction, physObj);
	}

	applyResps(physObj);
}

void SplinePhysics::adjustSpeed(void *obj, float speedMult) {
	PhysicalObject *physObj = (PhysicalObject*)obj;
	cugl::Vec2 velocity = physObj->getVelocity();
	cugl::Vec2 direction = velocity.getNormalization();

	adjustRespSpeed(direction, speedMult);
}

/*void Physics::splineMovementFromTouch(bool isTouch, cugl::Vec2 touchLoc, PhysicalObject * obj, cugl::Vec2 *dispVec) {
	if (isTouch) {
		*dispVec = getDirectionFromTouch(touchLoc, obj->getPosition());
		recoilSpline(*dispVec, obj);
	}
	
	applyResps(obj);
}*/

cugl::Vec2 SplinePhysics::getDirectionFromTouch(cugl::Vec2 touchLoc, cugl::Vec2 objPos) {
	cugl::Vec2 dispVec;

	cugl::Vec2 pos = objPos * Application::get()->getDisplayWidth()/32.0f;//_scale;
	pos.y = _physVertShift - pos.y;
	dispVec = pos - touchLoc;
	dispVec.y = -1.0f * dispVec.y;

	dispVec.normalize();

	return dispVec;
}


// make sure that sampling is not frame rate dependent
float SplinePhysics::sampleAverage(std::vector<float> &samples) {
	float sum;
	for (int i = 0; i < samples.size(); i++) {
		sum += samples[i];
	}

	float avg = sum / (float)samples.size();
	return avg;
}

void SplinePhysics::dispose() {
	_respStartTimes.clear();
	_respStartTimes.shrink_to_fit();

}

void SplinePhysics::onCollide(void *obj, cugl::Vec2 normalVec) {
	/*// average normal vecs from collisions this frame
	cugl::Vec2 nVec = cugl::Vec2::ZERO;
	for (int i = 0; i < _frameNormals.size(); i++) {
		nVec += _frameNormals[i];
	}
	nVec /= _frameNormals.size();

	// process reflection
	PhysicalObject *physObj = (PhysicalObject*)obj;
	reflectResps(physObj, nVec);

	_frameNormals.clear();*/

	//_frameNormals.push_back(normalVec);

	PhysicalObject *physObj = (PhysicalObject*)obj;
	reflectResps(physObj, normalVec);

}

bool SplinePhysics::doCollide(void *obj, void *otherPhysObj){
	if (otherPhysObj == nullptr) {
		return true;
	}
	
	PhysicalObject *physObj = (PhysicalObject*)obj;
	PhysicalObject *oPhysObj = (PhysicalObject*)otherPhysObj;

	cugl::Vec2 velocity = physObj->getVelocity();
	float speedMag = velocity.length();

	//auto otherBtype = oPhysObj->getBody(0)->getBodyType()
	if (_isBeserker && (speedMag > KILL_SPEED) && (oPhysObj->getBody(0)->isBullet())) {
		return false;
	}
	else {
		return true;
	}
	
}

//void SplinePhysics::procAllCollisions(void *obj) {
//	// save all surface normals for a particular collision
//	/*int key = getHashKey(idx1, idx2);
//	std::vector<Vec2> currVecs = colNormals[key];
//	colNormals[key].push_back(Vec2(0.0f, 0.0f));
//	CULog("%d %d\n", key, currVecs.size());*/
//	if (_frameNormals.size() == 0) {
//		return;
//	}
//
//	// average normal vecs from collisions this frame
//	cugl::Vec2 nVec = cugl::Vec2::ZERO;
//	for (int i = 0; i < _frameNormals.size(); i++) {
//		nVec += _frameNormals[i];
//	}
//	nVec /= _frameNormals.size();
//
//	// process reflection
//	PhysicalObject *physObj = (PhysicalObject*)obj;
//	reflectResps(physObj, nVec);
//
//	// remove for next frame
//	_frameNormals.clear();
//
//	//_frameNormals.push_back(normalVec);
//}
