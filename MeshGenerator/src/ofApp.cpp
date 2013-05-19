#include "ofApp.h"
#include "SharedUtils.h"

void ofApp::setup() {
	sharedSetup();
	setupGui();
	video.load("recording");
	thresholdValue = 30;
	active = true;
	intermediate = false;
	mask.loadImage("mask.png");
	
	minAllowableContourAreaAsAPercentOfImageSize = 0.04;
	maxAllowableContourAreaAsAPercentOfImageSize = 0.12; // approx 100000.0 / (768*1024);
}

void ofApp::setupGui() {
	gui = new ofxUICanvas();
	gui->addLabel("Mesh Generation");
	gui->addSpacer();
	gui->addFPS();
	gui->addSlider("Threshold", 0.0, 255.0, &thresholdValue);
	gui->addSlider("MinContourArea%", 0.0, 0.10, &minAllowableContourAreaAsAPercentOfImageSize);
	gui->addSlider("MaxContourArea%", 0.0, 0.25, &maxAllowableContourAreaAsAPercentOfImageSize);
	gui->addSpacer();
	gui->addLabelToggle("Active", &active);
	gui->addLabelToggle("Intermediate", &intermediate);
	gui->autoSizeToFitWidgets();
}


//==============================================================
void ofApp::update() {
	video.update();
	if(video.isFrameNew()) {
		
		// Fetch the (color) video, and convert to (properly weighted) grayscale.
		Mat videoMat = toCv(video);
		convertColor(videoMat, gray, CV_RGB2GRAY);
		imgH = videoMat.rows;
		imgW = videoMat.cols;
		
		// Threshold the result, and mask against relevant-area quad.
		threshold(gray, thresholded, thresholdValue);
		bitwise_and(mask, thresholded, thresholded);
		
		// Morphological cleanup filtering here. Uses thresholded; puts results in thresholdedCleaned
		doMorphologicalCleanupOnThresholdedVideo();
		
		// Extract the contour(s) of the binarized image
		float minArea = minAllowableContourAreaAsAPercentOfImageSize * (imgW * imgH);
		float maxArea = maxAllowableContourAreaAsAPercentOfImageSize * (imgW * imgH);
		contourFinder.setMinArea ( minArea );
		contourFinder.setMaxArea ( maxArea );
		contourFinder.findContours(thresholded);
		
		// Find the index ID of the largest contour, which is most likely the hand.
		int indexOfHandContour = NO_VALID_HAND;
		float largestContourArea = 0; 
		vector <ofPolyline> polylineContours = contourFinder.getPolylines();
		int nPolylineContours = polylineContours.size();
		for (int i=0; i<nPolylineContours; i++){
			// ofPolyLine p = contourFinder.getPolyline(i);
			float contourArea = contourFinder.getContourArea(i);
			if (contourArea > largestContourArea){
				contourArea = largestContourArea; 
				indexOfHandContour = i; 
			}
		}
		
		 
	}
}

//==============================================================
void ofApp::doMorphologicalCleanupOnThresholdedVideo(){
	
	//cv::Rect myROI(0, 0, imgW, imgH); // y, x, h, w
	//thresholdedCleaned = Mat(thresholded, myROI);
	
	
	for (int i=0; i<1; i++){
		cv::dilate(thresholded, thresholdedCleaned, Mat());
		cv::dilate(thresholdedCleaned, thresholded, Mat());
	}
	// tempGrayscaleMat
	
}



void ofApp::draw() {
	if(active) {
		
		ofPushStyle();
		drawMat(thresholdedCleaned, 0, 0);
		ofSetColor(255,0,0); 
		contourFinder.draw();
		ofPopStyle();
		
	} else {
		
		ofPushStyle();
		ofSetColor(255); 
		video.draw(0, 0);
		ofPopStyle();
	}
	if(intermediate) {
		
		ofPushMatrix();
		float miniScale = 0.2;
		ofTranslate(0, ofGetHeight() - (miniScale * imgH));
		ofScale(miniScale, miniScale);
		
		int xItem = 0;  
		drawMat(gray,        imgW * xItem, 0); xItem++;
		drawMat(thresholded, imgW * xItem, 0); xItem++;
		ofPopMatrix();
	}		
}