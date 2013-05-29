#pragma once

#include "Scene.h"
#include "HandWithFingertipsSkeleton.h"

class GrowingMiddleFingerScene : public Scene {
protected:
	float baseAngleRange;
	float midAngleRange;
	float topAngleRange;
	float maxLen;
	float growthAmount;
	float speedUp;
	float phaseOffset;
public:
	GrowingMiddleFingerScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
