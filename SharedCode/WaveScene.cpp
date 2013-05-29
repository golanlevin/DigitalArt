#pragma once

#include "WaveScene.h"

WaveScene::WaveScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Wave", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);
}
void WaveScene::setupGui() {
	WaveScene::initializeGui();
}
void WaveScene::setupMouseGui() {
	WaveScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
}
void WaveScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;

	int toWave[] = {HandSkeleton::PINKY_MID, HandSkeleton::RING_MID, HandSkeleton::MIDDLE_MID, HandSkeleton::INDEX_MID};
	int toWaveCount = 4;
	float theta = ofMap(sin(2 * ofGetElapsedTimef()), -1, 1, -20, 20);
	for(int i = 0; i < toWaveCount; i++) {
		int index = toWave[i];
		handSkeleton->setRotation(index, 2 * theta);
		handSkeleton->setRotation((int) ((int)index-1), -theta);
	}
}
void WaveScene::updateMouse(float mx, float my) {
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
void WaveScene::draw() {
}