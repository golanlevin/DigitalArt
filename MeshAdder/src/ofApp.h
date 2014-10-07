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
    
    void loadMesh(string handFile);
	
    ofDirectory dir;
    int curFile;
    void nextFile();
    
    PlusOne plusOne;
    
	ofxUICanvas* gui;
    bool showImage, showWireframe;
	
	ofImage hand;
    ofMesh handMesh;
};