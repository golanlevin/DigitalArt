#include "ofApp.h"
#include "SharedUtils.h"

enum Label {
	WRIST = 0,
	PALM,
	PINKY_BASE, PINKY_MID, PINKY_TIP,
	RING_BASE, RING_MID, RING_TIP,
	MIDDLE_BASE, MIDDLE_MID, MIDDLE_TIP,
	INDEX_BASE, INDEX_MID, INDEX_TIP,
	THUMB_BASE, THUMB_MID, THUMB_TIP
};

void ofApp::setup() {
	sharedSetup();
	setupGui();
	
	showWireframe = false;
	
	hand.loadImage("hand/genericHandCentered.jpg");
	mesh.load("hand/handmarks.ply");
	for(int i = 0; i < mesh.getNumVertices(); i++) {
		mesh.addTexCoord(mesh.getVertex(i));
	}
	puppet.setup(mesh);
	
	for(int i = 0; i < 17; i++) {
		puppet.setControlPoint(i);
	}
}

void ofApp::setupGui() {
	gui = new ofxUICanvas();
	gui->addLabel("Mesh Deformer");
	gui->addSpacer();
	gui->addFPS();
	gui->addSpacer();
	gui->addLabelToggle("Show Wireframe", &showWireframe);
	gui->autoSizeToFitWidgets();
}

void ofApp::update() {
	Label toWiggle[] = {PINKY_TIP, RING_TIP, MIDDLE_TIP, INDEX_TIP, THUMB_TIP};
	int toWiggleCount = 5;
	float wiggleRange = 10;
	float t = ofGetElapsedTimef();
	for(int i = 0; i < toWiggleCount; i++) {
		int index = toWiggle[i];
		ofVec2f original = puppet.getOriginalMesh().getVertex(index);
		puppet.setControlPoint(index, original + wiggleRange * ofVec2f(ofNoise(i, t, 0), ofNoise(i, t, 1)));
	}
	puppet.update();
}

void ofApp::draw() {
	ofBackground(0);
	
	hand.getTextureReference().bind();
	puppet.drawFaces();
	hand.getTextureReference().unbind();
	
	if(showWireframe) {
		puppet.drawWireframe();
		puppet.drawControlPoints();
	}
}

void ofApp::keyPressed(int key) {
}