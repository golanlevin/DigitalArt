#pragma once

#include "Scene.h"
#include "PalmSkeleton.h"

class PulsatingPalmScene : public Scene {
protected:
	float maxLength;
	float speedUp;
public:
	PulsatingPalmScene(ofxPuppet* puppet, PalmSkeleton* palmSkeleton, PalmSkeleton* immutablePalmSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
