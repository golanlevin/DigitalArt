#pragma once

#include "SplayFingersScene.h"

SplayFingersScene::SplayFingersScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton) {
	Scene::Scene();
	Scene::setup("Splay Fingers", "Splay Fingers (Hand With Fingertips)", puppet, (Skeleton*)handWithFingertipsSkeleton, (Skeleton*)immutableHandWithFingertipsSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;
	this->maxBaseAngleLeft = 20;
	this->maxBaseAngleRight = -20;

	this->splayHeight = 530;
	this->splayAxis = 291;
	this->maxAngle = 45;
}
void SplayFingersScene::setupGui() {
	SplayFingersScene::initializeGui();

	this->gui->addSlider("Splay Height", 0, 1024, &splayHeight);
	this->gui->addSpacer();
	this->gui->addSlider("Splay Axis", 0, 768, &splayAxis);
	this->gui->addSpacer();
	this->gui->addSlider("Max Angle", 0, 90, &maxAngle);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void SplayFingersScene::setupMouseGui() {
	SplayFingersScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	mouseOptions.push_back("Finger Base Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void SplayFingersScene::update() {
	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;

	int palm = HandWithFingertipsSkeleton::PALM;
	ofVec2f palmPos = handWithFingertipsSkeleton->getPositionAbsolute(palm);

	if (palmPos.y < splayHeight) {
		int tip[] = {HandWithFingertipsSkeleton::PINKY_TIP, HandWithFingertipsSkeleton::RING_TIP, HandWithFingertipsSkeleton::MIDDLE_TIP, HandWithFingertipsSkeleton::INDEX_TIP, HandWithFingertipsSkeleton::THUMB_TIP};
		int top[] = {HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::INDEX_TOP, HandWithFingertipsSkeleton::THUMB_TOP};
		int mid[] = {HandWithFingertipsSkeleton::PINKY_MID,	HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::INDEX_MID, HandWithFingertipsSkeleton::THUMB_MID};
		int base[] = {HandWithFingertipsSkeleton::PINKY_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::INDEX_BASE, HandWithFingertipsSkeleton::THUMB_BASE};

		float angleOffset = ofMap(palmPos.y, splayHeight, 0, 0, maxAngle);
		int fingerCount = 5;
		for (int i=0; i < fingerCount; i++) {
			int joints[] = {base[i], mid[i], top[i], tip[i]};

			ofVec2f basePos = handWithFingertipsSkeleton->getPositionAbsolute(joints[0]);
			if (i == fingerCount-1) basePos = handWithFingertipsSkeleton->getPositionAbsolute(joints[1]); 
			if (basePos.x < splayAxis) angleOffset = -abs(angleOffset);
			else if (basePos.x > splayAxis) angleOffset = abs(angleOffset);
			else angleOffset = 0;
			basePos = handWithFingertipsSkeleton->getPositionAbsolute(joints[0]);

			ofVec2f positions[] = {basePos, handWithFingertipsSkeleton->getPositionAbsolute(joints[1]), handWithFingertipsSkeleton->getPositionAbsolute(joints[2]), handWithFingertipsSkeleton->getPositionAbsolute(joints[3])};
			float lengths[] = {positions[0].distance(positions[1]), positions[1].distance(positions[2]), positions[2].distance(positions[3])};
			ofVec2f dir = positions[1] - positions[0];
			dir.normalize();

			int fingerPartCount = 3;
			for(int j = 0; j < fingerPartCount; j++) {
				dir = dir.getRotated(angleOffset);
				dir.normalize();
				dir = dir * lengths[j];

				ofVec2f parent = handWithFingertipsSkeleton->getPositionAbsolute(joints[j]);

				handWithFingertipsSkeleton->setPosition(joints[j+1], parent, true, false);
				handWithFingertipsSkeleton->setPosition(joints[j+1], dir, false, false);
			}
		}
	}
}
void SplayFingersScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;
	HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->immutableSkeleton;

	ofVec2f xAxis(1, 0);

	const int fingerCount = 5;

	int wrist = HandWithFingertipsSkeleton::WRIST;
	int palm = HandWithFingertipsSkeleton::PALM;
	int base[] = {HandWithFingertipsSkeleton::THUMB_BASE, HandWithFingertipsSkeleton::INDEX_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::PINKY_BASE};
	int mid[] = {HandWithFingertipsSkeleton::THUMB_MID, HandWithFingertipsSkeleton::INDEX_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::PINKY_MID};
	int top[] = {HandWithFingertipsSkeleton::THUMB_TIP, HandWithFingertipsSkeleton::INDEX_TIP, HandWithFingertipsSkeleton::MIDDLE_TIP, HandWithFingertipsSkeleton::RING_TIP, HandWithFingertipsSkeleton::PINKY_TIP};

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
	}
}
void SplayFingersScene::draw() {
	ofSetColor(255);
	ofLine(0, splayHeight, 768, splayHeight);
	ofLine(splayAxis, 0, splayAxis, 1024);
}