#include "ofApp.h"
#include "SharedUtils.h"
#include "MeshUtils.h"

void ofApp::setup() {
	sharedSetup();
	setupGui();
	
	showImage = true;
	showWireframe = true;
	showRemoval = true;
	
	hand.loadImage("hand/genericHandCentered.jpg");
	mesh.load("hand/handmarks.ply");
	for(int i = 0; i < mesh.getNumVertices(); i++) {
		mesh.addTexCoord(mesh.getVertex(i));
	}
	
	removalRegion.close();
	int toRemove[] = {
		293, 363, 364, 365,
		366, 367, 368, 289,
		176, 181, 186, 191,
		196, 201, 206, 211,
		216, 221, 226, 229,
		228, 227, 222, 217,
		212, 207, 202, 197,
		192, 187, 182, 177,
		119, 288, 362, 361,
		360, 359, 358, 357
	};
	int toRemoveCount = 40;
	for(int i = 0; i < toRemoveCount; i++) {
		removalRegion.addVertex(mesh.getVertex(toRemove[i]));
	}
	
	// post-removal indices, not original indices
	int toStitchLeft[] = {307, 308, 309, 310, 311, 312, 233, 173};
	int toStitchRight[] = {301, 302, 303, 304, 305, 306, 232, 119,};
	int toStitchCount = 8;
	for(int i = 0; i < toStitchCount; i++) {
		stitch.push_back(pair<ofIndexType, ofIndexType>(toStitchLeft[i], toStitchRight[i]));
	}
	
	mesh = removeAndStitch(mesh, removalRegion, stitch);
	
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
	gui->addLabelToggle("Show Removal", &showRemoval);
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
	if(showRemoval) {
		ofPushStyle();
		ofSetLineWidth(3);
		ofSetColor(ofColor::red);
		removalRegion.draw();
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
