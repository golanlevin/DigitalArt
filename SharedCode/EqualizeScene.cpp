#pragma once

#include "EqualizeScene.h"

EqualizeScene::EqualizeScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Equalize", "Equalize (Hand)", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;
	this->maxBaseAngleLeft = 20;
	this->maxBaseAngleRight = -20;
	this->maxMidAngleLeft = 45;
	this->maxMidAngleRight = -30;

	this->equalizeLength = 60;
}
void EqualizeScene::setupGui() {
	EqualizeScene::initializeGui();

	this->gui->addSlider("Equalize Length", 0, 200, &equalizeLength);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void EqualizeScene::setupMouseGui() {
	EqualizeScene::initializeMouseGui();

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
void EqualizeScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;

	int toEqualize[] = {
	HandSkeleton::PINKY_TIP, HandSkeleton::RING_TIP, HandSkeleton::MIDDLE_TIP, HandSkeleton::INDEX_TIP,
	HandSkeleton::PINKY_MID, HandSkeleton::RING_MID, HandSkeleton::MIDDLE_MID, HandSkeleton::INDEX_MID};
	float ratios[] = {
		1, 1, 1, 1,
		1.6, 1.6, 1.6, 1.6
	};
	int toEqualizeCount = 8;
	for(int i = 0; i < toEqualizeCount; i++) {
		handSkeleton->setBoneLength(toEqualize[i], ratios[i] * equalizeLength);
	}
	ofVec2f pinkyBase = handSkeleton->getPositionAbsolute(HandSkeleton::PINKY_BASE);
	ofVec2f indexBase = handSkeleton->getPositionAbsolute(HandSkeleton::INDEX_BASE);
	handSkeleton->setPosition(HandSkeleton::RING_BASE, pinkyBase.getInterpolated(indexBase, 1/3.), true);
	handSkeleton->setPosition(HandSkeleton::MIDDLE_BASE, pinkyBase.getInterpolated(indexBase, 2/3.), true);
}
void EqualizeScene::updateMouse(float mx, float my) {
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
void EqualizeScene::draw() {
}