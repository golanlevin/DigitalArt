#pragma once

#include "Scene.h"
#include "HandSkeleton.h"

class StartrekScene : public Scene {
public:
	StartrekScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
