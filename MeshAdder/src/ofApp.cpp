#include "ofApp.h"
#include "SharedUtils.h"
#include "MeshUtils.h"

void ofApp::setup() {
	sharedSetup();
	setupGui();
	
	showImage = true;
	showWireframe = true;
	showSplit = true;
	showCopy = true;
	
	hand.loadImage("hand/genericHandCentered.jpg");
	mesh.load("hand/handmarks.ply");
	for(int i = 0; i < mesh.getNumVertices(); i++) {
		mesh.addTexCoord(mesh.getVertex(i));
	}
	
	int toSplit[] = {81, 112, 80, 104, 79, 99, 8, 24};
	int toSplitCount = 8;
	for(int i = 0; i < toSplitCount; i++) {
		splitPath.addVertex(mesh.getVertex(toSplit[i]));
		indices.push_back(toSplit[i]);
	}
	
	split(mesh, indices);
	
	int toCopy[] = {
		1, 78, 113, 77, 103, 76, 98, 5, 23, 29, 296, 295, 37, 284, 283, 45, 272, 271, 53, 259, 19, 260, 54, 263, 264, 46, 276, 275, 38, 287, 288, 30, 24,	8, 99, 79, 104, 80, 112, 81,
	};
	int toCopyCount = 40;
	for(int i = 0; i < toCopyCount; i++) {
		copyRegion.addVertex(mesh.getVertex(toCopy[i]));
	}
	copyRegion.close();
	
	copyMesh.setMode(OF_PRIMITIVE_TRIANGLES);
	for(int i = 0; i < mesh.getNumIndices(); i += 3) {
		int i0 = mesh.getIndex(i + 0), i1 = mesh.getIndex(i + 1), i2 = mesh.getIndex(i + 2);
		ofVec2f vi0 = mesh.getVertex(i0), vi1 = mesh.getVertex(i1), vi2 = mesh.getVertex(i2);
		ofVec2f avg = (vi0 + vi1 + vi2) / 3;
		if(copyRegion.inside(avg)) {
			// this needs to be adding indexed vertices instead
			copyMesh.addVertex(vi0);
			copyMesh.addVertex(vi1);
			copyMesh.addVertex(vi2);
		}
	}
	
	// after creating a copy of the mesh, we need to deform the original using a puppet
	// this involves picking some seams and using control points to position the seams to match the copy
	// then we stitch the copied mesh into the original mesh, reusing vertices that are along the seams (merge coincident)
	
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
	gui->addLabelToggle("Show Copy", &showCopy);
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
	if(showCopy) {
		ofPushStyle();
		ofSetLineWidth(1);
		ofSetColor(ofColor::blue);
		copyRegion.draw();
		copyMesh.draw();
		ofPopStyle();
	}
	
	int n = mesh.getNumIndices();
	vector<ofIndexType> newIndices;
	for(int i = 0; i < indices.size(); i++) {
		newIndices.push_back(mesh.getNumVertices());
		int index = indices[i];
		//mesh.addVertex(mesh.getVertex(index));
		if(mesh.hasTexCoords()) {
			//mesh.addTexCoord(mesh.getTexCoord(index));
		}
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
