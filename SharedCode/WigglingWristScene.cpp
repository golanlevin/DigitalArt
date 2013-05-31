#pragma once

#include "WigglingWristScene.h"

WigglingWristScene::WigglingWristScene(ofxPuppet* puppet, WristSpineSkeleton* wristSpineSkeleton, WristSpineSkeleton* immutableWristSpineSkeleton) {
	Scene::Scene();
	Scene::setup("Wiggling Wrist", "Wiggling Wrist (Wrist Spine)", puppet, (Skeleton*)wristSpineSkeleton, (Skeleton*)immutableWristSpineSkeleton);

	this->maxPalmAngleLeft = 30;
	this->maxPalmAngleRight = -30;

	this->palmAngleRange = 45;
	this->topAngleRange = 30;
	this->midAngleRange = 15;
	this->speedUp = 2;
	this->phaseOffset = 0.5;
}
void WigglingWristScene::setupGui() {
	WigglingWristScene::initializeGui();

	this->gui->addSlider("Palm Angle Range", 10, 60, &palmAngleRange);
	this->gui->addSpacer();
	this->gui->addSlider("Top Angle Range", 10, 60, &topAngleRange);
	this->gui->addSpacer();
	this->gui->addSlider("Mid Angle Range", 10, 60, &midAngleRange);
	this->gui->addSpacer();
	this->gui->addSlider("Speed Up", 1, 5, &speedUp);
	this->gui->addSpacer();
	this->gui->addSlider("Phase Offset", 0, 1, &phaseOffset);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void WigglingWristScene::setupMouseGui() {
	WigglingWristScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void WigglingWristScene::update() {
	WristSpineSkeleton* wristSpineSkeleton = (WristSpineSkeleton*)this->skeleton;

	int index;
	ofVec2f original;
	float theta;

	float palmOffset = 0*phaseOffset;
	float topOffset = 1*phaseOffset;
	float midOffset = 2*phaseOffset;

	index = WristSpineSkeleton::PALM;
	theta = ofMap(sin(speedUp*ofGetElapsedTimef() + palmOffset), -1, 1, -(palmAngleRange/2.0), palmAngleRange/2.0);
	wristSpineSkeleton->setRotation(index, theta, false, false);
			
	index = WristSpineSkeleton::WRIST_TOP;
	theta = -ofMap(sin(speedUp*ofGetElapsedTimef() + topOffset), -1, 1, -(topAngleRange/2.0), topAngleRange/2.0);
	wristSpineSkeleton->setRotation(index, theta, false, false);

	index = WristSpineSkeleton::WRIST_MID;
	theta = ofMap(sin(speedUp*ofGetElapsedTimef() + midOffset), -1, 1, -(midAngleRange/2.0), midAngleRange/2.0);
	wristSpineSkeleton->setRotation(index, theta, false , false);
}
void WigglingWristScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	WristSpineSkeleton* wristSpineSkeleton = (WristSpineSkeleton*)this->skeleton;
	WristSpineSkeleton* immutableWristSpineSkeleton = (WristSpineSkeleton*)this->immutableSkeleton;

	switch(getSelection(mouseRadio)) {
		case 0: // palm position
			wristSpineSkeleton->setPosition(WristSpineSkeleton::HAND_TOP, mouse, true);
			immutableWristSpineSkeleton->setPosition(WristSpineSkeleton::HAND_TOP, mouse, true);
			break;
		case 1: // palm rotation
			ofVec2f xAxis(1, 0);

			int wrist = WristSpineSkeleton::HAND_MID;
			int palm = WristSpineSkeleton::HAND_BASE;

			ofVec2f origWristPos = puppet->getOriginalMesh().getVertex(wristSpineSkeleton->getControlIndex(wrist));
			ofVec2f origPalmPos = puppet->getOriginalMesh().getVertex(wristSpineSkeleton->getControlIndex(palm));

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

			wristSpineSkeleton->setRotation(palm, newRot, true, false);
			immutableWristSpineSkeleton->setRotation(palm, newRot, true, false);
			break;
	}
}
void WigglingWristScene::draw() {
}
