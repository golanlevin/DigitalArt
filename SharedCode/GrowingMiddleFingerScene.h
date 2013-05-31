#pragma once

#include "Scene.h"
#include "HandWithFingertipsSkeleton.h"

class GrowingMiddleFingerScene : public Scene {
protected:
	float maxPalmAngleLeft, maxPalmAngleRight;	
	float maxBaseAngleLeft, maxBaseAngleRight;
	float maxMidAngleLeft, maxMidAngleRight;
	float maxTopAngleLeft, maxTopAngleRight;

	float baseAngleRange;
	float midAngleRange;
	float topAngleRange;
	float maxLen;
	float growthAmount;
	float speedUp;
	float phaseOffset;
	ofxUIToggle* keepWiggling;
public:
	GrowingMiddleFingerScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
