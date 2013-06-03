#pragma once

#include "Scene.h"
#include "HandSkeleton.h"

class SinusoidalLengthScene : public Scene {
protected:
	float maxPalmAngleLeft, maxPalmAngleRight;	
	float maxBaseAngleLeft, maxBaseAngleRight;
	float maxMidAngleLeft, maxMidAngleRight;

	float maxLength;
	float speedUp;
	float phaseOffset;
	float sigmoidStrengthMid;
	float sigmoidStrengthTip;
public:
	SinusoidalLengthScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton);
	void setupGui();
	void setupMouseGui();
	void update();
	void updateMouse(float mx, float my);
	void draw();
	
	float function_NormalizedLogisticSigmoid (float x, float a);
	float function_DoubleExponentialSigmoid (float x, float a);
};
