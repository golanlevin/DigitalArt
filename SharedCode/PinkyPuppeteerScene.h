#pragma once

#include "Scene.h"
#include "HandWithFingertipsSkeleton.h"

class PinkyPuppeteerScene : public Scene {
protected:
	float maxThumbBaseAngleLeft, maxThumbBaseAngleRight;
	float maxThumbMidAngleLeft, maxThumbMidAngleRight;
	float maxThumbTopAngleLeft, maxThumbTopAngleRight;
	float maxPinkyBaseAngle, maxPinkyMidAngle, maxPinkyTopAngle;
	ofxUIToggle* mirror;
public:
	PinkyPuppeteerScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
