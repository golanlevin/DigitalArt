#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxUI.h"
#include "BufferedVideo.h"

#include "HandContourAnalyzerAndMeshBuilder.h"

using namespace ofxCv;
using namespace cv;

#define NO_VALID_HAND -1

class ofApp : public ofBaseApp {
public:
	void setup();
	void setupGui();
	void update();
	void draw();
	void keyPressed(int key);
	
	ofxUICanvas* gui;
	BufferedVideo video;
	float thresholdValue;
	bool active, intermediate, playing;
	ofImage mask;
	Mat gray, equalized, thresholded;
	
	void doMorphologicalCleanupOnThresholdedVideo();
	Mat thresholdedCleaned;  // the thresholded input, after morphological filtering.
	
	Mat tempGrayscaleMat;
	bool bDoMorphologicalCleanup; 
	bool bHandyBool; 

	int imgW; // width of our images for computer vision
	int imgH; // height of our images
	
	
	
	ContourFinder contourFinder;
	float minAllowableContourAreaAsAPercentOfImageSize;
	float maxAllowableContourAreaAsAPercentOfImageSize;
	
	bool		bValidHandContourExists;
	ofPolyline	handContourPolyline;
	cv::Point2f	handContourCentroid; 
	HandContourAnalyzerAndMeshBuilder HCAAMB;
	
};