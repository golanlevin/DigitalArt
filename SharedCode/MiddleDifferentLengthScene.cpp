#pragma once

#include "MiddleDifferentLengthScene.h"

MiddleDifferentLengthScene::MiddleDifferentLengthScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Middle Different Length", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;

	this->middleLength = 80;
	this->otherLength = 40;
}
void MiddleDifferentLengthScene::setupGui() {
	MiddleDifferentLengthScene::initializeGui();

	this->gui->addSlider("Middle Length", 0, 100, &middleLength);
	this->gui->addSpacer();
	this->gui->addSlider("Other Length", 0, 100, &otherLength);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void MiddleDifferentLengthScene::setupMouseGui() {
	MiddleDifferentLengthScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void MiddleDifferentLengthScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;
		
	int middle[] = {HandSkeleton::MIDDLE_TIP, HandSkeleton::MIDDLE_MID};
	int middleRatios[] = {1, 1.6};
	int middleCount = 2;
	for(int i = 0; i < middleCount; i++) {
		handSkeleton->setBoneLength(middle[i], middleRatios[i] * middleLength);
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
		handSkeleton->setBoneLength(others[i], otherRatios[i] * otherLength);
	}
}
void MiddleDifferentLengthScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;
	HandSkeleton* immutableHandSkeleton = (HandSkeleton*)this->immutableSkeleton;

	switch(getSelection(mouseRadio)) {
		case 0: // palm position
			handSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			immutableHandSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			break;
		case 1: // palm rotation
			ofVec2f xAxis(1, 0);

			int wrist = HandSkeleton::WRIST;
			int palm = HandSkeleton::PALM;


			ofVec2f origWristPos = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(wrist));
			ofVec2f origPalmPos = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(palm));

			ofVec2f origPalmDir = origPalmPos - origWristPos;
			
			float curRot = origPalmDir.angle(xAxis);
			float correction = 0;

			float newRot;
			if (mx <= 384) {
				newRot = ofMap(mx, 0, 384, -(curRot+correction+maxPalmAngleLeft), -(curRot+correction));
			}
			else {
				newRot = ofMap(mx, 384, 768, -(curRot+correction), -(curRot+correction+maxPalmAngleRight));
			}

			handSkeleton->setRotation(palm, newRot, true, false);
			immutableHandSkeleton->setRotation(palm, newRot, true, false);
			break;
	}
}
void MiddleDifferentLengthScene::draw() {
}