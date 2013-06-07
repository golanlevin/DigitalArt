#pragma once

#include "ofMain.h"

void sharedSetup() {
	ofSetVerticalSync(true);
	ofSetFrameRate(120);
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetCircleResolution(32);
	
	string basePath = ofToDataPath("", true);
	ofSetDataPathRoot(basePath + "../../../SharedData/");
	//ofSetDataPathRoot("../../../../../SharedData/"); // for Golan's machine. Don't delete, please!
}