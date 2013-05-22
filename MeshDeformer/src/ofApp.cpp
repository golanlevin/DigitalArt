#include "ofApp.h"
#include "SharedUtils.h"

void updatePuppet(Skeleton* skeleton, ofxPuppet& puppet) {
	for(int i = 0; i < skeleton->size(); i++) {
		puppet.setControlPoint(skeleton->getControlIndex(i), skeleton->getPositionAbsolute(i));
	}
}

void ofApp::setup() {
	setupGui();
	sharedSetup();
	
	equalizeLength = 40;
	
	lissajousAmplitude = 10;
	lissajousFrequency = 1;
	
	meanderAmount = 20;
	
	propWiggleBaseAngleRange = 15;
	propWiggleMidAngleRange = 30;
	propWiggleTopAngleRange = 45;
	propWiggleSpeedUp = 2;
	propWigglePhaseOffset = 0.5;

	sinLength = 15;
	sinLengthPhaseOffset = 0.5;

	pulseLength = 10;

	retractHeight = 150;

	mouseControl = false;
	showImage = true;
	showWireframe = false;
	showSkeleton = true;
	
	hand.loadImage("hand/genericHandCentered.jpg");
	mesh.load("hand/handmarks.ply");
	for(int i = 0; i < mesh.getNumVertices(); i++) {
		mesh.addTexCoord(mesh.getVertex(i));
	}
	
	puppet.setup(mesh);
	
	previousSkeleton = NULL;
	currentSkeleton = NULL;
	handWithFingertipsSkeleton.setup(mesh);
	immutableHandWithFingertipsSkeleton.setup(mesh);
	handSkeleton.setup(mesh);
	immutableHandSkeleton.setup(mesh);
	threePointSkeleton.setup(mesh);
	immutableThreePointSkeleton.setup(mesh);
	palmSkeleton.setup(mesh);
	immutablePalmSkeleton.setup(mesh);
	setSkeleton(&handSkeleton);
}

