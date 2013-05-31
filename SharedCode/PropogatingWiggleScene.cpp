#pragma once

#include "PropogatingWiggleScene.h"

PropogatingWiggleScene::PropogatingWiggleScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton) {
	Scene::Scene();
	Scene::setup("Propogating Wiggle", puppet, (Skeleton*)handWithFingertipsSkeleton, (Skeleton*)immutableHandWithFingertipsSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;

	this->baseAngleRange = 15;
	this->midAngleRange = 30;
	this->topAngleRange = 45;
	this->speedUp = 2;
	this->phaseOffset = 0.5;
}
void PropogatingWiggleScene::setupGui() {
	PropogatingWiggleScene::initializeGui();

	this->gui->addSlider("Base Angle Range", 10, 60, &baseAngleRange);
	this->gui->addSpacer();
	this->gui->addSlider("Mid Angle Range", 10, 60, &midAngleRange);
	this->gui->addSpacer();
	this->gui->addSlider("Top Angle Range", 10, 60, &topAngleRange);
	this->gui->addSpacer();
	this->gui->addSlider("Speed Up", 1, 5, &speedUp);
	this->gui->addSpacer();
	this->gui->addSlider("Phase Offset", 0, 1, &phaseOffset);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void PropogatingWiggleScene::setupMouseGui() {
	PropogatingWiggleScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void PropogatingWiggleScene::update() {
	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;

	int base[] = {HandWithFingertipsSkeleton::PINKY_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::INDEX_BASE};
	int mid[] = {HandWithFingertipsSkeleton::PINKY_MID, HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::INDEX_MID};
	int top[] = {HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::INDEX_TOP};
	int fingerCount = 4;

	int index;
	float theta;
	for (int i=0; i < fingerCount; i++) {
		float baseOffset = 0*phaseOffset + i;
		float midOffset = 1*phaseOffset + i;
		float topOffset = 2*phaseOffset + i;

		index = base[i];
		theta = ofMap(sin(speedUp*ofGetElapsedTimef() + baseOffset), -1, 1, -(baseAngleRange/2.0), baseAngleRange/2.0);
		handWithFingertipsSkeleton->setRotation(index, theta, false, false);
	
		index = mid[i];
		theta = -ofMap(sin(speedUp*ofGetElapsedTimef() + midOffset), -1, 1, -(midAngleRange/2.0), midAngleRange/2.0);
		handWithFingertipsSkeleton->setRotation(index, theta, false, false);

		index = top[i];
		theta = ofMap(sin(speedUp*ofGetElapsedTimef() + topOffset), -1, 1, -(topAngleRange/2.0), topAngleRange/2.0);
		handWithFingertipsSkeleton->setRotation(index, theta, false , false);
	}
}
void PropogatingWiggleScene::updateMouse(float mx, float my) {
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
void PropogatingWiggleScene::draw() {
}