#pragma once

#include "WobbleScene.h"

//==========================================================================
WobbleScene::WobbleScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Wobble", "Wobble (Hand)", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);

	this->maxPalmAngleLeft  =  60;
	this->maxPalmAngleRight = -60;
	this->maxBaseAngleLeft  =  20;
	this->maxBaseAngleRight = -20;
	this->maxMidAngleLeft   =  45;
	this->maxMidAngleRight  = -30;
}

//==========================================================================
void WobbleScene::setupGui() {
	WobbleScene::initializeGui();
	this->gui->autoSizeToFitWidgets();
}

//==========================================================================
void WobbleScene::setupMouseGui() {
	WobbleScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	mouseOptions.push_back("Finger Base Rotation");
	mouseOptions.push_back("Finger Mid Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}

//==========================================================================
void WobbleScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;
	
	float timeVal = ofGetElapsedTimef();
	if (bUseFrameBasedAnimation){
		timeVal = (float)ofGetFrameNum()/ 60.0;
	}

	float wobbleRange      = 5;
	float palmNoiseVal0 = ofNoise (0.03 * timeVal, 0);
	float palmNoiseVal1 = ofNoise (0.03 * timeVal, 1);
	float palmDx = ofMap(palmNoiseVal0,  0,1, -1,1);
	float palmDy = ofMap(palmNoiseVal1,  0,1, -1,1);
	
	// get the original positions 
	ofVec2f origPalmPos       = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(HandSkeleton::PALM));
	ofVec2f origThumbBasePos  = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(HandSkeleton::THUMB_BASE));
	ofVec2f origPBasePos      = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(HandSkeleton::PINKY_BASE));
	ofVec2f origRBasePos      = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(HandSkeleton::RING_BASE));
	ofVec2f origMBasePos      = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(HandSkeleton::MIDDLE_BASE));
	ofVec2f origIBasePos      = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(HandSkeleton::INDEX_BASE));
	
	// move the palm
	ofVec2f palmPosition (wobbleRange * ofVec2f (palmDx, palmDy ));
	handSkeleton->setPosition(HandSkeleton::PALM, palmPosition, false, true);
	ofVec2f newPalmPos = origPalmPos + palmPosition;
	
	ofVec2f vecFromThumbBaseToNewPalm = newPalmPos - origThumbBasePos;
	handSkeleton->setPosition(HandSkeleton::THUMB_BASE, -0.5 * vecFromThumbBaseToNewPalm, false, false);
	
	
	ofVec2f  RMI = (origRBasePos + origMBasePos + origIBasePos) / 3.0;
	ofVec2f  PMI = (origPBasePos + origMBasePos + origIBasePos) / 3.0;
	ofVec2f  PRI = (origPBasePos + origRBasePos + origIBasePos) / 3.0;
	ofVec2f  PRM = (origPBasePos + origRBasePos + origMBasePos) / 3.0;
	
	ofVec2f dRMI = RMI - origPBasePos;
	ofVec2f dPMI = PMI - origRBasePos;
	ofVec2f dPRI = PRI - origMBasePos;
	ofVec2f dPRM = PRM - origIBasePos;
	
	float baseMoveAmount = -0.25;
	ofVec2f newP = baseMoveAmount * dRMI;
	ofVec2f newR = baseMoveAmount * dPMI;
	ofVec2f newM = baseMoveAmount * dPRI;
	ofVec2f newI = baseMoveAmount * dPRM;
	
	handSkeleton->setPosition(HandSkeleton::PINKY_BASE,  newP, false, false);
	handSkeleton->setPosition(HandSkeleton::RING_BASE,   newR, false, false);
	handSkeleton->setPosition(HandSkeleton::MIDDLE_BASE, newM, false, false);
	handSkeleton->setPosition(HandSkeleton::INDEX_BASE,  newI, false, false);
	 
	
	// printf ("%f	%f\n", origPalmPos.x, newPalmPos.x);
	// ofVec2f thumbBasePosition (0.5 * wobbleRange * ofVec2f (palmDx, palmDy ));
	// handSkeleton->setPosition(HandSkeleton::THUMB_BASE, thumbBasePosition, false, false);
}


