#pragma once

#include "SinusoidalWiggleScene.h"

SinusoidalWiggleScene::SinusoidalWiggleScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton) {
	Scene::Scene();
	Scene::setup("Sinusoidal Wiggle", puppet, (Skeleton*)handWithFingertipsSkeleton, (Skeleton*)immutableHandWithFingertipsSkeleton);

	this->angleRange = 45;
	this->speedUp = 2;
	this->phaseOffset = 0.5;
	this->wiggleJoints.push_back("Base");
	this->wiggleJoints.push_back("Mid");
	this->wiggleJoints.push_back("Top");
}
void SinusoidalWiggleScene::setupGui() {
	SinusoidalWiggleScene::initializeGui();

	this->gui->addSlider("Angle Range", 5, 85, &angleRange);
	this->gui->addSlider("Speed Up", 1, 5, &speedUp);
	this->gui->addSlider("Phase Offset", 0, 1, &phaseOffset);
	this->jointsRadio = this->gui->addRadio("Wiggle Joint Options", wiggleJoints);
	this->jointsRadio->getToggles()[0]->setValue(true);
}
void SinusoidalWiggleScene::setupMouseGui() {
	SinusoidalWiggleScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
}
void SinusoidalWiggleScene::update() {
	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;
	
	int base[] = {HandWithFingertipsSkeleton::PINKY_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::INDEX_BASE, HandWithFingertipsSkeleton::THUMB_BASE};
	int mid[] = {HandWithFingertipsSkeleton::PINKY_MID, HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::INDEX_MID, HandWithFingertipsSkeleton::THUMB_MID};
	int top[] = {HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::INDEX_TOP, HandWithFingertipsSkeleton::THUMB_TOP};
			
	int* toWiggle;
	int toWiggleCount = 5;
	switch(getSelection(jointsRadio)) {
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
		float offset = i*phaseOffset + i;

		int index = toWiggle[i];
		float theta = ofMap(sin(speedUp*ofGetElapsedTimef() + offset), -1, 1, -(angleRange/2.0), angleRange/2.0);
		handWithFingertipsSkeleton->setRotation(index, theta, false, false);
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