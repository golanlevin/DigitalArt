#pragma once

#include "Scene.h"
#include "HandWithFingertipsSkeleton.h"

class StraightenFingersScene : public Scene {
public:
	StraightenFingersScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
