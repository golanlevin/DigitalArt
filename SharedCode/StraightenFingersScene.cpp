#pragma once

#include "StraightenFingersScene.h"

StraightenFingersScene::StraightenFingersScene(ofxPuppet* puppet, HandWithFingertipsSkeleton* handWithFingertipsSkeleton, HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton) {
	Scene::Scene();
	Scene::setup("Straighten Fingers", puppet, (Skeleton*)handWithFingertipsSkeleton, (Skeleton*)immutableHandWithFingertipsSkeleton);
}
void StraightenFingersScene::setupGui() {
	StraightenFingersScene::initializeGui();
}
void StraightenFingersScene::setupMouseGui() {
	StraightenFingersScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
}
void StraightenFingersScene::update() {
	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;

	int tip[] = {HandWithFingertipsSkeleton::PINKY_TIP, HandWithFingertipsSkeleton::RING_TIP, HandWithFingertipsSkeleton::MIDDLE_TIP, HandWithFingertipsSkeleton::INDEX_TIP, HandWithFingertipsSkeleton::THUMB_TIP};
	int top[] = {HandWithFingertipsSkeleton::PINKY_TOP, HandWithFingertipsSkeleton::RING_TOP, HandWithFingertipsSkeleton::MIDDLE_TOP, HandWithFingertipsSkeleton::INDEX_TOP, HandWithFingertipsSkeleton::THUMB_TOP};
	int mid[] = {HandWithFingertipsSkeleton::PINKY_MID,	HandWithFingertipsSkeleton::RING_MID, HandWithFingertipsSkeleton::MIDDLE_MID, HandWithFingertipsSkeleton::INDEX_MID, HandWithFingertipsSkeleton::THUMB_MID};
	int base[] = {HandWithFingertipsSkeleton::PINKY_BASE, HandWithFingertipsSkeleton::RING_BASE, HandWithFingertipsSkeleton::MIDDLE_BASE, HandWithFingertipsSkeleton::INDEX_BASE, HandWithFingertipsSkeleton::THUMB_BASE};

	int fingerCount = 5;

	for(int i = 0; i < fingerCount; i++) {
		int fingerTip = tip[i];
		int fingerTop = top[i];
		int fingerMid = mid[i];
		int fingerBase = base[i];

		ofVec2f tipPos = handWithFingertipsSkeleton->getPositionAbsolute(fingerTip);
		ofVec2f topPos = handWithFingertipsSkeleton->getPositionAbsolute(fingerTop);
		ofVec2f midPos = handWithFingertipsSkeleton->getPositionAbsolute(fingerMid);
		ofVec2f basePos = handWithFingertipsSkeleton->getPositionAbsolute(fingerBase);

		//float bottomLen = basePos.distance(midPos);
		float middleLen = midPos.distance(topPos);
		float topLen = topPos.distance(tipPos);
		ofVec2f dir = midPos - basePos;

		//basePos = handWithFingertipsSkeleton.getPositionAbsolute(fingerBase);
		//dir.normalize();
		//dir *= bottomLen;
		//handWithFingertipsSkeleton.setPosition(fingerMid, basePos, true, false);
		//handWithFingertipsSkeleton.setPosition(fingerMid, dir, false, false);

		midPos = handWithFingertipsSkeleton->getPositionAbsolute(fingerMid);
		dir.normalize();
		dir *= middleLen;
		handWithFingertipsSkeleton->setPosition(fingerTop, midPos, true, false);
		handWithFingertipsSkeleton->setPosition(fingerTop, dir, false, false);

		topPos = handWithFingertipsSkeleton->getPositionAbsolute(fingerTop);
		dir.normalize();
		dir *= topLen;
		handWithFingertipsSkeleton->setPosition(fingerTip, topPos, true, false);
		handWithFingertipsSkeleton->setPosition(fingerTip, dir, false, false);
	}
}
void StraightenFingersScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	HandWithFingertipsSkeleton* handWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->skeleton;
	HandWithFingertipsSkeleton* immutableHandWithFingertipsSkeleton = (HandWithFingertipsSkeleton*)this->immutableSkeleton;

	switch(getSelection(mouseRadio)) {
		case 0: // palm position
			handWithFingertipsSkeleton->setPosition(HandWithFingertipsSkeleton::PALM, mouse, true);
			immutableHandWithFingertipsSkeleton->setPosition(HandWithFingertipsSkeleton::PALM, mouse, true);
			break;
		case 1: // palm rotation
			break;
	}
}
void StraightenFingersScene::draw() {
}