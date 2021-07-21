
#include "HealthBarAnimation.h"
#include "PhysicalObject.h"

void HealthBarAnimation::updateTexture(anim_args_t *args) {
	PhysicalObject *physObj = (PhysicalObject*)args->obj;


	int healthLost = physObj->getHealthLostThisFrame();
	if (healthLost > 0) {
		// TODO hardcode for now
		std::shared_ptr<AnimationNode> health = args->nodes[0];
		std::shared_ptr<AnimationNode> base = args->nodes[1];

		float healthFraction = healthLost / (float)physObj->getTotalHealth();
		float totalWidth = health->getWidth();// *health->getScaleX();
		//CULog("widths %f %f\n", health->getWidth(), health->getScaleX());


		float shift = healthFraction * totalWidth;
		//CULog("update %f %f -> %f\n", healthFraction, totalWidth, shift);
		health->shiftPolygon(shift / health->getScaleX(), 0.0f);
		//health->setColor(cugl::Color4(0,255,0));
		//health->col
		//health->setRelativeColor(true);
	}
}



void HealthBarAnimation::stopAnim() {
	return;
}