//==========================================================================
void WobbleScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;
	HandSkeleton* immutableHandSkeleton = (HandSkeleton*)this->immutableSkeleton;

	ofVec2f xAxis(1, 0);

	const int fingerCount = 5;

	int wrist = HandSkeleton::WRIST;
	int palm = HandSkeleton::PALM;
	int base[] = {HandSkeleton::THUMB_BASE, HandSkeleton::INDEX_BASE, HandSkeleton::MIDDLE_BASE, HandSkeleton::RING_BASE, HandSkeleton::PINKY_BASE};
	int mid[] = {HandSkeleton::THUMB_MID, HandSkeleton::INDEX_MID, HandSkeleton::MIDDLE_MID, HandSkeleton::RING_MID, HandSkeleton::PINKY_MID};
	int top[] = {HandSkeleton::THUMB_TIP, HandSkeleton::INDEX_TIP, HandSkeleton::MIDDLE_TIP, HandSkeleton::RING_TIP, HandSkeleton::PINKY_TIP};

	ofVec2f origWristPos = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(wrist));
	ofVec2f origPalmPos = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(palm));
	ofVec2f origBasePos[fingerCount]; 
	ofVec2f origMidPos[fingerCount]; 
	ofVec2f origTopPos[fingerCount]; 
	for (int i=0; i < fingerCount; i++) {
		origBasePos[i] = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(base[i]));
		origMidPos[i] = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(mid[i]));
		origTopPos[i] = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(top[i]));
	}

	ofVec2f origPalmDir;
	ofVec2f origFingerDir;
	float curRot;
	float newRot;

	float correction = 0;
	float baseCorrection[] = {26.75, -3, 1.75, 7.75, 9.75};
	float midCorrection[] = {6.75, 2, -1.5, -1.75, -3.5};

	switch (getSelection(mouseRadio)) {
		case 0: // palm position
			handSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			immutableHandSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			break;
		case 1: // palm rotation
			origPalmDir = origPalmPos - origWristPos;
			
			curRot = origPalmDir.angle(xAxis);

			newRot;
			if (mx <= 384) {
				newRot = ofMap(mx, 0, 384, -(curRot+correction+maxPalmAngleLeft), -(curRot+correction));
			}
			else {
				newRot = ofMap(mx, 384, 768, -(curRot+correction), -(curRot+correction+maxPalmAngleRight));
			}

			handSkeleton->setRotation(palm, newRot, true, false);
			immutableHandSkeleton->setRotation(palm, newRot, true, false);
			break;
		case 2: // finger base rotation
			for (int i=0; i < fingerCount; i++) {
				origFingerDir = origBasePos[i] - origPalmPos;
				curRot = origFingerDir.angle(xAxis);

				if (mx <= 384) {
					newRot = ofMap(mx, 0, 384, -(curRot+baseCorrection[i]+maxBaseAngleLeft), -(curRot+baseCorrection[i]));
				}
				else {
					newRot = ofMap(mx, 384, 768, -(curRot+baseCorrection[i]), -(curRot+baseCorrection[i]+maxBaseAngleRight));
				}

				handSkeleton->setRotation(base[i], newRot, true, false);
				immutableHandSkeleton->setRotation(base[i], newRot, true, false);
			}
			break;
		case 3: // finger mid rotation
			for (int i=0; i < fingerCount; i++) {
				origFingerDir = origMidPos[i] - origBasePos[i];
				curRot = origFingerDir.angle(xAxis);

				if (mx <= 384) {
					newRot = ofMap(mx, 0, 384, -(curRot+midCorrection[i]+maxMidAngleLeft), -(curRot+midCorrection[i]));
				}
				else {
					newRot = ofMap(mx, 384, 768, -(curRot+midCorrection[i]), -(curRot+midCorrection[i]+maxMidAngleRight));
				}

				handSkeleton->setRotation(mid[i], newRot, true, false);
				immutableHandSkeleton->setRotation(mid[i], newRot, true, false);
			}
			break;
	}
}

//==========================================================================
void WobbleScene::draw() {
}