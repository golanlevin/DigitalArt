#pragma once

#include "Scene.h"
#include "HandWithFingertipsSkeleton.h"

class PropogatingWiggleScene : public Scene {
protected:
	float maxPalmAngleLeft, maxPalmAngleRight;	

	float baseAngleRange;
	float midAngleRange;
	float topAngleRange;
	float speedUp;
	float phaseOffset;
public:
	PropogatingWiggleScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
