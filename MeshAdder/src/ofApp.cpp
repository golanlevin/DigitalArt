#include "ofApp.h"
#include "SharedUtils.h"
#include "MeshUtils.h"

void ofApp::setup() {
	sharedSetup();
	setupGui();
	
	showImage = true;
	showWireframe = true;
	showSplit = true;
	
	hand.loadImage("hand/genericHandCentered.jpg");
	mesh.load("hand/handmarks.ply");
	for(int i = 0; i < mesh.getNumVertices(); i++) {
		mesh.addTexCoord(mesh.getVertex(i));
	}
	
	int toSplit[] = {81, 112, 80, 104, 79, 99, 8, 24};
	int toSplitCount = 8;
	vector<ofIndexType> indices;
	for(int i = 0; i < toSplitCount; i++) {
		splitPath.addVertex(mesh.getVertex(toSplit[i]));
		indices.push_back(toSplit[i]);
	}
	
	split(mesh, indices);
	mesh = dropUnusedVertices(mesh);
	
	/*
	int toRemove[] = {
		1, 81, 112, 80,
		104, 79, 99, 8,
		23, 29, 296, 295,
		37, 284, 283, 45,
		272, 271, 53, 259,
		19, 260, 54, 263,
		264, 46, 276, 275,
		38, 287, 288, 24,
		24, 11, 100, 82,
		105, 83, 111, 84};
	int toRemoveCount = 40;
	for(int i = 0; i < toRemoveCount; i++) {
		removalRegion.addVertex(mesh.getVertex(toRemove[i]));
	}
	
	int toStitchLeft[] = {69, 100, 68, 92, 67, 87, 8, 20};
	int toStitchRight[] = {72, 99, 71, 93, 70, 88, 9, 21};
	int toStichCount = 8;
	vector<pair<ofIndexType, ofIndexType> > stitch;
	for(int i = 0; i < toStichCount; i++) {
		stitch.push_back(pair<ofIndexType, ofIndexType>(toStitchLeft[i], toStitchRight[i]));
	}
	
	mesh = removeAndStitch(mesh, removalRegion, stitch);
	*/
	puppet.setup(mesh);
}

void ofApp::setupGui() {	
	gui = new ofxUICanvas();
	gui->addLabel("Mesh Deformer");
	gui->addSpacer();
	gui->addFPS();
	gui->addSpacer();
	gui->addLabelToggle("Show Image", &showImage);
	gui->addLabelToggle("Show Wireframe", &showWireframe);
	gui->addLabelToggle("Show Removal", &showSplit);
	gui->autoSizeToFitWidgets();
}

void ofApp::update() {
	puppet.update();
}

void ofApp::draw() {
	ofBackground(0);
	if (showImage) {
		hand.getTextureReference().bind();
		puppet.drawFaces();
		hand.getTextureReference().unbind();
	}	
	if(showWireframe) {
		puppet.drawWireframe();
		puppet.drawControlPoints();
	}
	if(showSplit) {
		ofPushStyle();
		ofSetLineWidth(3);
		ofSetColor(ofColor::red);
		splitPath.draw();
		ofPopStyle();
	}
}

void ofApp::keyPressed(int key) {
	if(key == ' ') {
		int vertexIndex;
		ofVec2f target(mouseX, mouseY);
		float bestDistance = 0;
		for(int i = 0; i < mesh.getNumVertices(); i++) {
			float distance = target.distance(mesh.getVertex(i));
			if(distance < bestDistance || i == 0) {
				bestDistance = distance;
				vertexIndex = i;
			}
		}
		cout << vertexIndex << ", ";
	}
}
