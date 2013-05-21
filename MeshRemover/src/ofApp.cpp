#include "ofApp.h"
#include "SharedUtils.h"

void removeTriangles(ofMesh& mesh, ofPolyline& region) {
	int n = mesh.getNumIndices();
	vector<ofIndexType> indices;
	for(int i = 0; i < n;) {
		ofIndexType i0 = mesh.getIndex(i++);
		ofIndexType i1 = mesh.getIndex(i++);
		ofIndexType i2 = mesh.getIndex(i++);
		ofVec2f v0 = mesh.getVertex(i0);
		ofVec2f v1 = mesh.getVertex(i1);
		ofVec2f v2 = mesh.getVertex(i2);
		ofVec2f avg = (v0 + v1 + v2) / 3.;
		if(!region.inside(avg)) {
			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);
		}
	}
	mesh.clearIndices();
	mesh.addIndices(indices);
}

ofMesh dropUnusedVertices(ofMesh& mesh) {
	int n = mesh.getNumIndices();
	ofMesh out;
	vector<bool> used(n);
	for(int i = 0; i < n; i++) {
		used[mesh.getIndex(i)] = true;
	}
	int total = 0;
	vector<ofIndexType> newIndex(n);
	for(int i = 0; i < n; i++) {
		if(used[i]) {
			newIndex[i] = total++;
			out.addVertex(mesh.getVertex(i));
			if(mesh.hasTexCoords()) {
				out.addTexCoord(mesh.getTexCoord(i));
			}
		}
	}
	for(int i = 0; i < n; i++) {
		out.addIndex(newIndex[mesh.getIndex(i)]);
	}
	return out;
}

void mergeCoincidentVertices(ofMesh& mesh, float epsilon = 10e-5) {
	int n = mesh.getNumVertices();
	vector<ofIndexType> newIndex(n);
	float epsilonSquared = epsilon * epsilon;
	for(int i = 0; i < n; i++) {
		ofVec2f cur = mesh.getVertex(i);
		newIndex[i] = i;
		for(int j = 0; j < i; j++) {
			ofVec2f other = mesh.getVertex(j);
			if(cur.squareDistance(other) < epsilonSquared) {
				newIndex[i] = j;
				break;
			}
		}
	}
	int m = mesh.getNumIndices();
	for(int i = 0; i < m; i++) {
		ofIndexType index = mesh.getIndex(i);
		mesh.setIndex(i, newIndex[index]);
	}
}

ofMesh removeAndStitch(ofMesh& mesh, ofPolyline& removalRegion, vector<pair<ofIndexType, ofIndexType> >& stitch) {
	ofMesh out = mesh;
	removeTriangles(out, removalRegion);
	out = dropUnusedVertices(out);
	ofxPuppet puppet;
	puppet.setup(out);
	puppet.update();
	for(int i = 0; i < stitch.size(); i++) {
		ofIndexType left = stitch[i].first, right = stitch[i].second;
		ofVec2f avg = (out.getVertex(left) + out.getVertex(right)) / 2;
		puppet.setControlPoint(left, avg);
		puppet.setControlPoint(right, avg);
	}
	puppet.update();
	out = puppet.getDeformedMesh();
	mergeCoincidentVertices(out);
	return dropUnusedVertices(out);
}

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
		mesh.drawWireframe();
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
