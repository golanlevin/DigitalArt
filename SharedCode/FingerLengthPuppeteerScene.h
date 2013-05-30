#pragma once

#include "Scene.h"
#include "HandWithFingertipsSkeleton.h"

class FingerLengthPuppeteerScene : public Scene {
protected:
	float maxThumbBaseAngleLeft, maxThumbBaseAngleRight;
	float maxThumbMidAngleLeft, maxThumbMidAngleRight;
	float maxThumbTopAngleLeft, maxThumbTopAngleRight;
	float maxLengthRatio;
	vector<string> thumbControlJointOptions, angleMeasurementOptions;
	ofxUIRadio *thumbControlJointRadio, *angleMeasurementRadio;

	int prevMouseSelection, prevThumbControlSelection;
public:
	FingerLengthPuppeteerScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
