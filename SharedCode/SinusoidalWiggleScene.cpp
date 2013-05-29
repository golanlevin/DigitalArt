#pragma once

#include "SinusoidalWiggleScene.h"

SinusoidalWiggleScene::SinusoidalWiggleScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton) {
	Scene::Scene();
	Scene::setup("Sinusoidal Wiggle", puppet, (Skeleton*)handWithFingertipsSkeleton, (Skeleton*)immutableHandWithFingertipsSkeleton);

	this->angleRange = 45;
	this->speedUp = 2;
	this->phaseOffset = 0.5;
}
void SinusoidalWiggleScene::setupGui() {
	SinusoidalWiggleScene::initializeGui();

	this->gui->addSlider("Angle Range", 5, 85, &angleRange);
	this->gui->addSlider("Speed Up", 1, 5, &speedUp);
	this->gui->addSlider("Phase Offset", 0, 1, &phaseOffset);
	this->baseJoint = this->gui->addToggle("Base", true);
	this->midJoint = this->gui->addToggle("Mid", false);
	this->topJoint = this->gui->addToggle("Top", false);
	
	this->gui->autoSizeToFitWidgets();
}
void SinusoidalWiggleScene::setupMouseGui() {
	SinusoidalWiggleScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);

	this->mouseGui->autoSizeToFitWidgets();
}
void SinusoidalWiggleScene::update() {
	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;
	
	int base[] = {HandWithFingertipsSkeleton::PINKY_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::INDEX_BASE, HandWithFingertipsSkeleton::THUMB_BASE};
	int mid[] = {HandWithFingertipsSkeleton::PINKY_MID, HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::INDEX_MID, HandWithFingertipsSkeleton::THUMB_MID};
	int top[] = {HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::INDEX_TOP, HandWithFingertipsSkeleton::THUMB_TOP};
			
	vector<int*> toWiggle;
	if (baseJoint->getValue()) toWiggle.push_back(base);
	if (midJoint->getValue()) toWiggle.push_back(mid);
	if (topJoint->getValue()) toWiggle.push_back(top);

	int toWiggleCount = 5;
	for (int i=0; i < toWiggle.size(); i++) {
		int* currentWiggle = toWiggle[i];
		for (int j=0; j < toWiggleCount; j++) {
			float offset = j*phaseOffset + j;

			int index = currentWiggle[j];
			float theta = ofMap(sin(speedUp*ofGetElapsedTimef() + offset), -1, 1, -(angleRange/2.0), angleRange/2.0);
			handWithFingertipsSkeleton->setRotation(index, theta, false, false);
		}
	}
}
void SinusoidalWiggleScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;
	HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->immutableSkeleton;

	switch(getSelection(mouseRadio)) {
		case 0: // palm position
			handWithFingertipsSkeleton->setPosition(HandWithFingertipsSkeleton::PALM, mouse, true);
			immutableHandWithFingertipsSkeleton->setPosition(HandWithFingertipsSkeleton::PALM, mouse, true);
			break;
		case 1: // palm rotation
			break;
	}
}
void SinusoidalWiggleScene::draw() {
}