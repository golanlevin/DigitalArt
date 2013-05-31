#pragma once

#include "GrowingMiddleFingerScene.h"

GrowingMiddleFingerScene::GrowingMiddleFingerScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton) {
	Scene::Scene();
	Scene::setup("Growing Middle Finger", "Growing Middle Finger (Hand With Fingertips)", puppet, (Skeleton*)handWithFingertipsSkeleton, (Skeleton*)immutableHandWithFingertipsSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;
	this->maxBaseAngleLeft = 20;
	this->maxBaseAngleRight = -20;
	this->maxMidAngleLeft = 45;
	this->maxMidAngleRight = -30;
	this->maxTopAngleLeft = 30;
	this->maxTopAngleRight = -20;

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
	mouseOptions.push_back("Finger Base Rotation");
	mouseOptions.push_back("Finger Mid Rotation");
	mouseOptions.push_back("Finger Top Rotation");
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

	ofVec2f xAxis(1, 0);

	const int fingerCount = 5;

	int wrist = HandWithFingertipsSkeleton::WRIST;
	int palm = HandWithFingertipsSkeleton::PALM;
	int base[] = {HandWithFingertipsSkeleton::THUMB_BASE, HandWithFingertipsSkeleton::INDEX_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::PINKY_BASE};
	int mid[] = {HandWithFingertipsSkeleton::THUMB_MID, HandWithFingertipsSkeleton::INDEX_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::PINKY_MID};
	int top[] = {HandWithFingertipsSkeleton::THUMB_TOP, HandWithFingertipsSkeleton::INDEX_TOP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::PINKY_TOP};

	ofVec2f origWristPos = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(wrist));
	ofVec2f origPalmPos = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(palm));
	ofVec2f origBasePos[fingerCount]; 
	ofVec2f origMidPos[fingerCount]; 
	ofVec2f origTopPos[fingerCount]; 
	for (int i=0; i < fingerCount; i++) {
		origBasePos[i] = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(base[i]));
		origMidPos[i] = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(mid[i]));
		origTopPos[i] = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(top[i]));
	}

	ofVec2f origPalmDir;
	ofVec2f origFingerDir;
	float curRot;
	float newRot;

	float correction = 0;
	float baseCorrection[] = {26.75, -3, 1.75, 7.75, 9.75};
	float midCorrection[] = {6.75, 2, -1.5, -1.75, -3.5};
	float topCorrection[] = {-16, 3, 3.5, 2.25, 0.5};

	switch(getSelection(mouseRadio)) {
		case 0: // palm position
			handWithFingertipsSkeleton->setPosition(HandWithFingertipsSkeleton::PALM, mouse, true);
			immutableHandWithFingertipsSkeleton->setPosition(HandWithFingertipsSkeleton::PALM, mouse, true);
			break;
		case 1: // palm rotation

			origPalmDir = origPalmPos - origWristPos;
			
			curRot = origPalmDir.angle(xAxis);

			newRot;
			if (mx <= 384) {
				newRot = ofMap(mx, 0, 384, -(curRot+correction+maxPalmAngleLeft), -(curRot+correction));
			}
			else {
				newRot = ofMap(mx, 384, 768, -(curRot+correction), -(curRot+correction+maxPalmAngleRight));
			}

			handWithFingertipsSkeleton->setRotation(palm, newRot, true, false);
			immutableHandWithFingertipsSkeleton->setRotation(palm, newRot, true, false);
			break;
		case 2: // finger base rotation
			for (int i=0; i < fingerCount; i++) {
				origFingerDir = origBasePos[i] - origPalmPos;
				curRot = origFingerDir.angle(xAxis);

				if (mx <= 384) {
					newRot = ofMap(mx, 0, 384, -(curRot+baseCorrection[i]+maxBaseAngleLeft), -(curRot+baseCorrection[i]));
				}
				else {
					newRot = ofMap(mx, 384, 768, -(curRot+baseCorrection[i]), -(curRot+baseCorrection[i]+maxBaseAngleRight));
				}

				handWithFingertipsSkeleton->setRotation(base[i], newRot, true, false);
				immutableHandWithFingertipsSkeleton->setRotation(base[i], newRot, true, false);
			}
			break;
		case 3: // finger mid rotation
			for (int i=0; i < fingerCount; i++) {
				origFingerDir = origMidPos[i] - origBasePos[i];
				curRot = origFingerDir.angle(xAxis);

				if (mx <= 384) {
					newRot = ofMap(mx, 0, 384, -(curRot+midCorrection[i]+maxMidAngleLeft), -(curRot+midCorrection[i]));
				}
				else {
					newRot = ofMap(mx, 384, 768, -(curRot+midCorrection[i]), -(curRot+midCorrection[i]+maxMidAngleRight));
				}

				handWithFingertipsSkeleton->setRotation(mid[i], newRot, true, false);
				immutableHandWithFingertipsSkeleton->setRotation(mid[i], newRot, true, false);
			}
			break;
		case 4: // finger top rotation
			for (int i=0; i < fingerCount; i++) {
				origFingerDir = origTopPos[i] - origMidPos[i];
				curRot = origFingerDir.angle(xAxis);

				if (mx <= 384) {
					newRot = ofMap(mx, 0, 384, -(curRot+topCorrection[i]+maxTopAngleLeft), -(curRot+topCorrection[i]));
				}
				else {
					newRot = ofMap(mx, 384, 768, -(curRot+topCorrection[i]), -(curRot+topCorrection[i]+maxTopAngleRight));
				}

				handWithFingertipsSkeleton->setRotation(top[i], newRot, true, false);
				immutableHandWithFingertipsSkeleton->setRotation(top[i], newRot, true, false);
			}
			break;
	}
}
void GrowingMiddleFingerScene::draw() {
}