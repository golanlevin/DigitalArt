#pragma once

#include "FingerLengthPuppeteerScene.h"

FingerLengthPuppeteerScene::FingerLengthPuppeteerScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton) {
	Scene::Scene();
	Scene::setup("Finger Length Puppeteer", "Finger Length Puppeteer (Hand With Fingertips)", puppet, (Skeleton*)handWithFingertipsSkeleton, (Skeleton*)immutableHandWithFingertipsSkeleton);

	this->startMouseControl = true;

	this->maxThumbBaseAngleLeft = 20;
	this->maxThumbBaseAngleRight = -20;
	this->maxThumbMidAngleLeft = 45;
	this->maxThumbMidAngleRight = -30;
	this->maxThumbTopAngleLeft = 30;
	this->maxThumbTopAngleRight = -20;

	this->maxLengthRatio = 1.25;

	this->thumbControlJointOptions.push_back("Base Joint");
	this->thumbControlJointOptions.push_back("Mid Joint");
	this->thumbControlJointOptions.push_back("Top Joint");

	this->angleMeasurementOptions.push_back("Relative to Orig. Angle");
	this->angleMeasurementOptions.push_back("Relative to X-Axis");
	this->angleMeasurementOptions.push_back("Relative to Y-Axis");

	this->prevMouseSelection = 0;
	this->prevThumbControlSelection = 0;
}
void FingerLengthPuppeteerScene::setupGui() {
	FingerLengthPuppeteerScene::initializeGui();
	
	this->gui->addSlider("Max Length Ratio", 1, 2, &maxLengthRatio);
	this->gui->addSpacer();
	this->gui->addLabel("Thumb Control Joint Options", 2);
	this->thumbControlJointRadio = this->gui->addRadio("Thumb Control Joint Options", thumbControlJointOptions);
	this->thumbControlJointRadio->getToggles()[0]->setValue(true);
	this->gui->addSpacer();
	this->gui->addLabel("Angle Measurement Options", 2);
	this->angleMeasurementRadio = this->gui->addRadio("Angle Measurement Options", angleMeasurementOptions);
	this->angleMeasurementRadio->getToggles()[0]->setValue(true);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void FingerLengthPuppeteerScene::setupMouseGui() {
	FingerLengthPuppeteerScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Thumb Base Rotation");
	mouseOptions.push_back("Thumb Mid Rotation");
	mouseOptions.push_back("Thumb Top Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void FingerLengthPuppeteerScene::update() {
	if (this->prevThumbControlSelection == getSelection(thumbControlJointRadio)) {
		if (getSelection(mouseRadio) != getSelection(thumbControlJointRadio)) {
			this->thumbControlJointRadio->getToggles()[getSelection(thumbControlJointRadio)]->setValue(false);
			this->thumbControlJointRadio->getToggles()[getSelection(mouseRadio)]->setValue(true);
		}
	}
	else {
		this->prevThumbControlSelection = getSelection(thumbControlJointRadio);
	}

	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;
	HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->immutableSkeleton;

	const int fingerCount = 4;
	const int fingerPartCount = 3;

	int palm = HandWithFingertipsSkeleton::PALM;

	int thumbBase, thumbMid;
	switch(getSelection(thumbControlJointRadio)) {
	case 0:
		thumbBase = HandWithFingertipsSkeleton::THUMB_BASE;
		thumbMid = HandWithFingertipsSkeleton::THUMB_MID;
		break;
	case 1:
		thumbBase = HandWithFingertipsSkeleton::THUMB_MID;
		thumbMid = HandWithFingertipsSkeleton::THUMB_TOP;
		break;
	case 2:
		thumbBase = HandWithFingertipsSkeleton::THUMB_TOP;
		thumbMid = HandWithFingertipsSkeleton::THUMB_TIP;
		break;
	}

	int fingerBases[] = {HandWithFingertipsSkeleton::INDEX_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::PINKY_BASE};

	int index[] = {HandWithFingertipsSkeleton::INDEX_MID, HandWithFingertipsSkeleton::INDEX_TOP, HandWithFingertipsSkeleton::INDEX_TIP};
	int middle[] = {HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::MIDDLE_TIP};
	int ring[] = {HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::RING_TIP};
	int pinky[] = {HandWithFingertipsSkeleton::PINKY_MID, HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::PINKY_TIP};
	int* fingers[] = {index, middle, ring, pinky};

	float ratios[] = {0.5, 0.3, 0.2};
	float ratioTotal = 0;
	for (int i=0; i < fingerPartCount; i++) {
		ratioTotal += ratios[i];
	}

	ofVec2f origThumbBasePos = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(thumbBase));
	ofVec2f origThumbMidPos = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(thumbMid));

	ofVec2f newThumbBasePos = handWithFingertipsSkeleton->getPositionAbsolute(thumbBase);
	ofVec2f newThumbMidPos = handWithFingertipsSkeleton->getPositionAbsolute(thumbMid);

	ofVec2f origThumbDir = origThumbMidPos - origThumbBasePos;
	ofVec2f newThumbDir = newThumbMidPos - newThumbBasePos;
	
	float thumbAngle;
	float lengthRatio;
	ofVec2f xAxis(1, 0);
	float origAngle, newAngle;
	switch(getSelection(angleMeasurementRadio)) {
	case 0:
		thumbAngle = newThumbDir.angle(origThumbDir);
		
		switch(getSelection(thumbControlJointRadio)) {
		case 0:
			if (thumbAngle > 0)
				lengthRatio = ofMap(thumbAngle, maxThumbBaseAngleLeft, 0, maxLengthRatio, 1);
			else 
				lengthRatio = ofMap(thumbAngle, 0, maxThumbBaseAngleRight, 1, maxLengthRatio);
			break;
		case 1:
			if (thumbAngle > 0)
				lengthRatio = ofMap(thumbAngle, maxThumbMidAngleLeft, 0, maxLengthRatio, 1);
			else 
				lengthRatio = ofMap(thumbAngle, 0, maxThumbMidAngleRight, 1, maxLengthRatio);
			break;
		case 2:
			if (thumbAngle > 0)
				lengthRatio = ofMap(thumbAngle, maxThumbTopAngleLeft, 0, maxLengthRatio, 1);
			else 
				lengthRatio = ofMap(thumbAngle, 0, maxThumbTopAngleRight, 1, maxLengthRatio);
			break;
		}

		break;
	case 1:
		switch(getSelection(thumbControlJointRadio)) {
		case 0:
			origAngle = xAxis.angle(origThumbDir);
			newAngle = xAxis.angle(newThumbDir);
			thumbAngle = abs(newAngle-(origAngle + maxThumbBaseAngleRight));
			lengthRatio = ofMap(thumbAngle, 0, abs(maxThumbBaseAngleLeft) + abs(maxThumbBaseAngleRight), maxLengthRatio, 1);
			break;
		case 1:
			origAngle = xAxis.angle(origThumbDir);
			newAngle = xAxis.angle(newThumbDir);
			thumbAngle = abs(newAngle-(origAngle + maxThumbMidAngleRight));
			lengthRatio = ofMap(thumbAngle, 0, abs(maxThumbMidAngleLeft) + abs(maxThumbMidAngleRight), maxLengthRatio, 1);
			break;
		case 2:
			origAngle = xAxis.angle(origThumbDir);
			newAngle = xAxis.angle(newThumbDir);
			thumbAngle = abs(newAngle-(origAngle + maxThumbTopAngleRight));
			lengthRatio = ofMap(thumbAngle, 0, abs(maxThumbTopAngleLeft) + abs(maxThumbTopAngleRight), maxLengthRatio, 1);
			break;
		}

		break;
		case 2:
			switch(getSelection(thumbControlJointRadio)) {
			case 0:
				origAngle = xAxis.angle(origThumbDir);
				newAngle = xAxis.angle(newThumbDir);
				thumbAngle = abs(newAngle-(origAngle + maxThumbBaseAngleRight));
				lengthRatio = ofMap(thumbAngle, 0, abs(maxThumbBaseAngleLeft) + abs(maxThumbBaseAngleRight), 1, maxLengthRatio);
				break;
			case 1:
				origAngle = xAxis.angle(origThumbDir);
				newAngle = xAxis.angle(newThumbDir);
				thumbAngle = abs(newAngle-(origAngle + maxThumbMidAngleRight));
				lengthRatio = ofMap(thumbAngle, 0, abs(maxThumbMidAngleLeft) + abs(maxThumbMidAngleRight), 1, maxLengthRatio);
				break;
			case 2:
				origAngle = xAxis.angle(origThumbDir);
				newAngle = xAxis.angle(newThumbDir);
				thumbAngle = abs(newAngle-(origAngle + maxThumbTopAngleRight));
				lengthRatio = ofMap(thumbAngle, 0, abs(maxThumbTopAngleLeft) + abs(maxThumbTopAngleRight), 1, maxLengthRatio);
				break;
		}

		break;
	}

	for(int i = 0; i < fingerCount; i++) {
		ofVec2f fingerBase = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(fingerBases[i]));
		ofVec2f fingerTip = puppet->getOriginalMesh().getVertex(handWithFingertipsSkeleton->getControlIndex(fingers[i][fingerPartCount-1]));
		
		float origLength = fingerBase.distance(fingerTip); 
		float newLength = lengthRatio * origLength;

		for (int j=0; j < fingerPartCount; j++) {
			ofVec2f original = immutableHandWithFingertipsSkeleton->getPositionAbsolute(fingers[i][j]);
			ofVec2f parent;
			if (j==0) parent = immutableHandWithFingertipsSkeleton->getPositionAbsolute(palm);
			else parent = immutableHandWithFingertipsSkeleton->getPositionAbsolute(fingers[i][j-1]);
			
			ofVec2f position = original - parent;
			position.normalize();
			position *= (ratios[j]/ratioTotal) * newLength;
			handWithFingertipsSkeleton->setBoneLength(fingers[i][j], position);
		}
	}
}
void FingerLengthPuppeteerScene::updateMouse(float mx, float my) {
	if (this->prevMouseSelection == getSelection(mouseRadio)) {
		if (getSelection(mouseRadio) != getSelection(thumbControlJointRadio)) {
			this->mouseRadio->getToggles()[getSelection(mouseRadio)]->setValue(false);
			this->mouseRadio->getToggles()[getSelection(thumbControlJointRadio)]->setValue(true);
		}
	}
	else {
		this->prevMouseSelection = getSelection(mouseRadio);
	}

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
void FingerLengthPuppeteerScene::draw() {
}