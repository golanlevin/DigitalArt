#include "ofApp.h"
#include "SharedUtils.h"

void updatePuppet(Skeleton* skeleton, ofxPuppet& puppet) {
	for(int i = 0; i < skeleton->size(); i++) {
		puppet.setControlPoint(i, skeleton->getPositionAbsolute(i));
	}
}

void ofApp::setup() {
	sharedSetup();
	setupGui();
	
	equalizeLength = 40;
	mouseControl = false;
	showImage = false;
	showWireframe = false;
	showSkeleton = true;
	
	hand.loadImage("hand/genericHandCentered.jpg");
	mesh.load("hand/handmarks.ply");
	for(int i = 0; i < mesh.getNumVertices(); i++) {
		mesh.addTexCoord(mesh.getVertex(i));
	}
	puppet.setup(mesh);
	handSkeleton.setup(mesh);
	threePointSkeleton.setup(mesh);
	currentSkeleton = &handSkeleton;
	
	for(int i = 0; i < 17; i++) {
		puppet.setControlPoint(i);
	}
}

void ofApp::setupGui() {
	sceneNames.push_back("None");
	sceneNames.push_back("Wave");
	sceneNames.push_back("Wiggle");
	sceneNames.push_back("Wobble");
	sceneNames.push_back("Equalize");
	sceneNames.push_back("North");
	sceneNames.push_back("Waggle");
	
	gui = new ofxUICanvas();
	gui->addLabel("Mesh Deformer");
	gui->addSpacer();
	gui->addFPS();
	gui->addSpacer();
	sceneRadio = gui->addRadio("Scene", sceneNames);
	gui->addSpacer();
	gui->addSlider("Equalize Length", 0, 100, &equalizeLength);
	gui->addLabelToggle("Mouse Control", &mouseControl);
	gui->addLabelToggle("Show Image", &showImage);
	gui->addLabelToggle("Show Wireframe", &showWireframe);
	gui->addLabelToggle("Show Skeleton", &showSkeleton);
	gui->autoSizeToFitWidgets();
	
	sceneRadio->getToggles()[0]->setValue(true);
}

int getSelection(ofxUIRadio* radio) {
	vector<ofxUIToggle*> toggles = radio->getToggles();
	for(int i = 0; i < toggles.size(); i++) {
		if(toggles[i]->getValue()) {
			return i;
		}
	}
	return -1;
}

void ofApp::update() {
	// every frame we get a new mesh from the hand tracker
	handSkeleton.setup(mesh);
	threePointSkeleton.setup(mesh);
	
	if(mouseControl) {
		ofVec2f mouse(mouseX, mouseY);
		handSkeleton.setPosition(HandSkeleton::PALM, mouse, true);
		threePointSkeleton.setPosition(HandSkeleton::PALM, mouse, true);
	}
	
	// then we modify the skeleton with one of our scenes
	int scene = getSelection(sceneRadio);
	if(scene == 1) {
		int toWave[] = {HandSkeleton::PINKY_MID, HandSkeleton::RING_MID, HandSkeleton::MIDDLE_MID, HandSkeleton::INDEX_MID};
		int toWaveCount = 4;
		float theta = ofMap(sin(2 * ofGetElapsedTimef()), -1, 1, -20, 20);
		for(int i = 0; i < toWaveCount; i++) {
			int index = toWave[i];
			handSkeleton.setRotation(index, 2 * theta);
			handSkeleton.setRotation((int) ((int)index-1), -theta);
		}
		currentSkeleton = &handSkeleton;
	} else if(scene == 2) {
		int toWiggle[] = {HandSkeleton::PINKY_TIP, HandSkeleton::RING_TIP, HandSkeleton::MIDDLE_TIP, HandSkeleton::INDEX_TIP, HandSkeleton::THUMB_TIP};
		int toWiggleCount = 5;
		float wiggleRange = 10;
		float t = ofGetElapsedTimef();
		for(int i = 0; i < toWiggleCount; i++) {
			int index = toWiggle[i];
			ofVec2f original = puppet.getOriginalMesh().getVertex(index);
			ofVec2f position(wiggleRange * ofVec2f(ofNoise(i, t, 0), ofNoise(i, t, 1)));
			handSkeleton.setPosition(index, position, false, false);
		}
		currentSkeleton = &handSkeleton;
	} else if(scene == 3) {
		float wiggleRange = 50;
		float t = ofGetElapsedTimef();
		ofVec2f position(wiggleRange * ofVec2f(ofNoise(t, 0), ofNoise(t, 1)));
		handSkeleton.setPosition(HandSkeleton::PALM, position, false, true);
		currentSkeleton = &handSkeleton;
	} else if(scene == 4) {
		int toEqualize[] = {
			HandSkeleton::PINKY_TIP, HandSkeleton::RING_TIP, HandSkeleton::MIDDLE_TIP, HandSkeleton::INDEX_TIP,
			HandSkeleton::PINKY_MID, HandSkeleton::RING_MID, HandSkeleton::MIDDLE_MID, HandSkeleton::INDEX_MID
		};
		float ratios[] = {
			1, 1, 1, 1,
			1.6, 1.6, 1.6, 1.6
		};
		int toEqualizeCount = 8;
		for(int i = 0; i < toEqualizeCount; i++) {
			handSkeleton.setBoneLength(toEqualize[i], ratios[i] * equalizeLength);
		}
		ofVec2f pinkyBase = handSkeleton.getPositionAbsolute(HandSkeleton::PINKY_BASE);
		ofVec2f indexBase = handSkeleton.getPositionAbsolute(HandSkeleton::INDEX_BASE);
		handSkeleton.setPosition(HandSkeleton::RING_BASE, pinkyBase.getInterpolated(indexBase, 1/3.), true);
		handSkeleton.setPosition(HandSkeleton::MIDDLE_BASE, pinkyBase.getInterpolated(indexBase, 2/3.), true);
		currentSkeleton = &handSkeleton;
	} else if(scene == 5) {
		int toRotate[] = {HandSkeleton::PINKY_BASE, HandSkeleton::RING_BASE, HandSkeleton::MIDDLE_BASE, HandSkeleton::INDEX_BASE};
		int toRotateCount = 4;
		for(int i = 0; i < toRotateCount; i++) {
			int index = toRotate[i];
			handSkeleton.setRotation(index, -90, true);
		}
		currentSkeleton = &handSkeleton;
	} else if(scene == 6) {
		ofVec2f position(10, 0);
		position.rotate(ofGetElapsedTimef() * 100);
		threePointSkeleton.setPosition(ThreePointSkeleton::MIDDLE_HAND, position, false, true);
		currentSkeleton = &threePointSkeleton;
	}
	
	// we update the puppet using that skeleton
	updatePuppet(currentSkeleton, puppet);
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
	if(showSkeleton) {
		currentSkeleton->draw();
	}
}

void ofApp::keyPressed(int key) {
}