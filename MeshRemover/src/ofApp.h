#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxUI.h"
#include "ofxPuppetInteractive.h"

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
	bool showImage, showWireframe, showRemoval;
	
	ofMesh mesh;
	ofImage hand;
	ofxPuppetInteractive puppet;
	
	ofPolyline removalRegion;
	vector<pair<ofIndexType, ofIndexType> > stitch;
};