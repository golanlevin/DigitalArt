#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxUI.h"
#include "ofxPuppetInteractive.h"
#include "PlusOne.h"

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp {
public:
	void setup();
	void setupGui();
	void update();
	void draw();
	void keyPressed(int key);
    
    PlusOne plusOne;
    
	ofxUICanvas* gui;
	bool showImage, showWireframe, showRemoval;
	
    ofMesh handMesh, removedMesh;
	ofImage hand;
	
	ofPolyline removalRegion;
	vector<pair<ofIndexType, ofIndexType> > stitchIndices;
};