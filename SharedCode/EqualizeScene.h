#pragma once

#include "Scene.h"
#include "HandSkeleton.h"

class EqualizeScene : public Scene {
protected:
	float maxPalmAngleLeft, maxPalmAngleRight;	 

	float equalizeLength;
public:
	EqualizeScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
