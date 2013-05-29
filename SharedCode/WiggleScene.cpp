#pragma once

#include "WiggleScene.h"

WiggleScene::WiggleScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Wiggle", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);
}
void WiggleScene::setupGui() {
	WiggleScene::initializeGui();

	this->gui->autoSizeToFitWidgets();
}
void WiggleScene::setupMouseGui() {
	WiggleScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);

	this->mouseGui->autoSizeToFitWidgets();
}
void WiggleScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;

	int toWiggle[] = {HandSkeleton::PINKY_TIP, HandSkeleton::RING_TIP, HandSkeleton::MIDDLE_TIP, HandSkeleton::INDEX_TIP, HandSkeleton::THUMB_TIP};
	int toWiggleCount = 5;
	float wiggleRange = 10;
	float t = ofGetElapsedTimef();
	for(int i = 0; i < toWiggleCount; i++) {
		int index = toWiggle[i];
		ofVec2f position(wiggleRange * ofVec2f(ofNoise(i, t, 0), ofNoise(i, t, 1)));
		handSkeleton->setPosition(index, position, false, false);
	}
}
void WiggleScene::updateMouse(float mx, float my) {
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
void WiggleScene::draw() {
}