#pragma once

#include "EqualizeScene.h"

EqualizeScene::EqualizeScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Equalize", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;

	this->equalizeLength = 40;
}
void EqualizeScene::setupGui() {
	EqualizeScene::initializeGui();

	this->gui->addSlider("Equalize Length", 0, 100, &equalizeLength);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void EqualizeScene::setupMouseGui() {
	EqualizeScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
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
void EqualizeScene::draw() {
}