#include "ofApp.h"
#include "SharedUtils.h"
#include "MeshUtils.h"

void ofApp::setup() {
	sharedSetup();
	setupGui();
	
	showImage = true;
	showWireframe = true;
	showRemoval = true;
	
	hand.loadImage("hand/genericHandCenteredNew.jpg");
	mesh.load("hand/handmarksNew.ply");
	for(int i = 0; i < mesh.getNumVertices(); i++) {
		mesh.addTexCoord(mesh.getVertex(i));
	}
	
    // step 1: build these points
	removalRegion.close();
	int toRemove[] = {176, 290, 374, 373, 372, 371, 370, 369, 293, 363, 364, 365, 366, 367, 368, 289, 119, 177, 182, 187, 192, 197, 202, 207, 212, 217, 222, 227, 228, 229, 226, 221, 216, 211, 206, 201, 196, 191, 186, 181
	};
	int toRemoveCount = 40;
	for(int i = 0; i < toRemoveCount; i++) {
		removalRegion.addVertex(mesh.getVertex(toRemove[i]));
    }
    
    // step 2: remove the triangles for the remaining indices
//    removeTriangles(mesh, removalRegion);
//    mesh = dropUnusedVertices(mesh);
	
	// post-removal indices, not original indices
    int toStitchLeft[] = {173, 234, 318, 317, 316, 315, 314, 313};
    int toStitchRight[] = {119, 233, 312, 311, 310, 309, 308, 307};
	int toStitchCount = 8;
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
