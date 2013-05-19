#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxUI.h"
#include "BufferedVideo.h"

using namespace ofxCv;
using namespace cv;

#define NO_VALID_HAND -1

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
	
	void doMorphologicalCleanupOnThresholdedVideo();
	Mat thresholdedCleaned;  // the thresholded input, after morphological filtering.
	Mat tempGrayscaleMat; 

	int imgW; // width of our images for computer vision
	int imgH; // height of our images
	
	
	
	ContourFinder contourFinder;
	float minAllowableContourAreaAsAPercentOfImageSize;
	float maxAllowableContourAreaAsAPercentOfImageSize;
};