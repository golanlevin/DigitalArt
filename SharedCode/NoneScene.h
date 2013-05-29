#pragma once

#include "Scene.h"
#include "HandSkeleton.h"

class NoneScene : public Scene {
public:
	NoneScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
