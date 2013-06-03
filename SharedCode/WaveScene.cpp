#pragma once

#include "WaveScene.h"

//==========================================================================
WaveScene::WaveScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Wave", "Wave (Hand)", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);

	this->maxPalmAngleLeft  =  60;
	this->maxPalmAngleRight = -60;
	this->maxBaseAngleLeft  =  20;
	this->maxBaseAngleRight = -20;
}

//==========================================================================
void WaveScene::setupGui() {
	WaveScene::initializeGui();

	this->gui->autoSizeToFitWidgets();
}

//==========================================================================
void WaveScene::setupMouseGui() {
	WaveScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	mouseOptions.push_back("Finger Base Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}

//==========================================================================
void WaveScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;

	int toWave[] = {
		HandSkeleton::PINKY_MID,
		HandSkeleton::RING_MID,
		HandSkeleton::MIDDLE_MID,
		HandSkeleton::INDEX_MID
	};
	int toWaveCount = 4;
	
	
	float timeVal = ofGetElapsedTimef();
	if (bUseFrameBasedAnimation){
		timeVal = (float)ofGetFrameNum()/ 60.0;
	}
	
	float theta = ofMap(sin(2.0 * timeVal), -1, 1, -12, 12);
	for(int i = 0; i < toWaveCount; i++) {
		int index = toWave[i];
		handSkeleton->setRotation(index, 2.0 * theta);
		handSkeleton->setRotation((int) ((int)index-1), -theta);
	}
}

//==========================================================================
void WaveScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;
	HandSkeleton* immutableHandSkeleton = (HandSkeleton*)this->immutableSkeleton;

	ofVec2f xAxis(1, 0);

	const int fingerCount = 5;

	int wrist = HandSkeleton::WRIST;
	int palm  = HandSkeleton::PALM;
	int base[] = {
		HandSkeleton::THUMB_BASE,
		HandSkeleton::INDEX_BASE,
		HandSkeleton::MIDDLE_BASE,
		HandSkeleton::RING_BASE,
		HandSkeleton::PINKY_BASE};
	int mid[] = {
		HandSkeleton::THUMB_MID,
		HandSkeleton::INDEX_MID,
		HandSkeleton::MIDDLE_MID,
		HandSkeleton::RING_MID,
		HandSkeleton::PINKY_MID};
	int top[] = {
		HandSkeleton::THUMB_TIP,
		HandSkeleton::INDEX_TIP,
		HandSkeleton::MIDDLE_TIP,
		HandSkeleton::RING_TIP,
		HandSkeleton::PINKY_TIP};

	ofVec2f origWristPos = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(wrist));
	ofVec2f origPalmPos  = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(palm));
	ofVec2f origBasePos[fingerCount]; 
	ofVec2f origMidPos[fingerCount]; 
	ofVec2f origTopPos[fingerCount];
	
	for (int i=0; i < fingerCount; i++) {
		origBasePos[i] = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(base[i]));
		origMidPos[i]  = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(mid[i]));
		origTopPos[i]  = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(top[i]));
	}

	ofVec2f origPalmDir;
	ofVec2f origFingerDir;
	float curRot;
	float newRot;

	float correction = 0;
	float baseCorrection[] = {26.75, -3, 1.75, 7.75, 9.75};
	float midCorrection[]  = {6.75, 2, -1.5, -1.75, -3.5};

	switch(getSelection(mouseRadio)) {
		case 0: // palm position
			handSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			immutableHandSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			break;
		case 1: // palm rotation
			origPalmDir = origPalmPos - origWristPos;
			
			curRot = origPalmDir.angle(xAxis);

			newRot;
			if (mx <= 384) {
				newRot = ofMap(mx, 0, 384, -(curRot+correction+maxPalmAngleLeft), -(curRot+correction));
			}
			else {
				newRot = ofMap(mx, 384, 768, -(curRot+correction), -(curRot+correction+maxPalmAngleRight));
			}

			handSkeleton->setRotation(palm, newRot, true, false);
			immutableHandSkeleton->setRotation(palm, newRot, true, false);
			break;
		case 2: // finger base rotation
			for (int i=0; i < fingerCount; i++) {
				origFingerDir = origBasePos[i] - origPalmPos;
				curRot = origFingerDir.angle(xAxis);

				if (mx <= 384) {
					newRot = ofMap(mx, 0, 384, -(curRot+baseCorrection[i]+maxBaseAngleLeft), -(curRot+baseCorrection[i]));
				}
				else {
					newRot = ofMap(mx, 384, 768, -(curRot+baseCorrection[i]), -(curRot+baseCorrection[i]+maxBaseAngleRight));
				}

				handSkeleton->setRotation(base[i], newRot, true, false);
				immutableHandSkeleton->setRotation(base[i], newRot, true, false);
			}
			break;
	}
}

//==========================================================================
void WaveScene::draw() {
}