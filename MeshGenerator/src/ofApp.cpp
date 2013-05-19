#include "ofApp.h"
#include "SharedUtils.h"

void ofApp::setup() {
	sharedSetup();
	setupGui();
	video.load("recording");
	thresholdValue = 197;
	active = true;
	intermediate = false;
	mask.loadImage("mask.png");
}

void ofApp::setupGui() {
	gui = new ofxUICanvas();
	gui->addLabel("Mesh Generation");
	gui->addSpacer();
	gui->addFPS();
	gui->addSlider("Threshold", 0.0, 255.0, &thresholdValue);
	gui->addSpacer();
	gui->addLabelToggle("Active", &active);
	gui->addLabelToggle("Intermediate", &intermediate);
	gui->autoSizeToFitWidgets();
}

void ofApp::update() {
	video.update();
	if(video.isFrameNew()) {
		Mat videoMat = toCv(video);
		convertColor(videoMat, gray, CV_RGB2GRAY);
		equalizeHist(gray, equalized);
		threshold(equalized, thresholded, thresholdValue);
		bitwise_and(mask, thresholded, thresholded);
		contourFinder.findContours(thresholded);
	}
}

void ofApp::draw() {
	if(active) {
		drawMat(thresholded, 0, 0);
		contourFinder.draw();
	} else {
		video.draw(0, 0);
	}
	if(intermediate) {
		ofPushMatrix();
		ofTranslate(0, ofGetHeight() - 256);
		ofScale(.25, .25);
		drawMat(gray, 768 * 0, 0);
		drawMat(equalized, 768 * 1, 0);
		drawMat(thresholded, 768 * 2, 0);
		ofPopMatrix();
	}		
}