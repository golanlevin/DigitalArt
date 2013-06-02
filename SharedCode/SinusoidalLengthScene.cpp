#pragma once

#include "SinusoidalLengthScene.h"

SinusoidalLengthScene::SinusoidalLengthScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Sinusoidal Length", "Sinusoidal Length (Hand)", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;
	this->maxBaseAngleLeft = 20;
	this->maxBaseAngleRight = -20;
	this->maxMidAngleLeft = 45;
	this->maxMidAngleRight = -30;

	this->maxLength = 15;
	this->speedUp = 2;
	this->phaseOffset = 0.5;
}
void SinusoidalLengthScene::setupGui() {
	SinusoidalLengthScene::initializeGui();

	this->gui->addSlider("Max Length", 10, 30, &maxLength);
	this->gui->addSpacer();
	this->gui->addSlider("Speed Up", 1, 5, &speedUp);
	this->gui->addSpacer();
	this->gui->addSlider("Phase Offset", 0, 1, &phaseOffset);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void SinusoidalLengthScene::setupMouseGui() {
	SinusoidalLengthScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	mouseOptions.push_back("Finger Base Rotation");
	mouseOptions.push_back("Finger Mid Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void SinusoidalLengthScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;
	HandSkeleton* immutableHandSkeleton = (HandSkeleton*)this->immutableSkeleton;

	int mid[] = {HandSkeleton::PINKY_MID, HandSkeleton::RING_MID, HandSkeleton::MIDDLE_MID, HandSkeleton::INDEX_MID, HandSkeleton::THUMB_MID};
	int tip[] = {HandSkeleton::PINKY_TIP, HandSkeleton::RING_TIP, HandSkeleton::MIDDLE_TIP, HandSkeleton::INDEX_TIP, HandSkeleton::THUMB_TIP};
	int fingerCount = 5;
	float t = ofGetElapsedTimef();
		
	int index;
	ofVec2f original;
	ofVec2f parent;
	ofVec2f position;
	for(int i = 0; i < fingerCount; i++) {
		index = mid[i];
		original = immutableSkeleton->getPositionAbsolute(index);
		parent = immutableSkeleton->getPositionAbsolute((int) ((int)index-1));
		position = original-parent;
		position.normalize();
		position = position * (maxLength*sin(speedUp*t + i*phaseOffset));
		handSkeleton->setPosition(index, position, false, false);

		index = tip[i];
		original = immutableSkeleton->getPositionAbsolute(index);
		parent = immutableSkeleton->getPositionAbsolute((int) ((int)index-1));
		position = original-parent;
		position.normalize();
		position = position * (maxLength*sin(speedUp*t + i*phaseOffset));
		handSkeleton->setPosition(index, position, false, false);
	}
}
void SinusoidalLengthScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;
	HandSkeleton* immutableHandSkeleton = (HandSkeleton*)this->immutableSkeleton;

	ofVec2f xAxis(1, 0);

	const int fingerCount = 5;

	int wrist = HandSkeleton::WRIST;
	int palm = HandSkeleton::PALM;
	int base[] = {HandSkeleton::THUMB_BASE, HandSkeleton::INDEX_BASE, HandSkeleton::MIDDLE_BASE, HandSkeleton::RING_BASE, HandSkeleton::PINKY_BASE};
	int mid[] = {HandSkeleton::THUMB_MID, HandSkeleton::INDEX_MID, HandSkeleton::MIDDLE_MID, HandSkeleton::RING_MID, HandSkeleton::PINKY_MID};
	int top[] = {HandSkeleton::THUMB_TIP, HandSkeleton::INDEX_TIP, HandSkeleton::MIDDLE_TIP, HandSkeleton::RING_TIP, HandSkeleton::PINKY_TIP};

	ofVec2f origWristPos = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(wrist));
	ofVec2f origPalmPos = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(palm));
	ofVec2f origBasePos[fingerCount]; 
	ofVec2f origMidPos[fingerCount]; 
	ofVec2f origTopPos[fingerCount]; 
	for (int i=0; i < fingerCount; i++) {
		origBasePos[i] = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(base[i]));
		origMidPos[i] = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(mid[i]));
		origTopPos[i] = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(top[i]));
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
			handSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			immutableHandSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
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

			handSkeleton->setRotation(palm, newRot, true, false);
			immutableHandSkeleton->setRotation(palm, newRot, true, false);
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

				handSkeleton->setRotation(base[i], newRot, true, false);
				immutableHandSkeleton->setRotation(base[i], newRot, true, false);
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

				handSkeleton->setRotation(mid[i], newRot, true, false);
				immutableHandSkeleton->setRotation(mid[i], newRot, true, false);
			}
			break;
	}
}
void SinusoidalLengthScene::draw() {
}