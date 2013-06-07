#pragma once

#include "RetractingFingersScene.h"

RetractingFingersScene::RetractingFingersScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton) {
	Scene::Scene();
	Scene::setup("Retracting Fingers", "Retracting Fingers (Hand With Fingertips)", puppet, (Skeleton*)handWithFingertipsSkeleton, (Skeleton*)immutableHandWithFingertipsSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;
	this->maxBaseAngleLeft = 20;
	this->maxBaseAngleRight = -20;
	this->maxMidAngleLeft = 45;
	this->maxMidAngleRight = -30;
	this->maxTopAngleLeft = 30;
	this->maxTopAngleRight = -20;

	this->retractHeight = 360;
	this->tipRatio = 0.2;
	this->topRatio = 0.3;
	this->middleRatio = 0.5;
	this->bottomRatio = 1;
}
void RetractingFingersScene::setupGui() {
	RetractingFingersScene::initializeGui();

	this->gui->addSlider("Retract Height", 0, 500, &retractHeight);
	this->gui->addSpacer();
	this->gui->addSlider("Tip Ratio", 0, 1, &tipRatio);
	this->gui->addSpacer();
	this->gui->addSlider("Top Ratio", 0, 1, &topRatio);
	this->gui->addSpacer();
	this->gui->addSlider("Middle Ratio", 0, 1, &middleRatio);
	this->gui->addSpacer();
	this->gui->addSlider("Bottom Ratio", 0, 1, &bottomRatio);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void RetractingFingersScene::setupMouseGui() {
	RetractingFingersScene::initializeMouseGui();

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
void RetractingFingersScene::update() {
	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;
	HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->immutableSkeleton;

	int palm = HandWithFingertipsSkeleton::PALM;
	int tip[] = {HandWithFingertipsSkeleton::PINKY_TIP, HandWithFingertipsSkeleton::RING_TIP, HandWithFingertipsSkeleton::MIDDLE_TIP, HandWithFingertipsSkeleton::INDEX_TIP};
	int top[] = {HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::INDEX_TOP};
	int mid[] = {HandWithFingertipsSkeleton::PINKY_MID, HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::INDEX_MID};
	int base[] = {HandWithFingertipsSkeleton::PINKY_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::INDEX_BASE};

	float fingerTot = tipRatio + topRatio + middleRatio;// + bottomRatio;

	int fingerCount = 4;
	for(int i = 0; i < fingerCount; i++) {
		ofVec2f fingerTip = immutableHandWithFingertipsSkeleton->getPositionAbsolute(tip[i]);
		if (fingerTip.y < retractHeight) {
			ofVec2f fingerBase = immutableHandWithFingertipsSkeleton->getPositionAbsolute(base[i]);

			float bigDx = fingerTip.x - fingerBase.x;
			float bigDy = fingerTip.y - fingerBase.y;

			float dy = retractHeight - fingerBase.y;
			float dx = (dy * bigDx) / bigDy;

			float maxLen = sqrt(dx*dx + dy*dy);

			if (dy > 0) {
				dy = 0; 
				dx = 0;
				maxLen = 0;
			}

			ofVec2f original;
			ofVec2f parent;
			ofVec2f position;
	
			original = immutableHandWithFingertipsSkeleton->getPositionAbsolute(tip[i]);
			parent = immutableHandWithFingertipsSkeleton->getPositionAbsolute(top[i]);
			position = original - parent;
			position.normalize();
			position *= (tipRatio/fingerTot) * maxLen;
			handWithFingertipsSkeleton->setBoneLength(tip[i], position);

			original = immutableHandWithFingertipsSkeleton->getPositionAbsolute(top[i]);
			parent = immutableHandWithFingertipsSkeleton->getPositionAbsolute(mid[i]);
			position = original - parent;
			position.normalize();
			position *= (topRatio/fingerTot) * maxLen;
			handWithFingertipsSkeleton->setBoneLength(top[i], position);

			original = immutableHandWithFingertipsSkeleton->getPositionAbsolute(mid[i]);
			parent = immutableHandWithFingertipsSkeleton->getPositionAbsolute(base[i]);
			position = original - parent;
			position.normalize();
			position *= (middleRatio/fingerTot) * maxLen;
			handWithFingertipsSkeleton->setBoneLength(mid[i], position);

			//original = immutableHandWithFingertipsSkeleton->getPositionAbsolute(base[i]);
			//parent = immutableHandWithFingertipsSkeleton->getPositionAbsolute(palm);
			//position = original - parent;
			//position.normalize();
			//position *= (bottomRatio/fingerTot) * maxLen;
			//handWithFingertipsSkeleton->setBoneLength(base[i], position);
		}	
	}
}
void RetractingFingersScene::updateMouse(float mx, float my) {
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
void RetractingFingersScene::draw() {
	ofSetColor(255);
	ofLine(0, retractHeight, 768, retractHeight);
}