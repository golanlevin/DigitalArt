#include "ofApp.h"
#include "SharedUtils.h"
#include "MeshUtils.h"

void ofApp::setup() {
	sharedSetup();
	setupGui();
	
	showImage = true;
	showWireframe = true;
	showRemoval = true;
	
	hand.loadImage("hand/hand.png");
	mesh.load("hand/hand.ply");
	for(int i = 0; i < mesh.getNumVertices(); i++) {
		mesh.addTexCoord(mesh.getVertex(i));
	}
	
    // step 1: build these points
	removalRegion.close();
	int toRemove[] = {119, 126, 143, 128, 148, 110, 149, 129, 144, 127, 120, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30
	};
	int toRemoveCount = 26;
	for(int i = 0; i < toRemoveCount; i++) {
		removalRegion.addVertex(mesh.getVertex(toRemove[i]));
    }
    
    // step 2: remove the triangles for the remaining indices
//    removeTriangles(mesh, removalRegion);
//    mesh = dropUnusedVertices(mesh);

	// post-removal indices, not original indices
    int toStitchLeft[] = {97, 104, 121, 106, 126};
    int toStitchRight[] = {98, 105, 122, 107, 127};
    int toStitchCount = 5;
	for(int i = 0; i < toStitchCount; i++) {
		stitch.push_back(pair<ofIndexType, ofIndexType>(toStitchLeft[i], toStitchRight[i]));
	}
    
    // step 3: remove and stitch
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
