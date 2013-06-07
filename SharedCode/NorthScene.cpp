#pragma once

#include "NorthScene.h"

NorthScene::NorthScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("North", "North (Hand)", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;
	this->maxBaseAngleLeft = 20;
	this->maxBaseAngleRight = -20;
	this->maxMidAngleLeft = 45;
	this->maxMidAngleRight = -30;
}
void NorthScene::setupGui() {
	NorthScene::initializeGui();

	this->gui->autoSizeToFitWidgets();
}
void NorthScene::setupMouseGui() {
	NorthScene::initializeMouseGui();

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
void NorthScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;

	// get the original positions 
	ofVec2f origPBasePos      = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(HandSkeleton::PINKY_BASE));
	ofVec2f origRBasePos      = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(HandSkeleton::RING_BASE));
	ofVec2f origMBasePos      = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(HandSkeleton::MIDDLE_BASE));
	ofVec2f origIBasePos      = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(HandSkeleton::INDEX_BASE));
	
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

	int toRotate[] = {HandSkeleton::PINKY_BASE, HandSkeleton::RING_BASE, HandSkeleton::MIDDLE_BASE, HandSkeleton::INDEX_BASE,
		HandSkeleton::PINKY_MID, HandSkeleton::RING_MID, HandSkeleton::MIDDLE_MID, HandSkeleton::INDEX_MID};
	int toRotateCount = 8;
	for(int i = 0; i < toRotateCount; i++) {
		int index = toRotate[i];
		handSkeleton->setRotation(index, -90, true);
	}
}
void NorthScene::updateMouse(float mx, float my) {
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

	switch(getSelection(mouseRadio)) {
		case 0: // palm position
			handSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			immutableHandSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			break;
		case 1: // palm rotation
			origPalmDir = origPalmPos - origWristPos;
			
			curRot = origPalmDir.angle(xAxis);

			newRot;
			if (mx <= 384) {
				newRot = ofMap(mx, 0, 384, -(curRot+maxPalmAngleLeft), -(curRot));
			}
			else {
				newRot = ofMap(mx, 384, 768, -(curRot), -(curRot+maxPalmAngleRight));
			}

			handSkeleton->setRotation(palm, newRot, true, false);
			immutableHandSkeleton->setRotation(palm, newRot, true, false);
			break;
		case 2: // finger base rotation
			for (int i=0; i < fingerCount; i++) {
				origFingerDir = origMidPos[i] - origBasePos[i];
				curRot = origFingerDir.angle(xAxis);

				if (mx <= 384) {
					newRot = ofMap(mx, 0, 384, -(curRot+maxBaseAngleLeft), -(curRot));
				}
				else {
					newRot = ofMap(mx, 384, 768, -(curRot), -(curRot+maxBaseAngleRight));
				}

				handSkeleton->setRotation(base[i], newRot, true, false);
				immutableHandSkeleton->setRotation(base[i], newRot, true, false);
			}
			break;
		case 3: // finger mid rotation
			for (int i=0; i < fingerCount; i++) {
				origFingerDir = origTopPos[i] - origMidPos[i];
				curRot = origFingerDir.angle(xAxis);

				if (mx <= 384) {
					newRot = ofMap(mx, 0, 384, -(curRot+maxMidAngleLeft), -(curRot));
				}
				else {
					newRot = ofMap(mx, 384, 768, -(curRot), -(curRot+maxMidAngleRight));
				}

				handSkeleton->setRotation(mid[i], newRot, true, false);
				immutableHandSkeleton->setRotation(mid[i], newRot, true, false);
			}
			break;
	}
}
void NorthScene::draw() {
}
