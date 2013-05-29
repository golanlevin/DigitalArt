#pragma once

#include "WobbleScene.h"

WobbleScene::WobbleScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Wobble", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);
}
void WobbleScene::setupGui() {
	WobbleScene::initializeGui();
}
void WobbleScene::setupMouseGui() {
	WobbleScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
}
void WobbleScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;

	float wobbleRange = 50;
	float t = ofGetElapsedTimef();
	ofVec2f position(wobbleRange * ofVec2f(ofNoise(t, 0), ofNoise(t, 1)));
	handSkeleton->setPosition(HandSkeleton::PALM, position, false, true);
}
void WobbleScene::updateMouse(float mx, float my) {
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
void WobbleScene::draw() {
}