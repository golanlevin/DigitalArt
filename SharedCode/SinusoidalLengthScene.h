#pragma once

#include "Scene.h"
#include "HandSkeleton.h"

class SinusoidalLengthScene : public Scene {
protected:
	float maxPalmAngleLeft, maxPalmAngleRight;	

	float maxLength;
	float speedUp;
	float phaseOffset;
public:
	SinusoidalLengthScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
