#pragma once

#include "TwitchScene.h"

#define EPSILON 0.001

TwitchScene::TwitchScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Twitch", "Twitch (Hand)", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;
	this->maxBaseAngleLeft = 20;
	this->maxBaseAngleRight = -20;
	this->maxMidAngleLeft = 45;
	this->maxMidAngleRight = -30;

	this->twitchSigmoidStrength = 0.8;
	this->choiceSigmoidStrength = 0.5;
	this->speedUp = 5;
	this->minAngle = 0.9;
}
void TwitchScene::setupGui() {
	TwitchScene::initializeGui();

	this->gui->addSlider("Twitch Sigmoid Strength", 0, 0.99, &twitchSigmoidStrength);
	this->gui->addSpacer();
	this->gui->addSlider("Choice Sigmoid Strength", 0, 0.99, &choiceSigmoidStrength);
	this->gui->addSpacer();
	this->gui->addSlider("Speed Up", 1, 10, &speedUp);
	this->gui->addSpacer();
	this->gui->addSlider("Min Angle", 0, 1, &minAngle);
	this->gui->addSpacer();
	this->gui->addLabel("Fingers Able to Twitch", 1);
	this->pinky = this->gui->addToggle("Pinky", true);
	this->ring = this->gui->addToggle("Ring", true);
	this->middle = this->gui->addToggle("Middle", false);
	this->index = this->gui->addToggle("Index", false);
	this->thumb = this->gui->addToggle("Thumb", false);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void TwitchScene::setupMouseGui() {
	TwitchScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	mouseOptions.push_back("Finger Base Rotation");
	mouseOptions.push_back("Finger Mid Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void TwitchScene::update() {
	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;

	float minStrength = 0.0 + EPSILON;
	float maxStrength = 1.0 - EPSILON;
	float strength;

	vector<int> twitchOptions;
	if (pinky->getValue()) twitchOptions.push_back(HandSkeleton::PINKY_BASE);
	if (ring->getValue()) twitchOptions.push_back(HandSkeleton::RING_BASE);
	if (middle->getValue()) twitchOptions.push_back(HandSkeleton::MIDDLE_BASE);
	if (index->getValue()) twitchOptions.push_back(HandSkeleton::INDEX_BASE);
	if (thumb->getValue()) twitchOptions.push_back(HandSkeleton::THUMB_BASE);

	if (twitchOptions.size() > 0) {
		float t = ofGetElapsedTimef();

		strength = choiceSigmoidStrength;
		strength = ofClamp(strength, minStrength, maxStrength);
		strength = 1 - strength;

		float choice = ofNoise(t, 1000);
		if (choice < 0.5) {
			choice = pow(2.0*choice, 1.0/strength)/ 2.0;
		}
		else {
			choice = 1.0 - (pow(2.0*(1.0-choice), 1.0/strength) / 2.0);
		}

		float sectionSize = 1.0 / (float)twitchOptions.size();
		int toTwitch;
		for (int i=0; i < twitchOptions.size(); i++) {
			if (choice < (i+1)*sectionSize) {
				toTwitch = twitchOptions[i];
				break;
			}
		}
				
		strength = twitchSigmoidStrength;
		strength = ofClamp(strength, minStrength, maxStrength);
		strength = 1 - strength;

		float angle = ofNoise(speedUp*t);
		if (angle < 0.5) {
			angle = pow(2.0*angle, 1.0/strength)/ 2.0;
		}
		else {
			angle = 1.0 - (pow(2.0*(1.0-angle), 1.0/strength) / 2.0);
		}

		if (angle >= minAngle) {
			handSkeleton->setRotation(toTwitch, angle, false, false);
		}
	}
}
void TwitchScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	HandSkeleton* handSkeleton = (HandSkeleton*)this->skeleton;
	HandSkeleton* immutableHandSkeleton = (HandSkeleton*)this->immutableSkeleton;

	ofVec2f xAxis(1, 0);

	const int fingerCount = 5;

	int wrist = HandSkeleton::WRIST;
	int palm = HandSkeleton::PALM;
	int base[] = {HandSkeleton::THUMB_BASE, HandSkeleton::INDEX_BASE, HandSkeleton::MIDDLE_BASE, HandSkeleton::RING_BASE, HandSkeleton::PINKY_BASE};
	int mid[] = {HandSkeleton::THUMB_MID, HandSkeleton::INDEX_MID, HandSkeleton::MIDDLE_MID, HandSkeleton::RING_MID, HandSkeleton::PINKY_MID};
	int top[] = {HandSkeleton::THUMB_TIP, HandSkeleton::INDEX_TIP, HandSkeleton::MIDDLE_TIP, HandSkeleton::RING_TIP, HandSkeleton::PINKY_TIP};

	ofVec2f origWristPos = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(wrist));
	ofVec2f origPalmPos = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(palm));
	ofVec2f origBasePos[fingerCount]; 
	ofVec2f origMidPos[fingerCount]; 
	ofVec2f origTopPos[fingerCount]; 
	for (int i=0; i < fingerCount; i++) {
		origBasePos[i] = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(base[i]));
		origMidPos[i] = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(mid[i]));
		origTopPos[i] = puppet->getOriginalMesh().getVertex(handSkeleton->getControlIndex(top[i]));
	}

	ofVec2f origPalmDir;
	ofVec2f origFingerDir;
	float curRot;
	float newRot;

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
				newRot = ofMap(mx, 0, 384, -(curRot+maxPalmAngleLeft), -(curRot));
			}
			else {
				newRot = ofMap(mx, 384, 768, -(curRot), -(curRot+maxPalmAngleRight));
			}

			handSkeleton->setRotation(palm, newRot, true, false);
			immutableHandSkeleton->setRotation(palm, newRot, true, false);
			break;
		case 2: // finger base rotation
			for (int i=0; i < fingerCount; i++) {
				origFingerDir = origMidPos[i] - origBasePos[i];
				curRot = origFingerDir.angle(xAxis);

				if (mx <= 384) {
					newRot = ofMap(mx, 0, 384, -(curRot+maxBaseAngleLeft), -(curRot));
				}
				else {
					newRot = ofMap(mx, 384, 768, -(curRot), -(curRot+maxBaseAngleRight));
				}

				handSkeleton->setRotation(base[i], newRot, true, false);
				immutableHandSkeleton->setRotation(base[i], newRot, true, false);
			}
			break;
		case 3: // finger mid rotation
			for (int i=0; i < fingerCount; i++) {
				origFingerDir = origTopPos[i] - origMidPos[i];
				curRot = origFingerDir.angle(xAxis);

				if (mx <= 384) {
					newRot = ofMap(mx, 0, 384, -(curRot+maxMidAngleLeft), -(curRot));
				}
				else {
					newRot = ofMap(mx, 384, 768, -(curRot), -(curRot+maxMidAngleRight));
				}

				handSkeleton->setRotation(mid[i], newRot, true, false);
				immutableHandSkeleton->setRotation(mid[i], newRot, true, false);
			}
			break;
	}
}
void TwitchScene::draw() {
}
