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
	tipRatio = 0.2;
	topRatio = 0.3;
	middleRatio = 0.5;
	bottomRatio = 1;

	sinWiggleAngleRange = 45;
	sinWiggleSpeedUp = 2;
	sinWigglePhaseOffset = 0.5;

	wristWigglePalmAngleRange = 45;
	wristWiggleTopAngleRange = 30;
	wristWiggleMidAngleRange = 15;
	wristWiggleSpeedUp = 2;
	wristWigglePhaseOffset = 0.5;

	middleLength = 80;
	otherLength = 40;

	growingMiddleBaseAngleRange = 60;
	growingMiddleMidAngleRange = 30;
	growingMiddleTopAngleRange = 15;
	growingMiddleLength = 225;
	growingMiddleGrowthAmount = 1.2;
	growingMiddleSpeedUp = 5;
	growingMiddlePhaseOffset = 0.5;

	splayHeight = 530;
	splayAxis = 291;
	splayMaxAngle = 45;

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
	wristSpineSkeleton.setup(mesh);
	immutableWristSpineSkeleton.setup(mesh);

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
	sceneNames.push_back("Sinusoidal Wiggle");
	sceneNames.push_back("Wiggling Wrist");
	sceneNames.push_back("Middle Different Length");
	sceneNames.push_back("Growing Middle Finger");
	sceneNames.push_back("StarTrek");
	sceneNames.push_back("Straighten");
	sceneNames.push_back("Splay");

	vector<string> lissajousStyle;
	lissajousStyle.push_back("Circle");
	lissajousStyle.push_back("Ellipse");
	lissajousStyle.push_back("Parabola");
	lissajousStyle.push_back("Bow tie");
	lissajousStyle.push_back("Vertical line");
	
	vector<string> sinusoidalWiggleJoint;
	sinusoidalWiggleJoint.push_back("Base");
	sinusoidalWiggleJoint.push_back("Mid");
	sinusoidalWiggleJoint.push_back("Top");

	// create the main gui
	gui = new ofxUICanvas();
	gui->setFont("GUI/NewMedia Fett.ttf"); 
	gui->addLabel("Mesh Deformer");
	gui->addSpacer();
	gui->addFPS();
	gui->addSpacer();
	sceneRadio = gui->addRadio("Scene", sceneNames);
	gui->addSpacer();
	gui->addLabelToggle("Show Image", &showImage);
	gui->addLabelToggle("Show Wireframe", &showWireframe);
	gui->addLabelToggle("Show Skeleton", &showSkeleton);
	gui->addLabelToggle("Mouse Control", &mouseControl);
	gui->addSpacer();
	gui->autoSizeToFitWidgets();
	
	// set the initial scene
	sceneRadio->getToggles()[SPLAY]->setValue(true);

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
	lissajousRadio->getToggles()[0]->setValue(true);

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
	guis[RETRACTION]->addSlider("Tip Ratio", 0, 1, &tipRatio);
	guis[RETRACTION]->addSlider("Top Ratio", 0, 1, &topRatio);
	guis[RETRACTION]->addSlider("Middle Ratio", 0, 1, &middleRatio);
	guis[RETRACTION]->addSlider("Bottom Ratio", 0, 1, &bottomRatio);

	guis[SIN_WIGGLE]->addSlider("Angle Range", 5, 85, &sinWiggleAngleRange);
	guis[SIN_WIGGLE]->addSlider("Speed Up", 1, 5, &sinWiggleSpeedUp);
	guis[SIN_WIGGLE]->addSlider("Phase Offset", 0, 1, &sinWigglePhaseOffset);
	sinusoidalWiggleRadio = guis[SIN_WIGGLE]->addRadio("Wiggle Joint", sinusoidalWiggleJoint);
	sinusoidalWiggleRadio->getToggles()[0]->setValue(true);

	guis[WRIST_WIGGLE]->addSlider("Palm Angle Range", 10, 60, &wristWigglePalmAngleRange);
	guis[WRIST_WIGGLE]->addSlider("Top Angle Range", 10, 60, &wristWiggleTopAngleRange);
	guis[WRIST_WIGGLE]->addSlider("Mid Angle Range", 10, 60, &wristWiggleMidAngleRange);
	guis[WRIST_WIGGLE]->addSlider("Wiggle Speed Up", 1, 3, &wristWiggleSpeedUp);
	guis[WRIST_WIGGLE]->addSlider("Phase Offset", 0, 1, &wristWigglePhaseOffset);

	guis[MIDDLE_LEN]->addSlider("Middle Length", 0, 100, &middleLength);
	guis[MIDDLE_LEN]->addSlider("Other Fingers Length", 0, 100, &otherLength);

	guis[GROWING_MIDDLE]->addSlider("Base Angle Range", 10, 90, &growingMiddleBaseAngleRange);
	guis[GROWING_MIDDLE]->addSlider("Mid Angle Range", 10, 90, &growingMiddleMidAngleRange);
	guis[GROWING_MIDDLE]->addSlider("Top Angle Range", 10, 90, &growingMiddleTopAngleRange);
	guis[GROWING_MIDDLE]->addSlider("Middle Length", 200, 300, &growingMiddleLength);
	guis[GROWING_MIDDLE]->addSlider("Growth Amount", 1, 2, &growingMiddleGrowthAmount);
	guis[GROWING_MIDDLE]->addSlider("Speed Up", 1, 10, &growingMiddleSpeedUp);
	guis[GROWING_MIDDLE]->addSlider("Phase Offset", 0, 1, &growingMiddlePhaseOffset);

	guis[SPLAY]->addSlider("Splay Height", 0, 1024, &splayHeight);
	guis[SPLAY]->addSlider("Splay Axis", 0, 768, &splayAxis);
	guis[SPLAY]->addSlider("Splay Angle Increment", 0, 90, &splayMaxAngle);

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
	wristSpineSkeleton.setup(mesh);
	immutableWristSpineSkeleton.setup(mesh);

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
		wristSpineSkeleton.setPosition(WristSpineSkeleton::HAND_TOP, mouse, true);
		immutableWristSpineSkeleton.setPosition(WristSpineSkeleton::HAND_TOP, mouse, true);
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
		float theta;
		for (int i=0; i < fingerCount; i++) {
			float baseOffset = 0*propWigglePhaseOffset + i;
			float midOffset = 1*propWigglePhaseOffset + i;
			float topOffset = 2*propWigglePhaseOffset + i;

			index = base[i];
			theta = ofMap(sin(propWiggleSpeedUp*ofGetElapsedTimef() + baseOffset), -1, 1, -(propWiggleBaseAngleRange/2.0), propWiggleBaseAngleRange/2.0);
			handWithFingertipsSkeleton.setRotation(index, theta, false, false);
			
			index = mid[i];
			theta = -ofMap(sin(propWiggleSpeedUp*ofGetElapsedTimef() + midOffset), -1, 1, -(propWiggleMidAngleRange/2.0), propWiggleMidAngleRange/2.0);
			handWithFingertipsSkeleton.setRotation(index, theta, false, false);

			index = top[i];
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
	} else if(scene == SIN_WIGGLE) {
		int base[] = {HandWithFingertipsSkeleton::PINKY_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::INDEX_BASE, HandWithFingertipsSkeleton::THUMB_BASE};
		int mid[] = {HandWithFingertipsSkeleton::PINKY_MID, HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::INDEX_MID, HandWithFingertipsSkeleton::THUMB_MID};
		int top[] = {HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::INDEX_TOP, HandWithFingertipsSkeleton::THUMB_TOP};
			
		int* toWiggle;
		int toWiggleCount = 5;
		switch(getSelection(sinusoidalWiggleRadio)) {
			case 0: // base
				toWiggle = base;
				break;
			case 1: // mid
				toWiggle = mid;
				break;
			case 2: // top
				toWiggle = top;
				break;
		}
		
		for (int i=0; i < toWiggleCount; i++) {
			float phaseOffset = i*sinWigglePhaseOffset + i;

			int index = toWiggle[i];
			float theta = ofMap(sin(sinWiggleSpeedUp*ofGetElapsedTimef() + phaseOffset), -1, 1, -(sinWiggleAngleRange/2.0), sinWiggleAngleRange/2.0);
			handWithFingertipsSkeleton.setRotation(index, theta, false, false);
		}
		setSkeleton(&handWithFingertipsSkeleton);
	} else if (scene == WRIST_WIGGLE) {
		int index;
		ofVec2f original;
		float theta;

		float palmOffset = 0*wristWigglePhaseOffset;
		float topOffset = 1*wristWigglePhaseOffset;
		float midOffset = 2*wristWigglePhaseOffset;

		index = WristSpineSkeleton::PALM;
		theta = ofMap(sin(wristWiggleSpeedUp*ofGetElapsedTimef() + palmOffset), -1, 1, -(wristWigglePalmAngleRange/2.0), wristWigglePalmAngleRange/2.0);
		wristSpineSkeleton.setRotation(index, theta, false, false);
			
		index = WristSpineSkeleton::WRIST_TOP;
		theta = -ofMap(sin(wristWiggleSpeedUp*ofGetElapsedTimef() + topOffset), -1, 1, -(wristWiggleTopAngleRange/2.0), wristWiggleTopAngleRange/2.0);
		wristSpineSkeleton.setRotation(index, theta, false, false);

		index = WristSpineSkeleton::WRIST_MID;
		theta = ofMap(sin(wristWiggleSpeedUp*ofGetElapsedTimef() + midOffset), -1, 1, -(wristWiggleMidAngleRange/2.0), wristWiggleMidAngleRange/2.0);
		handWithFingertipsSkeleton.setRotation(index, theta, false , false);

		setSkeleton(&wristSpineSkeleton);
	} else if(scene == MIDDLE_LEN) {
		int middle[] = {HandSkeleton::MIDDLE_TIP, HandSkeleton::MIDDLE_MID};
		int middleRatios[] = {1, 1.6};
		int middleCount = 2;
		for(int i = 0; i < middleCount; i++) {
			handSkeleton.setBoneLength(middle[i], middleRatios[i] * middleLength);
		}

		int others[] = {
			HandSkeleton::PINKY_TIP, HandSkeleton::RING_TIP, HandSkeleton::INDEX_TIP,
			HandSkeleton::PINKY_MID, HandSkeleton::RING_MID, HandSkeleton::INDEX_MID
		};
		float otherRatios[] = {
			1, 1, 1,
			1.6, 1.6, 1.6
		};
		int otherCount = 6;
		for(int i = 0; i < otherCount; i++) {
			handSkeleton.setBoneLength(others[i], otherRatios[i] * otherLength);
		}

		setSkeleton(&handSkeleton);
	} else if(scene == GROWING_MIDDLE) {
		int fingerParts[] = {HandWithFingertipsSkeleton::MIDDLE_TIP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::MIDDLE_BASE};
		float ratios[] = {0.2, 0.3, 0.5};
		float fingerPartCount = 3;

		float t = ofGetElapsedTimef();

		ofVec2f fingerBase = immutableHandWithFingertipsSkeleton.getPositionAbsolute(HandWithFingertipsSkeleton::MIDDLE_BASE);
		ofVec2f fingerTip = immutableHandWithFingertipsSkeleton.getPositionAbsolute(HandWithFingertipsSkeleton::MIDDLE_TIP);
		float len = fingerBase.distance(fingerTip);
		float newLen = len + t*growingMiddleGrowthAmount;
		
		if (newLen > growingMiddleLength) newLen = growingMiddleLength;
		//else {
			float angleRanges[] = {growingMiddleTopAngleRange, growingMiddleMidAngleRange, growingMiddleTopAngleRange};

			for (int i=1; i < fingerPartCount+1; i++) {
				float phaseOffset = i*growingMiddlePhaseOffset;
				float angleRange = angleRanges[i-1];

				int index = fingerParts[i];
				float theta = ofMap(sin(growingMiddleSpeedUp*t + phaseOffset), -1, 1, -(angleRange/2.0), angleRange/2.0);
				if (i % 2 == 1) theta = -theta;
				handWithFingertipsSkeleton.setRotation(index, theta, false, false);
			
			}
		//}

		for (int i=0; i < fingerPartCount; i++) {
			int index = fingerParts[i];
			ofVec2f original = immutableHandWithFingertipsSkeleton.getPositionAbsolute(index);
			ofVec2f parent = immutableHandWithFingertipsSkeleton.getPositionAbsolute(index-1);
			ofVec2f position = original - parent;
			position.normalize();
			position *= newLen*ratios[i];
			handWithFingertipsSkeleton.setPosition(index, parent, true, false);
			handWithFingertipsSkeleton.setPosition(index, position, false, false);
		}

		setSkeleton(&handWithFingertipsSkeleton);
	} else if (scene == STARTREK) {
		int toImitateTop[] = {HandSkeleton::PINKY_TIP, HandSkeleton::INDEX_TIP};
		int toImitateMid[] = {HandSkeleton::PINKY_MID, HandSkeleton::INDEX_MID};
		int toImitateBase[] = {HandSkeleton::PINKY_BASE, HandSkeleton::INDEX_BASE};
		
		int toSetMid[] = {HandSkeleton::RING_MID, HandSkeleton::MIDDLE_MID};
		int toSetBase[] = {HandSkeleton::RING_BASE, HandSkeleton::MIDDLE_BASE};
		int toSetCount = 2;
		
		ofVec2f imitateTop;
		ofVec2f imitateMid;
		ofVec2f imitateBase;
		for(int i = 0; i < toSetCount; i++) {
			ofVec2f xAxis(1, 0);
			ofVec2f yAxis(0, -1);

			imitateTop = handSkeleton.getPositionAbsolute(toImitateTop[i]);
			imitateBase = handSkeleton.getPositionAbsolute(toImitateBase[i]);
			ofVec2f dir = imitateTop - imitateBase;

			float fromY = dir.angle(yAxis);

			float angleOffset = ofMap(abs(fromY), 0, 60, 0, 25);
			if (i % 2 == 1) angleOffset = -angleOffset;
			handSkeleton.setRotation(toImitateBase[i], angleOffset, false, false);

			imitateMid = handSkeleton.getPositionAbsolute(toImitateMid[i]);
			imitateBase = handSkeleton.getPositionAbsolute(toImitateBase[i]);
			
			ofVec2f dirBase = imitateMid - imitateBase;
			float baseAngle = dirBase.angle(xAxis);

			int setBase = toSetBase[i];
			handSkeleton.setRotation(setBase, -baseAngle, true, false);

			imitateTop = handSkeleton.getPositionAbsolute(toImitateTop[i]);
			imitateMid = handSkeleton.getPositionAbsolute(toImitateMid[i]);
			
			ofVec2f dirMid = imitateTop - imitateMid;
			float midAngle = dirMid.angle(xAxis);

			int setMid = toSetMid[i];
			handSkeleton.setRotation(setMid, -midAngle, true, false);

			ofVec2f original = handSkeleton.getPositionAbsolute(setBase);
			ofVec2f position = imitateBase - original;
			position.normalize();
			position *= 35;
			handSkeleton.setPosition(toImitateBase[i], original, true, false);
			handSkeleton.setPosition(toImitateBase[i], position, false, false);
		}
		setSkeleton(&handSkeleton);
	} else if (scene == STRAIGHTEN) {
		int tip[] = {HandWithFingertipsSkeleton::PINKY_TIP, HandWithFingertipsSkeleton::RING_TIP, HandWithFingertipsSkeleton::MIDDLE_TIP, HandWithFingertipsSkeleton::INDEX_TIP, HandWithFingertipsSkeleton::THUMB_TIP};
		int top[] = {HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::INDEX_TOP, HandWithFingertipsSkeleton::THUMB_TOP};
		int mid[] = {HandWithFingertipsSkeleton::PINKY_MID,	HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::INDEX_MID, HandWithFingertipsSkeleton::THUMB_MID};
		int base[] = {HandWithFingertipsSkeleton::PINKY_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::INDEX_BASE, HandWithFingertipsSkeleton::THUMB_BASE};

		int fingerCount = 5;

		for(int i = 0; i < fingerCount; i++) {
			int fingerTip = tip[i];
			int fingerTop = top[i];
			int fingerMid = mid[i];
			int fingerBase = base[i];

			ofVec2f tipPos = handWithFingertipsSkeleton.getPositionAbsolute(fingerTip);
			ofVec2f topPos = handWithFingertipsSkeleton.getPositionAbsolute(fingerTop);
			ofVec2f midPos = handWithFingertipsSkeleton.getPositionAbsolute(fingerMid);
			ofVec2f basePos = handWithFingertipsSkeleton.getPositionAbsolute(fingerBase);

			//float bottomLen = basePos.distance(midPos);
			float middleLen = midPos.distance(topPos);
			float topLen = topPos.distance(tipPos);
			ofVec2f dir = midPos - basePos;

			//basePos = handWithFingertipsSkeleton.getPositionAbsolute(fingerBase);
			//dir.normalize();
			//dir *= bottomLen;
			//handWithFingertipsSkeleton.setPosition(fingerMid, basePos, true, false);
			//handWithFingertipsSkeleton.setPosition(fingerMid, dir, false, false);

			midPos = handWithFingertipsSkeleton.getPositionAbsolute(fingerMid);
			dir.normalize();
			dir *= middleLen;
			handWithFingertipsSkeleton.setPosition(fingerTop, midPos, true, false);
			handWithFingertipsSkeleton.setPosition(fingerTop, dir, false, false);

			topPos = handWithFingertipsSkeleton.getPositionAbsolute(fingerTop);
			dir.normalize();
			dir *= topLen;
			handWithFingertipsSkeleton.setPosition(fingerTip, topPos, true, false);
			handWithFingertipsSkeleton.setPosition(fingerTip, dir, false, false);
		}
		setSkeleton(&handWithFingertipsSkeleton);
	} else if (scene == SPLAY) {
		int palm = HandWithFingertipsSkeleton::PALM;
		ofVec2f palmPos = handWithFingertipsSkeleton.getPositionAbsolute(palm);

		if (palmPos.y < splayHeight) {
			int tip[] = {HandWithFingertipsSkeleton::PINKY_TIP, HandWithFingertipsSkeleton::RING_TIP, HandWithFingertipsSkeleton::MIDDLE_TIP, HandWithFingertipsSkeleton::INDEX_TIP, HandWithFingertipsSkeleton::THUMB_TIP};
			int top[] = {HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::INDEX_TOP, HandWithFingertipsSkeleton::THUMB_TOP};
			int mid[] = {HandWithFingertipsSkeleton::PINKY_MID,	HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::INDEX_MID, HandWithFingertipsSkeleton::THUMB_MID};
			int base[] = {HandWithFingertipsSkeleton::PINKY_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::INDEX_BASE, HandWithFingertipsSkeleton::THUMB_BASE};

			float angleOffset = ofMap(palmPos.y, splayHeight, 0, 0, splayMaxAngle);
			int fingerCount = 5;
			for (int i=0; i < fingerCount; i++) {
				int joints[] = {base[i], mid[i], top[i], tip[i]};

				ofVec2f basePos = handWithFingertipsSkeleton.getPositionAbsolute(joints[0]);
				if (basePos.x < splayAxis) angleOffset = -abs(angleOffset);
				else if (basePos.x > splayAxis) angleOffset = abs(angleOffset);
				else angleOffset = 0;

				ofVec2f positions[] = {basePos, handWithFingertipsSkeleton.getPositionAbsolute(joints[1]), handWithFingertipsSkeleton.getPositionAbsolute(joints[2]), handWithFingertipsSkeleton.getPositionAbsolute(joints[3])};
				float lengths[] = {positions[0].distance(positions[1]), positions[1].distance(positions[2]), positions[2].distance(positions[3])};
				ofVec2f dir = positions[1] - positions[0];
				dir.normalize();

				int fingerPartCount = 3;
				for(int j = 0; j < fingerPartCount; j++) {
					dir = dir.getRotated(angleOffset);
					dir.normalize();
					dir = dir * lengths[j];

					ofVec2f parent = handWithFingertipsSkeleton.getPositionAbsolute(joints[j]);

					handWithFingertipsSkeleton.setPosition(joints[j+1], parent, true, false);
					handWithFingertipsSkeleton.setPosition(joints[j+1], dir, false, false);
				}
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
	else if (scene == SPLAY) {
		ofSetColor(255);
		ofLine(0, splayHeight, 768, splayHeight);
		ofLine(splayAxis, 0, splayAxis, 1024);
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