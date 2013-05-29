#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxUI.h"
#include "ofxPuppetInteractive.h"

#include "NoneScene.h"
#include "WaveScene.h"
#include "WiggleScene.h"
#include "WobbleScene.h"
#include "EqualizeScene.h"
#include "NorthScene.h"
#include "LissajousScene.h"
#include "MeanderScene.h"
#include "PropogatingWiggleScene.h"
#include "SinusoidalLengthScene.h"
#include "PulsatingPalmScene.h"
#include "RetractingFingersScene.h"
#include "SinusoidalWiggleScene.h"
#include "WigglingWristScene.h"
#include "MiddleDifferentLengthScene.h"
#include "GrowingMiddleFingerScene.h"
#include "StartrekScene.h"
#include "StraightenFingersScene.h"
#include "SplayFingersScene.h"

#include "HandSkeleton.h"
#include "ThreePointSkeleton.h"
#include "HandWithFingertipsSkeleton.h"
#include "PalmSkeleton.h"
#include "WristSpineSkeleton.h"

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp {
public:
	void setup();
	void setupGui();
	void update();
	void draw();
	void keyPressed(int key);
	
	void setSkeleton(Skeleton* skeleton);

	enum SceneIndex {
		NONE = 0,
		WAVE, WIGGLE, WOBBLE, 
		EQUALIZE, NORTH, LISSAJOUS,
		MEANDER, PROP_WIGGLE, SIN_LENGTH,
		PULSE_PALM, RETRACTION, SIN_WIGGLE,
		WRIST_WIGGLE, MIDDLE_LEN, GROWING_MIDDLE,
		STARTREK, STRAIGHTEN, SPLAY
	};

	vector<Scene*> scenes;

	ofxUICanvas* gui;
	ofxUICanvas** guis;//, mouseGuis;
	ofxUIRadio *sceneRadio;//, *lissajousRadio, *sinusoidalWiggleRadio;
	//ofxUIRadio** mouseRadios;
	bool showImage, showWireframe, showSkeleton, mouseControl;
	//float equalizeLength;
	//float lissajousAmplitude, lissajousFrequency;
	//float meanderAmount;
	//float propWiggleBaseAngleRange, propWiggleMidAngleRange, propWiggleTopAngleRange, propWiggleSpeedUp, propWigglePhaseOffset;
	//float sinLength, sinLengthPhaseOffset;
	//float pulseLength;
	//float retractHeight, tipRatio, topRatio, middleRatio, bottomRatio;
	//float sinWiggleAngleRange, sinWiggleSpeedUp, sinWigglePhaseOffset;
	//float wristWigglePalmAngleRange, wristWiggleTopAngleRange, wristWiggleMidAngleRange, wristWiggleSpeedUp, wristWigglePhaseOffset;
	//float middleLength, otherLength;
	//float growingMiddleBaseAngleRange, growingMiddleMidAngleRange, growingMiddleTopAngleRange, growingMiddleLength, growingMiddleGrowthAmount, growingMiddleSpeedUp, growingMiddlePhaseOffset;
	//float splayHeight, splayAxis, splayMaxAngle;
	ofMesh mesh;
	ofImage hand;
	ofxPuppet puppet;
	ThreePointSkeleton threePointSkeleton, immutableThreePointSkeleton;
	PalmSkeleton palmSkeleton, immutablePalmSkeleton;
	WristSpineSkeleton wristSpineSkeleton, immutableWristSpineSkeleton;
	HandSkeleton handSkeleton, immutableHandSkeleton;
	HandWithFingertipsSkeleton handWithFingertipsSkeleton, immutableHandWithFingertipsSkeleton;
	Skeleton* previousSkeleton, *currentSkeleton;
	vector<string> sceneNames;
};