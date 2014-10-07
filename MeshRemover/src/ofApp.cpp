#include "ofApp.h"
#include "SharedUtils.h"
#include "MeshUtils.h"

float scaleFactor = 2;

void ofApp::setup() {
	sharedSetup();
	setupGui();
	
	showImage = true;
	showWireframe = true;
	showRemoval = true;
	
    string handFile = "handmesh-57";
    hand.loadImage("hand/"+handFile+".jpg");
    handMesh.load("hand/"+handFile+".ply");
    
//    handMesh.clearTexCoords();
//    for(int i = 0; i < handMesh.getNumVertices(); i++) {
//        handMesh.addTexCoord(ofVec2f(handMesh.getVertex(i)));
//    }
	
    // build these points
	removalRegion.close();
	int toRemove[] = {115, 120, 125, 130, 135, 142, 65, 68, 71, 74, 77, 80, 83, 82, 81, 78, 75, 72, 69, 66, 63, 140, 134, 129, 124, 119
	};
	int toRemoveCount = 26;
	for(int i = 0; i < toRemoveCount; i++) {
		removalRegion.addVertex(handMesh.getVertex(toRemove[i]));
    }
    
    // make a copy of the removal region, to be used for blending
    int baseRegionCount = 14;
    int baseRegionIndices[] = {
        115, 119, 124, 129, 134, 140, 63, 64, 65, 142, 135, 130, 125, 120
    };
    ofPolyline baseRegion = buildPolyline(handMesh, baseRegionIndices, baseRegionCount);
    removedMesh = copySubmesh(handMesh, removalRegion);
    
    // remove the triangles for the remaining indices
    removeTriangles(handMesh, removalRegion);
    handMesh = dropUnusedVertices(handMesh);

	// post-removal indices, not original indices
    int toStitchLeft[] = {99, 104, 109, 114, 120};
    int toStitchRight[] = {98, 103, 108, 113, 119};
    int toStitchCount = 5;
	for(int i = 0; i < toStitchCount; i++) {
		stitchIndices.push_back(pair<ofIndexType, ofIndexType>(toStitchLeft[i], toStitchRight[i]));
	}

    // stitch sides together
	handMesh = stitch(handMesh, stitchIndices);
    
    float opacity[] = {
        0,
        0,0,0,0,0,0,0,0,0,0,0,0,
        .5,
        1,1,1,1,1,1,1,1,1,1,1,1};
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
}

void ofApp::draw() {
    ofPushMatrix();
    ofBackground(0);
    ofScale(scaleFactor, scaleFactor);
	if (showImage) {
		hand.getTextureReference().bind();
		handMesh.drawFaces();
		hand.getTextureReference().unbind();
	}	
	if(showWireframe) {
//		handMesh.drawWireframe();
        removedMesh.drawWireframe();
	}
	if(showRemoval) {
		ofPushStyle();
		ofSetLineWidth(3);
		ofSetColor(ofColor::red);
		removalRegion.draw();
		ofPopStyle();
	}
    ofPopMatrix();
}

void ofApp::keyPressed(int key) {
	if(key == ' ') {
        ofMesh& mesh = handMesh;
		int vertexIndex;
        ofVec2f target(mouseX, mouseY);
        target /= scaleFactor;
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
