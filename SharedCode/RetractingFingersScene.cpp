#pragma once

#include "RetractingFingersScene.h"

RetractingFingersScene::RetractingFingersScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton) {
	Scene::Scene();
	Scene::setup("Retracting Fingers", puppet, (Skeleton*)handWithFingertipsSkeleton, (Skeleton*)immutableHandWithFingertipsSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;

	this->retractHeight = 150;
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

	float fingerTot = tipRatio + topRatio + middleRatio + bottomRatio;

	int fingerCount = 4;
	for(int i = 0; i < fingerCount; i++) {
		ofVec2f fingerTip = immutableHandWithFingertipsSkeleton->getPositionAbsolute(tip[i]);
		if (fingerTip.y < retractHeight) {
			ofVec2f fingerBase = immutableHandWithFingertipsSkeleton->getPositionAbsolute(palm);

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

			original = immutableHandWithFingertipsSkeleton->getPositionAbsolute(base[i]);
			parent = immutableHandWithFingertipsSkeleton->getPositionAbsolute(palm);
			position = original - parent;
			position.normalize();
			position *= (bottomRatio/fingerTot) * maxLen;
			handWithFingertipsSkeleton->setBoneLength(base[i], position);
		}	
	}
}
void RetractingFingersScene::updateMouse(float mx, float my) {
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
void RetractingFingersScene::draw() {
	ofSetColor(255);
	ofLine(0, retractHeight, 768, retractHeight);
}