#pragma once

#include "Scene.h"
#include "HandSkeleton.h"

class WobbleScene : public Scene {
protected:
	float maxPalmAngleLeft, maxPalmAngleRight;	
public:
	WobbleScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
