#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxUI.h"
#include "ofxPuppetInteractive.h"

#include "HandSkeleton.h"
#include "ThreePointSkeleton.h"

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp {
public:
	void setup();
	void setupGui();
	void update();
	void draw();
	void keyPressed(int key);
	
	ofxUICanvas* gui;
	ofxUIRadio* sceneRadio;
	bool showImage, showWireframe, showSkeleton, mouseControl;
	float equalizeLength;
	
	ofMesh mesh;
	ofImage hand;
	ofxPuppet puppet;
	ThreePointSkeleton threePointSkeleton;
	HandSkeleton handSkeleton;
	Skeleton* currentSkeleton;
	vector<string> sceneNames;
};