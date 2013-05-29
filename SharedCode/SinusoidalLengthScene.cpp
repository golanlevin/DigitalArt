#pragma once

#include "SinusoidalLengthScene.h"

SinusoidalLengthScene::SinusoidalLengthScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Sinusoidal Length", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);

	this->maxLength = 15;
	this->speedUp = 2;
	this->phaseOffset = 0.5;
}
void SinusoidalLengthScene::setupGui() {
	SinusoidalLengthScene::initializeGui();

	this->gui->addSlider("Max Length", 10, 30, &maxLength);
	this->gui->addSlider("Speed Up", 1, 5, &speedUp);
	this->gui->addSlider("Phase Offset", 0, 1, &phaseOffset);

	this->gui->autoSizeToFitWidgets();
}
void SinusoidalLengthScene::setupMouseGui() {
	SinusoidalLengthScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);

	this->mouseGui->autoSizeToFitWidgets();
}
void SinusoidalLengthScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;

	int mid[] = {HandSkeleton::PINKY_MID, HandSkeleton::RING_MID, HandSkeleton::MIDDLE_MID, HandSkeleton::INDEX_MID, HandSkeleton::THUMB_MID};
	int tip[] = {HandSkeleton::PINKY_TIP, HandSkeleton::RING_TIP, HandSkeleton::MIDDLE_TIP, HandSkeleton::INDEX_TIP, HandSkeleton::THUMB_TIP};
	int fingerCount = 5;
	float t = ofGetElapsedTimef();
		
	int index;
	ofVec2f original;
	ofVec2f parent;
	ofVec2f position(0, 0);
	for(int i = 0; i < fingerCount; i++) {
		index = mid[i];
		original = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(index));
		parent = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex((int) ((int)index-1)));
		position.set(original-parent);
		position.normalize();
		position = position * (maxLength*sin(speedUp*t + i*phaseOffset));
		handSkeleton->setPosition(index, position, false, false);

		index = tip[i];
		original = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(index));
		parent = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex((int) ((int)index-1)));
		position.set(original-parent);
		position.normalize();
		position = position * (maxLength*sin(speedUp*t + i*phaseOffset));
		handSkeleton->setPosition(index, position, false, false);
	}
}
void SinusoidalLengthScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;
	HandSkeleton* immutableHandSkeleton = (HandSkeleton*)this->immutableSkeleton;

	switch(getSelection(mouseRadio)) {
		case 0: // palm position
			handSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			immutableHandSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			break;
		case 1: // palm rotation
			break;
	}
}
void SinusoidalLengthScene::draw() {
}