#pragma once

#include "Scene.h"
#include "WristSpineSkeleton.h"

class WigglingWristScene : public Scene {
protected:
	float maxPalmAngleLeft, maxPalmAngleRight;	

	float palmAngleRange;
	float topAngleRange;
	float midAngleRange;
	float speedUp;
	float phaseOffset;
public:
	WigglingWristScene(ofxPuppet* puppet, WristSpineSkeleton* wristSpineSkeleton, WristSpineSkeleton* immutableWristSpineSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
