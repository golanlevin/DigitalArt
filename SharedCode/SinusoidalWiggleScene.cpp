#pragma once

#include "SinusoidalWiggleScene.h"

//==========================================================================
SinusoidalWiggleScene::SinusoidalWiggleScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton) {
	Scene::Scene();
	Scene::setup("Sinusoidal Wiggle", "Sinusoidal Wiggle (Hand With Fingertips)", puppet, (Skeleton*)handWithFingertipsSkeleton, (Skeleton*)immutableHandWithFingertipsSkeleton);

	this->maxPalmAngleLeft  =  60;
	this->maxPalmAngleRight = -60;

	this->angleRange        = 25;
	this->speedUp           = 1.7;
	this->phaseOffset       = 0.5;
	
}

//==========================================================================
void SinusoidalWiggleScene::setupGui() {
	SinusoidalWiggleScene::initializeGui();

	this->gui->addSlider("Angle Range", 5, 85, &angleRange);
	this->gui->addSpacer();
	this->gui->addSlider("Speed Up", 1, 5, &speedUp);
	this->gui->addSpacer();
	this->gui->addSlider("Phase Offset", 0, 1, &phaseOffset);
	this->gui->addSpacer();
	this->gui->addLabel("Joints to Wiggle", 2);
	this->baseJoint = this->gui->addToggle("Base", false);
	this->midJoint  = this->gui->addToggle("Mid",  true);
	this->topJoint  = this->gui->addToggle("Top",  true);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}

//==========================================================================
void SinusoidalWiggleScene::setupMouseGui() {
	SinusoidalWiggleScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}

//==========================================================================
void SinusoidalWiggleScene::update() {
	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;
	
	int base[] = {
		HandWithFingertipsSkeleton::PINKY_BASE,
		HandWithFingertipsSkeleton::RING_BASE,
		HandWithFingertipsSkeleton::MIDDLE_BASE,
		HandWithFingertipsSkeleton::INDEX_BASE,
		HandWithFingertipsSkeleton::THUMB_BASE};
	
	int mid[] = {
		HandWithFingertipsSkeleton::PINKY_MID,
		HandWithFingertipsSkeleton::RING_MID,
		HandWithFingertipsSkeleton::MIDDLE_MID,
		HandWithFingertipsSkeleton::INDEX_MID,
		HandWithFingertipsSkeleton::THUMB_MID};
	
	int top[] = {
		HandWithFingertipsSkeleton::PINKY_TOP,
		HandWithFingertipsSkeleton::RING_TOP,
		HandWithFingertipsSkeleton::MIDDLE_TOP,
		HandWithFingertipsSkeleton::INDEX_TOP,
		HandWithFingertipsSkeleton::THUMB_TOP};
			
	vector<int*> toWiggle;
	if (baseJoint->getValue()) toWiggle.push_back(base);
	if (midJoint->getValue())  toWiggle.push_back(mid);
	if (topJoint->getValue())  toWiggle.push_back(top);
	
	
	float timeVal = ofGetElapsedTimef();
	if (bUseFrameBasedAnimation){
		timeVal = (float)ofGetFrameNum()/ 60.0;
	}

	int toWiggleCount = 5;
	for (int i=0; i < toWiggle.size(); i++) {
		int* currentWiggle = toWiggle[i];
		for (int j=0; j < toWiggleCount; j++) {
			float offset = j*phaseOffset + j;

			int index = currentWiggle[j];
			float sinValue = sin(speedUp*timeVal + offset);
			float theta = ofMap(sinValue, -1, 1, -(angleRange/2.0), angleRange/2.0);
			handWithFingertipsSkeleton->setRotation(index, theta, false, false);
		}
	}
}

//==========================================================================
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
			ofVec2f xAxis(1, 0);

			int wrist = HandWithFingertipsSkeleton::WRIST;
			int palm = HandWithFingertipsSkeleton::PALM;


			ofVec2f origWristPos = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(wrist));
			ofVec2f origPalmPos = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(palm));

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

			handWithFingertipsSkeleton->setRotation         (palm, newRot, true, false);
			immutableHandWithFingertipsSkeleton->setRotation(palm, newRot, true, false);
			break;
	}
}

//==========================================================================
void SinusoidalWiggleScene::draw() {
	
}