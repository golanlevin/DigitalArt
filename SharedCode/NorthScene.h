#pragma once

#include "Scene.h"
#include "HandSkeleton.h"

class NorthScene : public Scene {
public:
	NorthScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
