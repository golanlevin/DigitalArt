#pragma once

#include "StartrekScene.h"

StartrekScene::StartrekScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Startrek", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;
}
void StartrekScene::setupGui() {
	StartrekScene::initializeGui();

	this->gui->autoSizeToFitWidgets();
}
void StartrekScene::setupMouseGui() {
	StartrekScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void StartrekScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;

	int toImitateTop[] = {HandSkeleton::PINKY_TIP, HandSkeleton::INDEX_TIP};
	int toImitateMid[] = {HandSkeleton::PINKY_MID, HandSkeleton::INDEX_MID};
	int toImitateBase[] = {HandSkeleton::PINKY_BASE, HandSkeleton::INDEX_BASE};
		
	int toSetMid[] = {HandSkeleton::RING_MID, HandSkeleton::MIDDLE_MID};
	int toSetBase[] = {HandSkeleton::RING_BASE, HandSkeleton::MIDDLE_BASE};
	int toSetCount = 2;
		
	ofVec2f imitateTop;
	ofVec2f imitateMid;
	ofVec2f imitateBase;
	for(int i = 0; i < toSetCount; i++) {
		ofVec2f xAxis(1, 0);
		ofVec2f yAxis(0, -1);

		imitateTop = handSkeleton->getPositionAbsolute(toImitateTop[i]);
		imitateBase = handSkeleton->getPositionAbsolute(toImitateBase[i]);
		ofVec2f dir = imitateTop - imitateBase;

		float fromY = dir.angle(yAxis);

		float angleOffset = ofMap(abs(fromY), 0, 60, 0, 25);
		if (i % 2 == 1) angleOffset = -angleOffset;
		handSkeleton->setRotation(toImitateBase[i], angleOffset, false, false);

		imitateMid = handSkeleton->getPositionAbsolute(toImitateMid[i]);
		imitateBase = handSkeleton->getPositionAbsolute(toImitateBase[i]);
			
		ofVec2f dirBase = imitateMid - imitateBase;
		float baseAngle = dirBase.angle(xAxis);

		int setBase = toSetBase[i];
		handSkeleton->setRotation(setBase, -baseAngle, true, false);

		imitateTop = handSkeleton->getPositionAbsolute(toImitateTop[i]);
		imitateMid = handSkeleton->getPositionAbsolute(toImitateMid[i]);
			
		ofVec2f dirMid = imitateTop - imitateMid;
		float midAngle = dirMid.angle(xAxis);

		int setMid = toSetMid[i];
		handSkeleton->setRotation(setMid, -midAngle, true, false);

		ofVec2f original = handSkeleton->getPositionAbsolute(setBase);
		ofVec2f position = imitateBase - original;
		position.normalize();
		position *= 35;
		handSkeleton->setPosition(toImitateBase[i], original, true, false);
		handSkeleton->setPosition(toImitateBase[i], position, false, false);
	}
}
void StartrekScene::updateMouse(float mx, float my) {
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
void StartrekScene::draw() {
}
