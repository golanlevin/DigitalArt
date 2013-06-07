#pragma once

#include "StraightenFingersScene.h"

StraightenFingersScene::StraightenFingersScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton) {
	Scene::Scene();
	Scene::setup("Straigthen Fingers", "Straighten Fingers (Hand With Fingertips)", puppet, (Skeleton*)handWithFingertipsSkeleton, (Skeleton*)immutableHandWithFingertipsSkeleton);

	this->startShowSkeleton = true;
	this->startMouseControl = true;

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;
	this->maxBaseAngleLeft = 20;
	this->maxBaseAngleRight = -20;
	this->maxMidAngleLeft = 45;
	this->maxMidAngleRight = -30;
	this->maxTopAngleLeft = 30;
	this->maxTopAngleRight = -20;
}
void StraightenFingersScene::setupGui() {
	StraightenFingersScene::initializeGui();

	this->gui->autoSizeToFitWidgets();
}
void StraightenFingersScene::setupMouseGui() {
	StraightenFingersScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	mouseOptions.push_back("Finger Base Rotation");
	mouseOptions.push_back("Finger Mid Rotation");
	mouseOptions.push_back("Finger Top Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[2]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void StraightenFingersScene::update() {
	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;

	int tip[] = {HandWithFingertipsSkeleton::PINKY_TIP, HandWithFingertipsSkeleton::RING_TIP, HandWithFingertipsSkeleton::MIDDLE_TIP, HandWithFingertipsSkeleton::INDEX_TIP, HandWithFingertipsSkeleton::THUMB_TIP};
	int top[] = {HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::INDEX_TOP, HandWithFingertipsSkeleton::THUMB_TOP};
	int mid[] = {HandWithFingertipsSkeleton::PINKY_MID,	HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::INDEX_MID, HandWithFingertipsSkeleton::THUMB_MID};
	int base[] = {HandWithFingertipsSkeleton::PINKY_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::INDEX_BASE, HandWithFingertipsSkeleton::THUMB_BASE};

	int fingerCount = 5;

	for(int i = 0; i < fingerCount; i++) {
		int fingerTip = tip[i];
		int fingerTop = top[i];
		int fingerMid = mid[i];
		int fingerBase = base[i];

		ofVec2f tipPos = handWithFingertipsSkeleton->getPositionAbsolute(fingerTip);
		ofVec2f topPos = handWithFingertipsSkeleton->getPositionAbsolute(fingerTop);
		ofVec2f midPos = handWithFingertipsSkeleton->getPositionAbsolute(fingerMid);
		ofVec2f basePos = handWithFingertipsSkeleton->getPositionAbsolute(fingerBase);

		//float bottomLen = basePos.distance(midPos);
		float middleLen = midPos.distance(topPos);
		float topLen = topPos.distance(tipPos);
		ofVec2f dir = midPos - basePos;

		//basePos = handWithFingertipsSkeleton.getPositionAbsolute(fingerBase);
		//dir.normalize();
		//dir *= bottomLen;
		//handWithFingertipsSkeleton.setPosition(fingerMid, basePos, true, false);
		//handWithFingertipsSkeleton.setPosition(fingerMid, dir, false, false);

		midPos = handWithFingertipsSkeleton->getPositionAbsolute(fingerMid);
		dir.normalize();
		dir *= middleLen;
		handWithFingertipsSkeleton->setPosition(fingerTop, midPos, true, false);
		handWithFingertipsSkeleton->setPosition(fingerTop, dir, false, false);

		topPos = handWithFingertipsSkeleton->getPositionAbsolute(fingerTop);
		dir.normalize();
		dir *= topLen;
		handWithFingertipsSkeleton->setPosition(fingerTip, topPos, true, false);
		handWithFingertipsSkeleton->setPosition(fingerTip, dir, false, false);
	}
}
void StraightenFingersScene::updateMouse(float mx, float my) {
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
	int tip[] = {HandWithFingertipsSkeleton::THUMB_TIP, HandWithFingertipsSkeleton::INDEX_TIP, HandWithFingertipsSkeleton::MIDDLE_TIP, HandWithFingertipsSkeleton::RING_TIP, HandWithFingertipsSkeleton::PINKY_TIP};

	ofVec2f origWristPos = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(wrist));
	ofVec2f origPalmPos = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(palm));
	ofVec2f origBasePos[fingerCount]; 
	ofVec2f origMidPos[fingerCount]; 
	ofVec2f origTopPos[fingerCount]; 
	ofVec2f origTipPos[fingerCount];
	for (int i=0; i < fingerCount; i++) {
		origBasePos[i] = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(base[i]));
		origMidPos[i] = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(mid[i]));
		origTopPos[i] = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(top[i]));
		origTipPos[i] = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(tip[i]));
	}

	ofVec2f origPalmDir;
	ofVec2f origFingerDir;
	float curRot;
	float newRot;

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
				newRot = ofMap(mx, 0, 384, -(curRot+maxPalmAngleLeft), -(curRot));
			}
			else {
				newRot = ofMap(mx, 384, 768, -(curRot), -(curRot+maxPalmAngleRight));
			}

			handWithFingertipsSkeleton->setRotation(palm, newRot, true, false);
			immutableHandWithFingertipsSkeleton->setRotation(palm, newRot, true, false);
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

				handWithFingertipsSkeleton->setRotation(base[i], newRot, true, false);
				immutableHandWithFingertipsSkeleton->setRotation(base[i], newRot, true, false);
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

				handWithFingertipsSkeleton->setRotation(mid[i], newRot, true, false);
				immutableHandWithFingertipsSkeleton->setRotation(mid[i], newRot, true, false);
			}
			break;
		case 4: // finger top rotation
			for (int i=0; i < fingerCount; i++) {
				origFingerDir = origTipPos[i] - origTopPos[i];
				curRot = origFingerDir.angle(xAxis);

				if (mx <= 384) {
					newRot = ofMap(mx, 0, 384, -(curRot+maxTopAngleLeft), -(curRot));
				}
				else {
					newRot = ofMap(mx, 384, 768, -(curRot), -(curRot+maxTopAngleRight));
				}

				handWithFingertipsSkeleton->setRotation(top[i], newRot, true, false);
				immutableHandWithFingertipsSkeleton->setRotation(top[i], newRot, true, false);
			}
			break;
	}
}
void StraightenFingersScene::draw() {
}