#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxUI.h"

#include "PlusOne.h"
#include "MinusONe.h"

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp {
public:
	void setup();
	void setupGui();
	void update();
	void draw();
	void keyPressed(int key);
    
    ofDirectory dir;
    int curFile;
    void nextFile();
    
    ofImage hand;
    ofMesh handMesh;
    void loadMesh(string handFile);
    
    PlusOne plusOne;
    MinusOne minusOne;
    TopologyModifier* modifier;
    
	ofxUICanvas* gui;
    bool showImage, showWireframe;
};