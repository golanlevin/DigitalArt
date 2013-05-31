#pragma once

#include "Scene.h"
#include "HandSkeleton.h"

class NoneScene : public Scene {
protected:
	float maxPalmAngleLeft, maxPalmAngleRight;	
public:
	NoneScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
