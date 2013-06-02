#pragma once

#include "Scene.h"
#include "ThreePointSkeleton.h"

class LissajousScene : public Scene {
protected:
	float maxPalmAngleLeft, maxPalmAngleRight;	

	float amplitude;
	float frequency;
	vector<string> lissajousStyle;
	ofxUIRadio* lissajousRadio;
public:
	LissajousScene(ofxPuppet* puppet, ThreePointSkeleton* threePointSkeleton, ThreePointSkeleton* immutableThreePointSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
