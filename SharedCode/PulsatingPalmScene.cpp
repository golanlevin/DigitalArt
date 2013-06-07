#pragma once

#include "PulsatingPalmScene.h"

PulsatingPalmScene::PulsatingPalmScene(ofxPuppet* puppet, PalmSkeleton* palmSkeleton, PalmSkeleton* immutablePalmSkeleton) {
	Scene::Scene();
	Scene::setup("Pulsating Palm", "Pulsating Palm (Palm)", puppet, (Skeleton*)palmSkeleton, (Skeleton*)immutablePalmSkeleton);

	this->maxPalmAngleLeft = 20;
	this->maxPalmAngleRight = -20;

	this->maxLength = 10;
	this->speedUp = 1;
}
void PulsatingPalmScene::setupGui() {
	PulsatingPalmScene::initializeGui();

	this->gui->addSlider("Max Length", 5, 20, &maxLength);
	this->gui->addSpacer();
	this->gui->addSlider("Speed Up", 0, 5, &speedUp);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void PulsatingPalmScene::setupMouseGui() {
	PulsatingPalmScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void PulsatingPalmScene::update() {
	PalmSkeleton* palmSkeleton = (PalmSkeleton*)this->skeleton;

	int toPulsate[] = {PalmSkeleton::TOP, PalmSkeleton::RIGHT_BASE, PalmSkeleton::RIGHT_MID, PalmSkeleton::RIGHT_TOP, PalmSkeleton::LEFT_BASE, PalmSkeleton::LEFT_MID, PalmSkeleton::LEFT_TOP};
	int toPulsateCount = 7;
	

	float timeVal = ofGetElapsedTimef();
	if (bUseFrameBasedAnimation){
		timeVal = (float)ofGetFrameNum()/ 60.0;
	}
	
	for(int i = 0; i < toPulsateCount; i++) {
		int index = toPulsate[i];
		ofVec2f original = puppet->getOriginalMesh().getVertex(palmSkeleton->getControlIndex(index));
		ofVec2f parent = puppet->getOriginalMesh().getVertex(palmSkeleton->getControlIndex(PalmSkeleton::CENTROID));
		ofVec2f position(original-parent);
		position.normalize();
		position = position * (maxLength*sin(speedUp*timeVal));
		palmSkeleton->setPosition(index, position, false, false);
	}
}
void PulsatingPalmScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	PalmSkeleton* palmSkeleton = (PalmSkeleton*)this->skeleton;
	PalmSkeleton* immutablePalmSkeleton = (PalmSkeleton*)this->immutableSkeleton;

	switch(getSelection(mouseRadio)) {
		case 0: // palm position
			palmSkeleton->setPosition(PalmSkeleton::BASE, mouse, true);
			immutablePalmSkeleton->setPosition(PalmSkeleton::BASE, mouse, true);
			break;
		case 1: // palm rotation
			ofVec2f xAxis(1, 0);

			int wrist = PalmSkeleton::BASE;
			int palm = PalmSkeleton::CENTROID;

			ofVec2f origWristPos = puppet->getOriginalMesh().getVertex(palmSkeleton->getControlIndex(wrist));
			ofVec2f origPalmPos = puppet->getOriginalMesh().getVertex(palmSkeleton->getControlIndex(palm));

			ofVec2f origPalmDir = origPalmPos - origWristPos;
			
			float curRot = origPalmDir.angle(xAxis);

			float newRot;
			if (mx <= 384) {
				newRot = ofMap(mx, 0, 384, -(curRot+maxPalmAngleLeft), -(curRot));
			}
			else {
				newRot = ofMap(mx, 384, 768, -(curRot), -(curRot+maxPalmAngleRight));
			}

			palmSkeleton->setRotation(palm, newRot, true, false);
			immutablePalmSkeleton->setRotation(palm, newRot, true, false);
			break;
	}
}
void PulsatingPalmScene::draw() {
}
