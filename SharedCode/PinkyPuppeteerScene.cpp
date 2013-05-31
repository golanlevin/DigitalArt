#pragma once

#include "PinkyPuppeteerScene.h"

PinkyPuppeteerScene::PinkyPuppeteerScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton) {
	Scene::Scene();
	Scene::setup("Pinky Puppeteer", "Pinky Puppeteer (Hand With Fingertips)", puppet, (Skeleton*)handWithFingertipsSkeleton, (Skeleton*)immutableHandWithFingertipsSkeleton);

	this->startMouseControl = true;

	this->maxThumbBaseAngleLeft = 20;
	this->maxThumbBaseAngleRight = -20;
	this->maxThumbMidAngleLeft = 45;
	this->maxThumbMidAngleRight = -30;
	this->maxThumbTopAngleLeft = 30;
	this->maxThumbTopAngleRight = -20;

	this->maxPinkyBaseAngle = 20;
	this->maxPinkyMidAngle = 10;
	this->maxPinkyTopAngle = 10;
}
void PinkyPuppeteerScene::setupGui() {
	PinkyPuppeteerScene::initializeGui();
	
	this->gui->addSlider("Max Base Angle", 0, 30, &maxPinkyBaseAngle);
	this->gui->addSpacer();
	this->gui->addSlider("Max Mid Angle", 0, 30, &maxPinkyMidAngle);
	this->gui->addSpacer();
	this->gui->addSlider("Max Top Angle", 0, 30, &maxPinkyTopAngle);
	this->gui->addSpacer();
	this->gui->addLabel("Pinky Mirrors Thumb", 2);
	this->mirror = this->gui->addToggle("Mirror Movements", true);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void PinkyPuppeteerScene::setupMouseGui() {
	PinkyPuppeteerScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Thumb Base Rotation");
	mouseOptions.push_back("Thumb Mid Rotation");
	mouseOptions.push_back("Thumb Top Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void PinkyPuppeteerScene::update() {
	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;
	
	int thumb[] = {HandWithFingertipsSkeleton::THUMB_BASE, HandWithFingertipsSkeleton::THUMB_MID, HandWithFingertipsSkeleton::THUMB_TOP, HandWithFingertipsSkeleton::THUMB_TIP};
	int pinky[] = {HandWithFingertipsSkeleton::PINKY_BASE, HandWithFingertipsSkeleton::PINKY_MID, HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::PINKY_TIP};

	const int fingerPartCount = 4;
	const int fingerJointCount = 3;

	ofVec2f origThumbPos[fingerPartCount];
	ofVec2f newThumbPos[fingerPartCount];
	for (int i=0; i < fingerPartCount; i++) {
		origThumbPos[i] = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(thumb[i]));
		newThumbPos[i] = handWithFingertipsSkeleton->getPositionAbsolute(thumb[i]);
	}

	ofVec2f origThumbDir[fingerJointCount];
	ofVec2f newThumbDir[fingerJointCount];
	float thumbAngle[fingerJointCount];
	for (int i=0; i < fingerJointCount; i++) {
		origThumbDir[i] = origThumbPos[i+1] - origThumbPos[i];
		newThumbDir[i] = newThumbPos[i+1] - newThumbPos[i];
		thumbAngle[i] = origThumbDir[i].angle(newThumbDir[i]);
	}

	for (int i=0; i < fingerJointCount; i++) {
		float angle;
		if (i==0) angle = ofMap(thumbAngle[i], maxThumbBaseAngleLeft, maxThumbBaseAngleRight, maxPinkyBaseAngle, -maxPinkyBaseAngle);
		else if (i==1) angle = ofMap(thumbAngle[i], maxThumbMidAngleLeft, maxThumbMidAngleRight, maxPinkyMidAngle, -maxPinkyMidAngle);
		else angle = ofMap(thumbAngle[i], maxThumbTopAngleLeft, maxThumbTopAngleRight, maxPinkyTopAngle, -maxPinkyTopAngle);

		if (mirror->getValue()) angle = -angle;

		handWithFingertipsSkeleton->setRotation(pinky[i], angle, false, false);
	}
}
void PinkyPuppeteerScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;
	HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->immutableSkeleton;

	ofVec2f xAxis(1, 0);

	int palm = HandWithFingertipsSkeleton::PALM;
	int thumbBase = HandWithFingertipsSkeleton::THUMB_BASE;
	int thumbMid = HandWithFingertipsSkeleton::THUMB_MID;
	int thumbTop = HandWithFingertipsSkeleton::THUMB_TOP;

	ofVec2f origPalmPos = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(palm));
	ofVec2f origThumbBasePos = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(thumbBase));
	ofVec2f origThumbMidPos = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(thumbMid));
	ofVec2f origThumbTopPos = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(thumbTop));

	ofVec2f origThumbDir;
	float curRot;
	float newRot;
	float correction;
	switch(getSelection(mouseRadio)) {
		case 0: // thumb base rotation
			origThumbDir = origThumbBasePos - origPalmPos;
			
			curRot = origThumbDir.angle(xAxis);
			correction = 26.75;

			if (mx <= 384) {
				newRot = ofMap(mx, 0, 384, -(curRot+correction+maxThumbBaseAngleLeft), -(curRot+correction));
			}
			else {
				newRot = ofMap(mx, 384, 768, -(curRot+correction), -(curRot+correction+maxThumbBaseAngleRight));
			}

			handWithFingertipsSkeleton->setRotation(thumbBase, newRot, true, false);
			immutableHandWithFingertipsSkeleton->setRotation(thumbBase, newRot, true, false);
			break;
		case 1: // thumb mid rotation
			origThumbDir = origThumbMidPos - origThumbBasePos;
			
			curRot = origThumbDir.angle(xAxis);
			correction = 6.75;

			if (mx <= 384) {
				newRot = ofMap(mx, 0, 384, -(curRot+correction+maxThumbMidAngleLeft), -(curRot+correction));
			}
			else {
				newRot = ofMap(mx, 384, 768, -(curRot+correction), -(curRot+correction+maxThumbMidAngleRight));
			}

			handWithFingertipsSkeleton->setRotation(thumbMid, newRot, true, false);
			immutableHandWithFingertipsSkeleton->setRotation(thumbMid, newRot, true, false);
			break;
		case 2: // thumb top rotation
			origThumbDir = origThumbTopPos - origThumbMidPos;
			
			curRot = origThumbDir.angle(xAxis);
			correction = -16;

			if (mx <= 384) {
				newRot = ofMap(mx, 0, 384, -(curRot+correction+maxThumbTopAngleLeft), -(curRot+correction));
			}
			else {
				newRot = ofMap(mx, 384, 768, -(curRot+correction), -(curRot+correction+maxThumbTopAngleRight));
			}

			handWithFingertipsSkeleton->setRotation(thumbTop, newRot, true, false);
			immutableHandWithFingertipsSkeleton->setRotation(thumbTop, newRot, true, false);
			break;
	}
}
void PinkyPuppeteerScene::draw() {
}