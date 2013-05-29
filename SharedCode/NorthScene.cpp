#pragma once

#include "NorthScene.h"

NorthScene::NorthScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("North", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);
}
void NorthScene::setupGui() {
	NorthScene::initializeGui();

	this->gui->autoSizeToFitWidgets();
}
void NorthScene::setupMouseGui() {
	NorthScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);

	this->mouseGui->autoSizeToFitWidgets();
}
void NorthScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;

	int toRotate[] = {HandSkeleton::PINKY_BASE, HandSkeleton::RING_BASE, HandSkeleton::MIDDLE_BASE, HandSkeleton::INDEX_BASE};
	int toRotateCount = 4;
	for(int i = 0; i < toRotateCount; i++) {
		int index = toRotate[i];
		handSkeleton->setRotation(index, -90, true);
	}
}
void NorthScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;
	HandSkeleton* immutableHandSkeleton = (HandSkeleton*)this->immutableSkeleton;

	switch(NorthScene::getSelection(mouseRadio)) {
		case 0: // palm position
			handSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			immutableHandSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			break;
		case 1: // palm rotation
			break;
	}
}
void NorthScene::draw() {
}
