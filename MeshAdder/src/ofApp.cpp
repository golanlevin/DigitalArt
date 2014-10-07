#include "ofApp.h"

#include "SharedUtils.h"
#include "MeshUtils.h"

float scaleFactor = 2;

void ofApp::setup() {
	sharedSetup();
	setupGui();
	
	showImage = true;
	showWireframe = true;
    
    dir.allowExt("ply");
    dir.listDir("hand");
    curFile = 0;
    nextFile();
}

void ofApp::nextFile() {
    string curName = dir.getFile(curFile).getBaseName();
    loadMesh(curName);
    curFile = (curFile + 1) % dir.size();
}

void ofApp::loadMesh(string handFile) {
    hand.loadImage("hand/"+handFile+".jpg");
	handMesh.load("hand/"+handFile+".ply");
    
    plusOne.update(handMesh);
}

void ofApp::setupGui() {	
	gui = new ofxUICanvas();
	gui->addLabel("Mesh Adder");
	gui->addSpacer();
	gui->addLabelToggle("Show Image", &showImage);
	gui->addLabelToggle("Show Wireframe", &showWireframe);
	gui->autoSizeToFitWidgets();
}

void ofApp::update() {
}

void ofApp::draw() {
    ofPushMatrix();
    ofBackground(0);
    ofScale(scaleFactor, scaleFactor);
    if (showImage) {
        plusOne.draw(hand.getTextureReference());
	}	
    if(showWireframe) {
        plusOne.getModifiedMesh().drawWireframe();
    }
    ofPopMatrix();
}

void ofApp::keyPressed(int key) {
    if(key == '\t') {
        nextFile();
    }
}
