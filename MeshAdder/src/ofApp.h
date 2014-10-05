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
    bool showImage, showWireframe, showSplit, showExtra, showSides;
	
	ofImage hand;
    ofMesh handMesh;
	ofxPuppet handPuppet;
    ofxPuppet fingerPuppet;
	
    ofMesh leftBaseMesh, rightBaseMesh;
    
    ofPolyline splitPath;
    ofPolyline extraLeftPath, extraRightPath;
    ofPolyline splitLeftPath, splitRightPath;
	vector<ofIndexType> indices;
	
	ofMesh extraMesh;
};