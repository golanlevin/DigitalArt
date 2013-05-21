#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxUI.h"
#include "ofxPuppetInteractive.h"

#include "HandSkeleton.h"
#include "ThreePointSkeleton.h"
#include "HandWithFingertipsSkeleton.h"

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

	enum Scene {
		NONE = 0,
		WAVE, WIGGLE, WOBBLE, 
		EQUALIZE, NORTH, LISSAJOUS,
		MEANDER, PROP_WIGGLE, SIN_LENGTH
	};

	ofxUICanvas* gui;
	ofxUICanvas** guis;
	ofxUIRadio* sceneRadio, *lissajousRadio;
	bool showImage, showWireframe, showSkeleton, mouseControl;
	float equalizeLength;
	float lissajousAmplitude, lissajousFrequency;
	float meanderAmount;
	
	ofMesh mesh;
	ofImage hand;
	ofxPuppet puppet;
	ThreePointSkeleton threePointSkeleton;
	HandSkeleton handSkeleton;
	HandWithFingertipsSkeleton handWithFingertipsSkeleton;
	Skeleton* previousSkeleton, *currentSkeleton;
	vector<string> sceneNames;
};