#include "testApp.h"

void testApp::setup() {
	ofSetVerticalSync(true);
	
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// If you want to set any non-default parameters like size, format7, blocking
	// capture, etc., you can do it here before setup. They'll be applied to the
	// camera during setup().
	//camera.setFormat7(true);
	camera.setSize(1024, 768);
	camera.setImageType(OF_IMAGE_COLOR);
	camera.setBayerMode(DC1394_COLOR_FILTER_GRBG);
	camera.setBlocking(true);
	//camera.setFrameRate(30);
	
	// setup() will pick the first camera. Or if there are multiple cameras you
	// can use the number, or the GUID of the camera.
	camera.setup();
	
	// After setup it's still possible to change a lot of parameters. If you want
	// to change a pre-setup parameter, the camera will auto-restart
	camera.setBrightness(0);
	camera.setGain(0);
	camera.setExposure(1);
	camera.setGammaAbs(1);
	camera.setShutterAbs(1. / 30.);
	
	// Set the window size to the camera size.
	ofSetWindowShape(camera.getWidth(), camera.getHeight());
	
	recording = false;
	currentFrame = 0;
	imageSequence.resize(300);
	for(int i = 0; i < imageSequence.size(); i++) {
		imageSequence[i].allocate(camera.getWidth(), camera.getHeight(), OF_IMAGE_COLOR);
	}
}

void testApp::update() {
	// grabVideo() will place the most recent frame into curFrame. If it's a new
	// frame, grabFrame returns true. If there are multiple frames available, it
	// will drop old frames and only give you the newest. This guarantees the
	// lowest latency. If you prefer to not drop frames, set the second argument
	// (dropFrames) to false. By default, capture is non-blocking.
	if(camera.grabVideo(curFrame)) {
		curFrame.update();
		if(recording) {
			if(currentFrame < imageSequence.size()) {
				ofPixels& target = imageSequence[currentFrame];
				memcpy(target.getPixels(), curFrame.getPixels(), target.getWidth() * target.getHeight() * target.getNumChannels());
				currentFrame++;
			} else {
				recording = false;
				for(int i = 0; i < imageSequence.size(); i++) {
					imageSequence[i].rotate90(1);
					ofSaveImage(imageSequence[i], ofToString(i, 3, '0') + ".jpg");
				}
			}
		}
	}
}

void testApp::draw() {
	// If the camera isn't ready, the curFrame will be empty.
	if(camera.isReady()) {
		// Camera doesn't draw itself, curFrame does.
		curFrame.draw(0, 0);
		ofDrawBitmapString(ofToString((int) ofGetFrameRate()) + " fps " + (recording ? "recording" : "") , 10, 20);
	}
}

void testApp::keyPressed(int key) {
	if(key == 'r') {
		recording = true;
		currentFrame = 0;
	}
	if(key == 'i') {
		curFrame.saveImage(ofToString(ofGetFrameNum()) + ".png");
	}
}