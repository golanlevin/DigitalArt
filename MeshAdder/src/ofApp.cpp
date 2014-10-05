#include "ofApp.h"
#include "SharedUtils.h"
#include "MeshUtils.h"

float scaleFactor = 2;

void orientMesh(ofMesh& mesh,
                ofVec2f fromStart, ofVec2f fromEnd,
                ofVec2f toStart, ofVec2f toEnd) {
    ofVec2f translation = toStart - fromStart;
    ofVec2f fromVec = fromEnd - fromStart;
    ofVec2f toVec = toEnd - toStart;
    float angle = fromVec.angle(toVec); // degrees
    ofVec3f z(0, 0, 1);
    for(int i = 0; i < mesh.getNumVertices(); i++) {
        ofVec3f& cur = mesh.getVertices()[i];
        cur.rotate(angle, fromStart, z);
        cur += translation;
    }
}

void orientPolyline(ofPolyline& polyline,
                ofVec2f fromStart, ofVec2f fromEnd,
                ofVec2f toStart, ofVec2f toEnd) {
    ofVec2f translation = toStart - fromStart;
    ofVec2f fromVec = fromEnd - fromStart;
    ofVec2f toVec = toEnd - toStart;
    float angle = fromVec.angle(toVec); // degrees
    ofVec3f z(0, 0, 1);
    for(int i = 0; i < polyline.size(); i++) {
        ofVec3f& cur = polyline[i];
        cur.rotate(angle, fromStart, z);
        cur += translation;
    }
}

ofPolyline buildPolyline(ofMesh& mesh, int indices[], int count) {
    ofPolyline line;
    for(int i = 0; i < count; i++) {
        line.addVertex(mesh.getVertex(indices[i]));
    }
    return line;
}

void ofApp::setup() {
	sharedSetup();
	setupGui();
	
	showImage = true;
	showWireframe = true;
	showSplit = false;
	showExtra = false;
    showSides = false;
	
    string handFile = "handmesh-226";
	hand.loadImage("hand/"+handFile+".jpg");
    hand.getPixelsRef().swapRgb();
    hand.update();
	handMesh.load("hand/"+handFile+".ply");
    
    handMesh.clearTexCoords();
	for(int i = 0; i < handMesh.getNumVertices(); i++) {
        handMesh.addTexCoord(ofVec2f(handMesh.getVertex(i)));
	}
	
    // copy of the ring finger
	int toCopy[] = {
        115, 119, 124, 129, 134, 140, 63, 66, 69, 72, 75, 78, 81, 82, 83, 80, 77, 74, 71, 68, 65, 142, 135, 130, 125, 120};
    ofPolyline extraRegion = buildPolyline(handMesh, toCopy, 26);
	extraRegion.close();
    extraMesh = copySubmesh(handMesh, extraRegion);

    // place the extra finger in approximately the right place
    int extraRootIndex = 23, extraKnuckleIndex = 31;
    int splitRootIndex = 115, splitCrotchIndex = 140;
    orientMesh(extraMesh,
               extraMesh.getVertex(extraRootIndex),
               extraMesh.getVertex(extraKnuckleIndex),
               handMesh.getVertex(splitRootIndex),
               handMesh.getVertex(splitCrotchIndex));

    int leftCount = 6, rightCount = 6;

    // find the sides of the copied mesh
    int extraLeftIndices[] = {23, 21, 24, 26, 28, 30};
    extraLeftPath = buildPolyline(extraMesh, extraLeftIndices, leftCount);
    int extraRightIndices[] = {23, 22, 25, 27, 29, 32};
    extraRightPath = buildPolyline(extraMesh, extraRightIndices, rightCount);

    // split path
    int toSplit[] = {119, 124, 129, 134, 140};
    int toSplitCount = 5;
    for(int i = 0; i < toSplitCount; i++) {
        splitPath.addVertex(handMesh.getVertex(toSplit[i]));
        indices.push_back(toSplit[i]);
    }
    split(handMesh, indices);

    // find the sides of the split mesh
    int splitLeftIndices[] = {115, 119, 124, 129, 134, 140};
    int splitRightIndices[] = {115, 151, 152, 153, 154, 155};

    splitLeftPath = buildPolyline(handMesh, splitLeftIndices, leftCount);
    splitRightPath = buildPolyline(handMesh, splitRightIndices, rightCount);

    orientPolyline(splitLeftPath,
                   *splitLeftPath.begin(),
                   *splitLeftPath.rbegin(),
                   *extraLeftPath.begin(),
                   *extraLeftPath.rbegin());
    
    orientPolyline(splitRightPath,
                   *splitRightPath.begin(),
                   *splitRightPath.rbegin(),
                   *extraRightPath.begin(),
                   *extraRightPath.rbegin());
    
    handPuppet.setup(handMesh);

    // put control points along hand's left seam
    for(int i = 0; i < leftCount; i++) {
        int splitIndex = splitLeftIndices[i];
        ofVec2f splitVertex = splitLeftPath[i];
        handPuppet.setControlPoint(splitIndex, splitVertex);
    }

    // put control points along hand's right seam
    for(int i = 0; i < rightCount; i++) {
        int splitIndex = splitRightIndices[i];
        ofVec2f splitVertex = splitRightPath[i];
        handPuppet.setControlPoint(splitIndex, splitVertex);
    }
    
    // bends the hand into shape
    handPuppet.update();
    handMesh = handPuppet.getDeformedMesh();

    fingerPuppet.setup(extraMesh);
    
    // put control points along finger's left seam
    for(int i = 0; i < leftCount; i++) {
        int extraIndex = extraLeftIndices[i];
        ofVec2f splitVertex = splitLeftPath[i];
        fingerPuppet.setControlPoint(extraIndex, splitVertex);
    }
    
    // put control points along finger's right seam
    for(int i = 0; i < rightCount; i++) {
        int extraIndex = extraRightIndices[i];
        ofVec2f splitVertex = splitRightPath[i];
        fingerPuppet.setControlPoint(extraIndex, splitVertex);
    }
    
    // make extra fingernail bisect adjacent fingers
    ofVec2f ringFingernail = handMesh.getVertex(58);
    ofVec2f middleFingernail = handMesh.getVertex(79);
    ofVec2f bisector = (middleFingernail + ringFingernail) / 2;
    ofVec2f extraKnuckle = extraMesh.getVertex(31);
    ofVec2f extraDirection = (bisector - extraKnuckle).normalize();
    ofVec2f middleKnuckle = handMesh.getVertex(141);
    float extraLength = middleKnuckle.distance(middleFingernail);
    fingerPuppet.setControlPoint(16, extraKnuckle + extraDirection * extraLength);
    
    // bend the finger into shape
    fingerPuppet.update();
    extraMesh = fingerPuppet.getDeformedMesh();

    // extract a mesh from the base of the ring finger
    int baseRegionCount = 15;
    int leftBaseRegionIndices[] = {
        115, 118, 123, 128, 133, 138, 42, 43, 44, 140, 134, 129, 124, 119};
    ofPolyline leftBaseRegion = buildPolyline(handMesh, leftBaseRegionIndices, baseRegionCount);
    leftBaseMesh = copySubmesh(handMesh, leftBaseRegion);

    // extract a mesh from the base of the middle finger
    int rightBaseRegionIndices[] = {
        115, 151, 152, 153, 154, 155, 63, 64, 65, 142, 135, 130, 125, 120};
    ofPolyline rightBaseRegion = buildPolyline(handMesh, rightBaseRegionIndices, baseRegionCount);
    rightBaseMesh = copySubmesh(handMesh, rightBaseRegion);

    // set the colors of the left base to fade from right to left
    // and the right base to fade from left to right
    int baseIndices[] = {2, 0, 3, 5, 7, 9, 10, 14, 13, 12, 8, 6, 4, 1, 11};
    float leftBaseOpacity[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, .5};
    float rightBaseOpacity[] = {0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, .5};
    int baseCount = 15;
    vector<ofFloatColor> leftBaseColors(baseCount), rightBaseColors(baseCount);
    for(int i = 0; i < baseCount; i++) {
        leftBaseColors[baseIndices[i]] = ofFloatColor(1, leftBaseOpacity[i]);
        rightBaseColors[baseIndices[i]] = ofFloatColor(1, rightBaseOpacity[i]);
    }
    leftBaseMesh.addColors(leftBaseColors);
    rightBaseMesh.addColors(rightBaseColors);

    // make sure the vertices of the index base match the position of the extra finger
    int extraMeshBaseIndices[] = {23, 21, 24, 26, 28, 30, 0, 2, 4, 32, 29, 27, 25, 22, 31};
    for(int i = 0; i < baseCount; i++) {
        int fromIndex = extraMeshBaseIndices[i];
        int toIndex = baseIndices[i];
        ofVec3f& fromVertex = extraMesh.getVertices()[fromIndex];
        rightBaseMesh.getVertices()[toIndex] = fromVertex;
        leftBaseMesh.getVertices()[toIndex] = fromVertex;
    }
}

