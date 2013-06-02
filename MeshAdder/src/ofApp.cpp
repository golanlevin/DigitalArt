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
	for(int i = 0; i < toSplitCount; i++) {
		splitPath.addVertex(mesh.getVertex(toSplit[i]));
		indices.push_back(toSplit[i]);
	}
	
	split(mesh, indices);
	//mesh = dropUnusedVertices(mesh);
	
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
	
	/*
	ofMesh edgeTriangles;
	edgeTriangles.setMode(OF_PRIMITIVE_TRIANGLES);
	for(int i = 0; i < n; i += 3) {
		ofIndexType i0 = mesh.getIndex(i + 0);
		ofIndexType i1 = mesh.getIndex(i + 1);
		ofIndexType i2 = mesh.getIndex(i + 2);
		
		set<ofIndexType> triangleIndices;
		triangleIndices.insert(i0);
		triangleIndices.insert(i1);
		triangleIndices.insert(i2);
		
		for(int j = 0; j < indices.size() - 1; j++) {
			int j0 = indices[j], j1 = indices[j + 1];
			
			set<ofIndexType> lineIndices;
			lineIndices.insert(j0);
			lineIndices.insert(j1);
			
			set<ofIndexType> result;
			set_intersection(lineIndices.begin(), lineIndices.end(), triangleIndices.begin(), triangleIndices.end(), std::inserter(result, result.end()));
			
			if(!result.empty()) {
				ofVec2f vj0 = mesh.getVertex(j0);
				ofVec2f vj1 = mesh.getVertex(j1);
				ofVec2f vi0 = mesh.getVertex(i0);
				ofVec2f vi1 = mesh.getVertex(i1);
				ofVec2f vi2 = mesh.getVertex(i2);
				ofVec2f avg = (vi0 + vi1 + vi2) / 3;
				
				if(isLeft(vj0, vj1, avg)) {
					for(int k = 0; k < 3; k++) {
						int cur = mesh.getIndex(i + k);
						if(cur == indices[j]) {
							mesh.setIndex(i + k, newIndices[j]);
						} else if(cur == indices[j + 1]) {
							mesh.setIndex(i + k, newIndices[j + 1]);
						}
					}
					edgeTriangles.addColor(ofColor::red);
					edgeTriangles.addColor(ofColor::red);
					edgeTriangles.addColor(ofColor::red);
				} else {
					edgeTriangles.addColor(ofColor::blue);
					edgeTriangles.addColor(ofColor::blue);
					edgeTriangles.addColor(ofColor::blue);
				}
				edgeTriangles.addVertex(vi0);
				edgeTriangles.addVertex(vi1);
				edgeTriangles.addVertex(vi2);
			}
		}
	}
	edgeTriangles.drawWireframe();
	 */
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
