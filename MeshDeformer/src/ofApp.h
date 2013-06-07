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
#include "MiddleDifferentLengthScene.h"
#include "GrowingMiddleFingerScene.h"
#include "StartrekScene.h"
#include "StraightenFingersScene.h"
#include "SplayFingersScene.h"
#include "TwitchScene.h"
#include "PinkyPuppeteerScene.h"
#include "FingerLengthPuppeteerScene.h"

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

	vector<Scene*> scenes;
	int curScene, prevScene;

	ofxUICanvas* gui;
	ofxUICanvas** guis;
	ofxUIRadio *sceneRadio;
	
	bool showGuis;
	bool showImage, showWireframe, showSkeleton, mouseControl;
	bool frameBasedAnimation; 

	ofMesh mesh;
	ofImage hand;
	ofxPuppet puppet;
	ThreePointSkeleton threePointSkeleton, immutableThreePointSkeleton;
	PalmSkeleton palmSkeleton, immutablePalmSkeleton;
	WristSpineSkeleton wristSpineSkeleton, immutableWristSpineSkeleton;
	HandSkeleton handSkeleton, immutableHandSkeleton;
	HandWithFingertipsSkeleton handWithFingertipsSkeleton, immutableHandWithFingertipsSkeleton;
	Skeleton* previousSkeleton, *currentSkeleton;
	vector<string> sceneNames, sceneWithSkeletonNames;
};