void ofApp::setupGui() {	
	gui = new ofxUICanvas();
	gui->addLabel("Mesh Adder");
	gui->addSpacer();
	gui->addLabelToggle("Show Image", &showImage);
	gui->addLabelToggle("Show Wireframe", &showWireframe);
	gui->addLabelToggle("Show Split", &showSplit);
    gui->addLabelToggle("Show Extra", &showExtra);
    gui->addLabelToggle("Show Sides", &showSides);
	gui->autoSizeToFitWidgets();
}

void ofApp::update() {
    handPuppet.update();
}

void ofApp::draw() {
    ofPushMatrix();
    ofBackground(0);
    ofScale(scaleFactor, scaleFactor);
	if (showImage) {
        hand.getTextureReference().bind();
//        handMesh.drawFaces();
        handPuppet.drawFaces();
        fingerPuppet.drawFaces();
        leftBaseMesh.drawFaces();
        rightBaseMesh.drawFaces();
		hand.getTextureReference().unbind();
	}	
    if(showWireframe) {
        handMesh.drawWireframe();
        handPuppet.drawWireframe();
        fingerPuppet.drawWireframe();
        handPuppet.drawControlPoints();
        fingerPuppet.drawControlPoints();
	}
	if(showSplit) {
		ofPushStyle();
		ofSetLineWidth(3);
		ofSetColor(ofColor::red);
		splitPath.draw();
		ofPopStyle();
	}
	if(showExtra) {
		ofPushStyle();
		ofSetLineWidth(1);
        extraMesh.drawWireframe();
        ofPopStyle();
	}
    if(showSides) {
        ofPushStyle();
        ofSetLineWidth(2);
        ofSetColor(ofColor::green);
        extraLeftPath.draw();
        extraRightPath.draw();
        ofSetColor(ofColor::orange);
        splitLeftPath.draw();
        splitRightPath.draw();
        ofPopStyle();
    }
    ofPopMatrix();
}

void ofApp::keyPressed(int key) {
    if(key == ' ') {
//        ofMesh& mesh = rightBaseMesh;
//        ofMesh& mesh = handMesh;
        ofMesh& mesh = extraMesh;
//        ofMesh& mesh = indexBaseMesh;
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
