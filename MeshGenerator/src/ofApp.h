#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxUI.h"
#include "BufferedVideo.h"

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp {
public:
	void setup();
	void setupGui();
	void update();
	void draw();
	
	ofxUICanvas* gui;
	BufferedVideo video;
	float thresholdValue;
	bool active, intermediate;
	ofImage mask;
	Mat gray, equalized, thresholded;
	ContourFinder contourFinder;
};