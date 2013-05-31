#pragma once

#include "Scene.h"
#include "HandSkeleton.h"

class TwitchScene : public Scene {
protected:
	float maxPalmAngleLeft, maxPalmAngleRight;	
	float maxBaseAngleLeft, maxBaseAngleRight;
	float maxMidAngleLeft, maxMidAngleRight;

	float twitchSigmoidStrength;
	float choiceSigmoidStrength;
	float speedUp;
	float minAngle;
	ofxUIToggle *pinky, *ring, *middle, *index, *thumb; 
public:
	TwitchScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
};
