#pragma once

#include "Scene.h"
#include "HandWithFingertipsSkeleton.h"

class RetractingFingersScene : public Scene {
protected:
	float retractHeight;
	float tipRatio;
	float topRatio;
	float middleRatio;
	float bottomRatio;
public:
	RetractingFingersScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
