#pragma once

#include "Scene.h"
#include "HandSkeleton.h"

class WiggleScene : public Scene {
protected:
	float maxPalmAngleLeft, maxPalmAngleRight;	
public:
	WiggleScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