void ofApp::setupGui() {
	sceneNames.push_back("None");
	sceneNames.push_back("Wave");
	sceneNames.push_back("Wiggle");
	sceneNames.push_back("Wobble");
	sceneNames.push_back("Equalize");
	sceneNames.push_back("North");
	sceneNames.push_back("Lissajous");
	sceneNames.push_back("Meander");
	sceneNames.push_back("Propagating Wiggle");
	sceneNames.push_back("Sinusoidal Length");
	sceneNames.push_back("Pulsating Palm");
	sceneNames.push_back("Retracting Fingers");

	vector<string> lissajousStyle;
	lissajousStyle.push_back("Circle");
	lissajousStyle.push_back("Ellipse");
	lissajousStyle.push_back("Parabola");
	lissajousStyle.push_back("Bow tie");
	lissajousStyle.push_back("Vertical line");
	
	// create the main gui
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
	gui->addSpacer();
	gui->autoSizeToFitWidgets();
	
	// set the initial scene
	sceneRadio->getToggles()[RETRACTION]->setValue(true);

	// create the scene specific guis
	guis = new ofxUICanvas*[sceneNames.size()];
	for (int i=0; i < sceneNames.size(); i++) {
		guis[i] = new ofxUICanvas();
		guis[i]->setFont("GUI/NewMedia Fett.ttf");
		guis[i]->setPosition(555, 0);
		guis[i]->setVisible(false);
	}
	
	guis[EQUALIZE]->addSlider("Equalize Length", 0, 100, &equalizeLength);

	guis[LISSAJOUS]->addSlider("Lissajous Amplitude", 0, 100, &lissajousAmplitude);
	guis[LISSAJOUS]->addSlider("Lissajous Frequency", 0, 5, &lissajousFrequency);
	lissajousRadio = guis[LISSAJOUS]->addRadio("Lissajous Style", lissajousStyle);

	guis[MEANDER]->addSlider("Meander", 0, 60, &meanderAmount);

	guis[PROP_WIGGLE]->addSlider("Base Angle Range", 10, 60, &propWiggleBaseAngleRange);
	guis[PROP_WIGGLE]->addSlider("Mid Angle Range", 10, 60, &propWiggleMidAngleRange);
	guis[PROP_WIGGLE]->addSlider("Top Angle Range", 10, 60, &propWiggleTopAngleRange);
	guis[PROP_WIGGLE]->addSlider("Wiggle Speed", 1, 3, &propWiggleSpeedUp);
	guis[PROP_WIGGLE]->addSlider("Phase Offset", 0, 1, &propWigglePhaseOffset);

	guis[SIN_LENGTH]->addSlider("Max Length", 10, 30, &sinLength);
	guis[SIN_LENGTH]->addSlider("Phase Offset", 0, 1, &sinLengthPhaseOffset);

	guis[PULSE_PALM]->addSlider("Pulse Length", 5, 20, &pulseLength);

	guis[RETRACTION]->addSlider("Retract Height", 0, 500, &retractHeight);

	for (int i=0; i < sceneNames.size(); i++) {
		guis[i]->autoSizeToFitWidgets();
	}
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
	handWithFingertipsSkeleton.setup(mesh);
	immutableHandWithFingertipsSkeleton.setup(mesh);
	handSkeleton.setup(mesh);
	immutableHandSkeleton.setup(mesh);
	threePointSkeleton.setup(mesh);
	immutableThreePointSkeleton.setup(mesh);
	palmSkeleton.setup(mesh);
	immutablePalmSkeleton.setup(mesh);

	if(mouseControl) {
		ofVec2f mouse(mouseX, mouseY);
		handWithFingertipsSkeleton.setPosition(HandWithFingertipsSkeleton::PALM, mouse, true);
		immutableHandWithFingertipsSkeleton.setPosition(HandWithFingertipsSkeleton::PALM, mouse, true);
		handSkeleton.setPosition(HandSkeleton::PALM, mouse, true);
		immutableHandSkeleton.setPosition(HandSkeleton::PALM, mouse, true);
		threePointSkeleton.setPosition(ThreePointSkeleton::PALM, mouse, true);
		immutableThreePointSkeleton.setPosition(ThreePointSkeleton::PALM, mouse, true);
		palmSkeleton.setPosition(PalmSkeleton::BASE, mouse, true);
		immutablePalmSkeleton.setPosition(PalmSkeleton::BASE, mouse, true);
	}

	// get the current scene
	int scene = getSelection(sceneRadio);
	
	// turn off all scene-specific guis
	for (int i=0; i < sceneNames.size(); i++) {
		guis[i]->setVisible(false);
	}
	// turn on the gui for the current scene
	guis[scene]->setVisible(true);

	// then we modify the skeleton with one of our scenes
	if(scene == WAVE) {
		int toWave[] = {HandSkeleton::PINKY_MID, HandSkeleton::RING_MID, HandSkeleton::MIDDLE_MID, HandSkeleton::INDEX_MID};
		int toWaveCount = 4;
		float theta = ofMap(sin(2 * ofGetElapsedTimef()), -1, 1, -20, 20);
		for(int i = 0; i < toWaveCount; i++) {
			int index = toWave[i];
			handSkeleton.setRotation(index, 2 * theta);
			handSkeleton.setRotation((int) ((int)index-1), -theta);
		}
		setSkeleton(&handSkeleton);
	} else if(scene == WIGGLE) {
		int toWiggle[] = {HandSkeleton::PINKY_TIP, HandSkeleton::RING_TIP, HandSkeleton::MIDDLE_TIP, HandSkeleton::INDEX_TIP, HandSkeleton::THUMB_TIP};
		int toWiggleCount = 5;
		float wiggleRange = 10;
		float t = ofGetElapsedTimef();
		for(int i = 0; i < toWiggleCount; i++) {
			int index = toWiggle[i];
			ofVec2f position(wiggleRange * ofVec2f(ofNoise(i, t, 0), ofNoise(i, t, 1)));
			handSkeleton.setPosition(index, position, false, false);
		}
		setSkeleton(&handSkeleton);
	} else if(scene == WOBBLE) {
		float wiggleRange = 50;
		float t = ofGetElapsedTimef();
		ofVec2f position(wiggleRange * ofVec2f(ofNoise(t, 0), ofNoise(t, 1)));
		handSkeleton.setPosition(HandSkeleton::PALM, position, false, true);
		setSkeleton(&handSkeleton);
	} else if(scene == EQUALIZE) {
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
		setSkeleton(&handSkeleton);
	} else if(scene == NORTH) {
		int toRotate[] = {HandSkeleton::PINKY_BASE, HandSkeleton::RING_BASE, HandSkeleton::MIDDLE_BASE, HandSkeleton::INDEX_BASE};
		int toRotateCount = 4;
		for(int i = 0; i < toRotateCount; i++) {
			int index = toRotate[i];
			handSkeleton.setRotation(index, -90, true);
		}
		setSkeleton(&handSkeleton);
	} else if(scene == LISSAJOUS) {
		float t = lissajousFrequency * ofGetElapsedTimef() * TWO_PI;
		ofVec2f position;
		switch(getSelection(lissajousRadio)) {
			case 0: // circle
				position.set(sin(t + PI/2), sin(t));
				break;
			case 1: // ellipse
				position.set(sin(t + PI/4), sin(t));
				break;
			case 2: // parabola
				position.set(sin(t + PI/2), sin(2*t));
				break;
			case 3: // bow tie
				position.set(sin(2*t), sin(t));
				break;
			case 4: // vertical line
				position.set(0, sin(t));
				break;
		}
		threePointSkeleton.setPosition(ThreePointSkeleton::MIDDLE_HAND, lissajousAmplitude * position, false, true);
		setSkeleton(&threePointSkeleton);
	} else if(scene == MEANDER) {
		int toMove[] = {HandSkeleton::PINKY_BASE, HandSkeleton::RING_BASE, HandSkeleton::MIDDLE_BASE, HandSkeleton::INDEX_BASE};
		int toMoveCount = 4;
		float t = ofGetElapsedTimef();
		for(int i = 0; i < toMoveCount; i++) {
			handSkeleton.setRotation(toMove[i], meanderAmount * ofSignedNoise(i, t));
		}
		setSkeleton(&handSkeleton);
	} else if(scene == PROP_WIGGLE) {
		int base[] = {HandWithFingertipsSkeleton::PINKY_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::INDEX_BASE};
		int mid[] = {HandWithFingertipsSkeleton::PINKY_MID, HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::INDEX_MID};
		int top[] = {HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::INDEX_TOP};
		int fingerCount = 4;

		int index;
		ofVec2f original;
		float theta;
		for (int i=0; i < fingerCount; i++) {
			float baseOffset = 0*propWigglePhaseOffset + i;
			float midOffset = 1*propWigglePhaseOffset + i;
			float topOffset = 2*propWigglePhaseOffset + i;

			index = base[i];
			original = puppet.getOriginalMesh().getVertex(index);
			theta = ofMap(sin(propWiggleSpeedUp*ofGetElapsedTimef() + baseOffset), -1, 1, -(propWiggleBaseAngleRange/2.0), propWiggleBaseAngleRange/2.0);
			handWithFingertipsSkeleton.setRotation(index, theta, false, false);
			
			index = mid[i];
			original = puppet.getOriginalMesh().getVertex(index);
			theta = -ofMap(sin(propWiggleSpeedUp*ofGetElapsedTimef() + midOffset), -1, 1, -(propWiggleMidAngleRange/2.0), propWiggleMidAngleRange/2.0);
			handWithFingertipsSkeleton.setRotation(index, theta, false, false);

			index = top[i];
			original = puppet.getOriginalMesh().getVertex(index);
			theta = ofMap(sin(propWiggleSpeedUp*ofGetElapsedTimef() + topOffset), -1, 1, -(propWiggleTopAngleRange/2.0), propWiggleTopAngleRange/2.0);
			handWithFingertipsSkeleton.setRotation(index, theta, false , false);
		}
		setSkeleton(&handWithFingertipsSkeleton);
	} else if(scene == SIN_LENGTH) {
		int mid[] = {HandSkeleton::PINKY_MID, HandSkeleton::RING_MID, HandSkeleton::MIDDLE_MID, HandSkeleton::INDEX_MID, HandSkeleton::THUMB_MID};
		int tip[] = {HandSkeleton::PINKY_TIP, HandSkeleton::RING_TIP, HandSkeleton::MIDDLE_TIP, HandSkeleton::INDEX_TIP, HandSkeleton::THUMB_TIP};
		int fingerCount = 5;
		float t = ofGetElapsedTimef();
		
		int index;
		ofVec2f original;
		ofVec2f parent;
		ofVec2f position(0, 0);
		for(int i = 0; i < fingerCount; i++) {
			index = mid[i];
			original = puppet.getOriginalMesh().getVertex(handSkeleton.getControlIndex(index));
			parent = puppet.getOriginalMesh().getVertex(handSkeleton.getControlIndex((int) ((int)index-1)));
			position.set(original-parent);
			position.normalize();
			position = position * (sinLength*sin(t + i*sinLengthPhaseOffset));
			handSkeleton.setPosition(index, position, false, false);

			index = tip[i];
			original = puppet.getOriginalMesh().getVertex(handSkeleton.getControlIndex(index));
			parent = puppet.getOriginalMesh().getVertex(handSkeleton.getControlIndex((int) ((int)index-1)));
			position.set(original-parent);
			position.normalize();
			position = position * (sinLength*sin(t + i*sinLengthPhaseOffset));
			handSkeleton.setPosition(index, position, false, false);
		}
		setSkeleton(&handSkeleton);
	} else if(scene == PULSE_PALM) {
		int toPulsate[] = {PalmSkeleton::TOP, PalmSkeleton::RIGHT_BASE, PalmSkeleton::RIGHT_MID, PalmSkeleton::RIGHT_TOP, PalmSkeleton::LEFT_BASE, PalmSkeleton::LEFT_MID, PalmSkeleton::LEFT_TOP};
		int toPulsateCount = 7;
		float t = ofGetElapsedTimef();
		
		for(int i = 0; i < toPulsateCount; i++) {
			int index = toPulsate[i];
			ofVec2f original = puppet.getOriginalMesh().getVertex(palmSkeleton.getControlIndex(index));
			ofVec2f parent = puppet.getOriginalMesh().getVertex(palmSkeleton.getControlIndex(PalmSkeleton::CENTROID));
			ofVec2f position(original-parent);
			position.normalize();
			position = position * (pulseLength*sin(t));
			palmSkeleton.setPosition(index, position, false, false);
		}
		setSkeleton(&palmSkeleton);
	} else if(scene == RETRACTION) {
		int palm = HandWithFingertipsSkeleton::PALM;
		int tip[] = {HandWithFingertipsSkeleton::PINKY_TIP, HandWithFingertipsSkeleton::RING_TIP, HandWithFingertipsSkeleton::MIDDLE_TIP, HandWithFingertipsSkeleton::INDEX_TIP};
		int top[] = {HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::INDEX_TOP};
		int mid[] = {HandWithFingertipsSkeleton::PINKY_MID, HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::INDEX_MID};
		int base[] = {HandWithFingertipsSkeleton::PINKY_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::INDEX_BASE};

		float tipRatio = 0.2;
		float topRatio = 0.3;
		float middleRatio = 0.5;
		float bottomRatio = 1;
		float fingerTot = tipRatio + topRatio + middleRatio + bottomRatio;

		int fingerCount = 4;
		for(int i = 0; i < fingerCount; i++) {
			ofVec2f fingerTip = immutableHandWithFingertipsSkeleton.getPositionAbsolute(tip[i]);
			if (fingerTip.y < retractHeight) {
				ofVec2f fingerBase = immutableHandWithFingertipsSkeleton.getPositionAbsolute(palm);

				float bigDx = fingerTip.x - fingerBase.x;
				float bigDy = fingerTip.y - fingerBase.y;

				float dy = retractHeight - fingerBase.y;
				float dx = (dy * bigDx) / bigDy;

				float maxLen = sqrt(dx*dx + dy*dy);

				if (dy > 0) {
					dy = 0; 
					dx = 0;
					maxLen = 0;
				}

				ofVec2f original;
				ofVec2f parent;
				ofVec2f position;
		
				original = immutableHandWithFingertipsSkeleton.getPositionAbsolute(tip[i]);
				parent = immutableHandWithFingertipsSkeleton.getPositionAbsolute(top[i]);
				position = original - parent;
				position.normalize();
				position *= (tipRatio/fingerTot) * maxLen;
				handWithFingertipsSkeleton.setBoneLength(tip[i], position);

				original = immutableHandWithFingertipsSkeleton.getPositionAbsolute(top[i]);
				parent = immutableHandWithFingertipsSkeleton.getPositionAbsolute(mid[i]);
				position = original - parent;
				position.normalize();
				position *= (topRatio/fingerTot) * maxLen;
				handWithFingertipsSkeleton.setBoneLength(top[i], position);

				original = immutableHandWithFingertipsSkeleton.getPositionAbsolute(mid[i]);
				parent = immutableHandWithFingertipsSkeleton.getPositionAbsolute(base[i]);
				position = original - parent;
				position.normalize();
				position *= (middleRatio/fingerTot) * maxLen;
				handWithFingertipsSkeleton.setBoneLength(mid[i], position);

				original = immutableHandWithFingertipsSkeleton.getPositionAbsolute(base[i]);
				parent = immutableHandWithFingertipsSkeleton.getPositionAbsolute(palm);
				position = original - parent;
				position.normalize();
				position *= (bottomRatio/fingerTot) * maxLen;
				handWithFingertipsSkeleton.setBoneLength(base[i], position);
			}	
		}
		setSkeleton(&handWithFingertipsSkeleton);
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

	int scene = getSelection(sceneRadio);
	if (scene == RETRACTION) {
		ofSetColor(255);
		ofLine(0, retractHeight, 768, retractHeight);
	}
}

void ofApp::keyPressed(int key) {
}

void ofApp::setSkeleton(Skeleton* skeleton) {
	if(skeleton != currentSkeleton) {
		previousSkeleton = currentSkeleton;
		currentSkeleton = skeleton;
		if(previousSkeleton != NULL) {
			vector<int>& previousControlIndices = previousSkeleton->getControlIndices();
			for(int i = 0; i < previousControlIndices.size(); i++) {
				puppet.removeControlPoint(previousControlIndices[i]);
			}
		}
		vector<int>& currentControlIndices = currentSkeleton->getControlIndices();
		for(int i = 0; i < currentControlIndices.size(); i++) {
			puppet.setControlPoint(currentControlIndices[i]);
		}
	}
}