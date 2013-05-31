#pragma once

#include "GrowingMiddleFingerScene.h"

GrowingMiddleFingerScene::GrowingMiddleFingerScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton) {
	Scene::Scene();
	Scene::setup("Growing Middle Finger", puppet, (Skeleton*)handWithFingertipsSkeleton, (Skeleton*)immutableHandWithFingertipsSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;

	this->baseAngleRange = 60;
	this->midAngleRange = 30;
	this->topAngleRange = 15;
	this->maxLen = 225;
	this->growthAmount = 1.2;
	this->speedUp = 2;
	this->phaseOffset = 0.5;
}
void GrowingMiddleFingerScene::setupGui() {
	GrowingMiddleFingerScene::initializeGui();

	this->gui->addSlider("Base Angle Range", 10, 90, &baseAngleRange);
	this->gui->addSpacer();
	this->gui->addSlider("Mid Angle Range", 10, 90, &midAngleRange);
	this->gui->addSpacer();
	this->gui->addSlider("Top Angle Range", 10, 90, &topAngleRange);
	this->gui->addSpacer();
	this->gui->addSlider("Max Length", 200, 300, &maxLen);
	this->gui->addSpacer();
	this->gui->addSlider("Growth Amount", 1, 2, &growthAmount);
	this->gui->addSpacer();
	this->gui->addSlider("Speed Up", 1, 10, &speedUp);
	this->gui->addSpacer();
	this->gui->addSlider("Phase Offset", 0, 1, &phaseOffset);
	this->gui->addSpacer();
	this->gui->addLabel("Wiggle After Done Growing", 2);
	this->keepWiggling = this->gui->addToggle("Keep Wiggling", false);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void GrowingMiddleFingerScene::setupMouseGui() {
	GrowingMiddleFingerScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void GrowingMiddleFingerScene::update() {
	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;
	HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->immutableSkeleton;

	int fingerParts[] = {HandWithFingertipsSkeleton::MIDDLE_TIP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::MIDDLE_BASE};
	float ratios[] = {0.2, 0.3, 0.5};
	float fingerPartCount = 3;

	float t = ofGetElapsedTimef() - getBeginSceneTime();

	ofVec2f fingerBase = immutableHandWithFingertipsSkeleton->getPositionAbsolute(HandWithFingertipsSkeleton::MIDDLE_BASE);
	ofVec2f fingerTip = immutableHandWithFingertipsSkeleton->getPositionAbsolute(HandWithFingertipsSkeleton::MIDDLE_TIP);
	float len = fingerBase.distance(fingerTip);
	float newLen = len + t*growthAmount;

	if (newLen > maxLen) {
		newLen = maxLen;
	}

	if ((newLen < maxLen) || keepWiggling->getValue()) {
		float angleRanges[] = {baseAngleRange, midAngleRange, topAngleRange};

		for (int i=1; i < fingerPartCount+1; i++) {
			float offset = i*phaseOffset;
			float angleRange = angleRanges[i-1];

			int index = fingerParts[i];
			float theta = ofMap(sin(speedUp*t + offset), -1, 1, -(angleRange/2.0), angleRange/2.0);
			if (i % 2 == 1) theta = -theta;
			handWithFingertipsSkeleton->setRotation(index, theta, false, false);
			
		}
	}

	for (int i=0; i < fingerPartCount; i++) {
		int index = fingerParts[i];
		ofVec2f original = immutableHandWithFingertipsSkeleton->getPositionAbsolute(index);
		ofVec2f parent = immutableHandWithFingertipsSkeleton->getPositionAbsolute(index-1);
		ofVec2f position = original - parent;
		position.normalize();
		position *= newLen*ratios[i];
		handWithFingertipsSkeleton->setPosition(index, parent, true, false);
		handWithFingertipsSkeleton->setPosition(index, position, false, false);
	}
}
void GrowingMiddleFingerScene::updateMouse(float mx, float my) {
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

			handWithFingertipsSkeleton->setRotation(palm, newRot, true, false);
			immutableHandWithFingertipsSkeleton->setRotation(palm, newRot, true, false);
			break;
	}
}
void GrowingMiddleFingerScene::draw() {
}