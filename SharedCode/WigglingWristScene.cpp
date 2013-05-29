#pragma once

#include "WigglingWristScene.h"

WigglingWristScene::WigglingWristScene(ofxPuppet* puppet, WristSpineSkeleton* wristSpineSkeleton, WristSpineSkeleton* immutableWristSpineSkeleton) {
	Scene::Scene();
	Scene::setup("Wiggling Wrist", puppet, (Skeleton*)wristSpineSkeleton, (Skeleton*)immutableWristSpineSkeleton);

	this->palmAngleRange = 45;
	this->topAngleRange = 30;
	this->midAngleRange = 15;
	this->speedUp = 2;
	this->phaseOffset = 0.5;
}
void WigglingWristScene::setupGui() {
	WigglingWristScene::initializeGui();

	this->gui->addSlider("Palm Angle Range", 10, 60, &palmAngleRange);
	this->gui->addSlider("Top Angle Range", 10, 60, &topAngleRange);
	this->gui->addSlider("Mid Angle Range", 10, 60, &midAngleRange);
	this->gui->addSlider("Speed Up", 1, 5, &speedUp);
	this->gui->addSlider("Phase Offset", 0, 1, &phaseOffset);

	this->gui->autoSizeToFitWidgets();
}
void WigglingWristScene::setupMouseGui() {
	WigglingWristScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);

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
			break;
	}
}
void WigglingWristScene::draw() {
}
