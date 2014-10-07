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
    
    void loadMesh(string handFile);
	
    ofDirectory dir;
    int curFile;
    void nextFile();
    
	ofxUICanvas* gui;
    bool showImage, showWireframe, showSplit, showExtra, showSides, showIndices;
	
	ofImage hand;
    ofMesh handMesh;
    ofPolyline extraRegion;
	ofxPuppet handPuppet;
    ofxPuppet fingerPuppet;
	
    ofMesh leftBaseMesh, rightBaseMesh;
    
    ofPolyline splitPath;
    ofPolyline extraLeftPath, extraRightPath;
    ofPolyline splitLeftPath, splitRightPath;
	
	ofMesh extraMesh;
};