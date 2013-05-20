#include "ofApp.h"
#include "SharedUtils.h"

void updatePuppet(Skeleton& skeleton, ofxPuppet& puppet) {
	for(int i = 0; i < skeleton.size(); i++) {
		puppet.setControlPoint(i, skeleton.getPositionAbsolute((Bone::Label) i));
	}
}

void ofApp::setup() {
	setupGui();
	sharedSetup();
	
	mouseControl = true;
	showImage = true;
	showWireframe = true;
	showSkeleton = true;
	
	hand.loadImage("hand/genericHandCentered.jpg");
	mesh.load("hand/handmarks.ply");
	for(int i = 0; i < mesh.getNumVertices(); i++) {
		mesh.addTexCoord(mesh.getVertex(i));
	}
	puppet.setup(mesh);
	skeleton.setup(mesh);
	
	for(int i = 0; i < 17; i++) {
		puppet.setControlPoint(i);
	}
}

void ofApp::setupGui() {
	sceneNames.push_back("Wave");
	sceneNames.push_back("Wiggle");
	sceneNames.push_back("Wobble");
	
	gui = new ofxUICanvas();
	gui->setFont("GUI/NewMedia Fett.ttf");
	gui->addLabel("Mesh Deformer");
	gui->addSpacer();
	gui->addFPS();
	gui->addSpacer();
	sceneRadio = gui->addRadio("Scene", sceneNames);
	gui->addSpacer();
	gui->addLabelToggle("Mouse Control", &mouseControl);
	gui->addLabelToggle("Show Image", &showImage);
	gui->addLabelToggle("Show Wireframe", &showWireframe);
	gui->addLabelToggle("Show Skeleton", &showSkeleton);
	gui->autoSizeToFitWidgets();
	
	//sceneRadio->getToggles()[0]->setValue(true);
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
	skeleton.setup(mesh);
	
	if(mouseControl) {
		//skeleton.setPositionAbsolute(Bone::PALM, ofVec2f(mouseX, mouseY));
	}
	
	// then we modify the skeleton with one of our scenes
	int scene = getSelection(sceneRadio);
	if(scene == 0) {
		Bone::Label toWave[] = {Bone::PINKY_MID, Bone::RING_MID, Bone::MIDDLE_MID, Bone::INDEX_MID};
		int toWaveCount = 4;
		float theta = ofMap(sin(ofGetElapsedTimef()), -1, 1, -30, 30);
		for(int i = 0; i < toWaveCount; i++) {
			Bone::Label index = toWave[i];
			ofVec2f original = puppet.getOriginalMesh().getVertex(index);
			skeleton.setRotation(index, theta);
			skeleton.setRotation((Bone::Label) ((int)index-1), -theta);
		}
	} else if(scene == 1) {
		Bone::Label toWiggle[] = {Bone::PINKY_TIP, Bone::RING_TIP, Bone::MIDDLE_TIP, Bone::INDEX_TIP, Bone::THUMB_TIP};
		int toWiggleCount = 5;
		float wiggleRange = 10;
		float t = ofGetElapsedTimef();
		for(int i = 0; i < toWiggleCount; i++) {
			Bone::Label index = toWiggle[i];
			ofVec2f original = puppet.getOriginalMesh().getVertex(index);
			skeleton.setPositionRelativeToSelf(index, wiggleRange * ofVec2f(ofNoise(i, t, 0), ofNoise(i, t, 1)));
		}
	} else if(scene == 2) {
		float wiggleRange = 50;
		float t = ofGetElapsedTimef();
		skeleton.setPositionAbsoluteIndependent(Bone::PALM, wiggleRange * ofVec2f(ofNoise(t, 0), ofNoise(t, 1)));
	}
	
	// we update the puppet using that skeleton
	updatePuppet(skeleton, puppet);
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
		skeleton.draw();
	}
}

void ofApp::keyPressed(int key) {
}