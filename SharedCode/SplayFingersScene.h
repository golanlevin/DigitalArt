#pragma once

#include "Scene.h"
#include "HandWithFingertipsSkeleton.h"

class SplayFingersScene : public Scene {
protected:
	float splayHeight;
	float splayAxis;
	float maxAngle;
public:
	SplayFingersScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
