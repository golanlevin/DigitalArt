#pragma once

#include "MeanderScene.h"

MeanderScene::MeanderScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Meander", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;

	this->meanderAmount = 20;
}
void MeanderScene::setupGui() {
	MeanderScene::initializeGui();

	this->gui->addSlider("Meander Amount", 0, 60, &meanderAmount);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void MeanderScene::setupMouseGui() {
	MeanderScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void MeanderScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;

	int toMove[] = {HandSkeleton::PINKY_BASE, HandSkeleton::RING_BASE, HandSkeleton::MIDDLE_BASE, HandSkeleton::INDEX_BASE};
	int toMoveCount = 4;
	float t = ofGetElapsedTimef();
	for(int i = 0; i < toMoveCount; i++) {
		handSkeleton->setRotation(toMove[i], meanderAmount * ofSignedNoise(i, t));
	}
}
void MeanderScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;
	HandSkeleton* immutableHandSkeleton = (HandSkeleton*)this->immutableSkeleton;

	switch(getSelection(mouseRadio)) {
		case 0: // palm position
			handSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			immutableHandSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			break;
		case 1: // palm rotation
			ofVec2f xAxis(1, 0);

			int wrist = HandSkeleton::WRIST;
			int palm = HandSkeleton::PALM;


			ofVec2f origWristPos = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(wrist));
			ofVec2f origPalmPos = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(palm));

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

			handSkeleton->setRotation(palm, newRot, true, false);
			immutableHandSkeleton->setRotation(palm, newRot, true, false);
			break;
	}
}
void MeanderScene::draw() {
}