#pragma once

#include "Scene.h"
#include "HandSkeleton.h"

class WaveScene : public Scene {
protected:
	float maxPalmAngleLeft, maxPalmAngleRight;	
public:
	WaveScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
