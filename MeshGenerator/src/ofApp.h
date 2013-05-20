#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
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
	float prevThresholdValue;
	float blurKernelSize;
	float blurredStrengthWeight;
	
	bool active, intermediate, playing;
	ofImage mask;
	
	Mat gray;				// grayscale version of hand input
	Mat thresholded;		// binarized hand, black-white only
	Mat graySmall;
	Mat blurredSmall;
	Mat blurred;
	Mat thresholdConstMat; 
	Mat adaptiveThreshImg;	// blurred minus Constant; the per-pixel thresholds. 
	
	Mat tempGrayscaleMat;
	ofxCvGrayscaleImage tempGrayscaleImg;
	
	void doMorphologicalCleanupOnThresholdedVideo();
	Mat thresholdedCleaned;  // the thresholded input, after morphological filtering.
	
	
	bool bDoAdaptiveThresholding;
	bool bDoMorphologicalCleanup;
	bool bDoMorphologicalOpening; 
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