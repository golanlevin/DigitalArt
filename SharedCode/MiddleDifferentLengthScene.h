#pragma once

#include "Scene.h"
#include "HandSkeleton.h"

class MiddleDifferentLengthScene : public Scene {
protected:
	float maxPalmAngleLeft, maxPalmAngleRight;	
	float maxBaseAngleLeft, maxBaseAngleRight;
	float maxMidAngleLeft, maxMidAngleRight;

	float middleLength;
	float otherLength;
public:
	MiddleDifferentLengthScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
