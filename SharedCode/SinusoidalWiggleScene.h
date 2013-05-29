#pragma once

#include "Scene.h"
#include "HandWithFingertipsSkeleton.h"

class SinusoidalWiggleScene : public Scene {
protected:
	float angleRange;
	float speedUp;
	float phaseOffset;
	vector<string> wiggleJoints;
	ofxUIRadio* jointsRadio;
public:
	SinusoidalWiggleScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
