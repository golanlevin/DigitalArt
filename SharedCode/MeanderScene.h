#pragma once

#include "Scene.h"
#include "HandSkeleton.h"

class MeanderScene : public Scene {
protected:
	float meanderAmount;
public:
	MeanderScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
