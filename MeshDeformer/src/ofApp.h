#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxUI.h"
#include "ofxPuppetInteractive.h"
#include "Skeleton.h"

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
	bool showImage, showWireframe, showSkeleton;
	
	ofMesh mesh;
	ofImage hand;
	ofxPuppetInteractive puppet;
	Skeleton skeleton;
	vector<string> sceneNames;
};