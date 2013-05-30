#pragma once

#include "TwitchScene.h"

#define EPSILON 0.001

TwitchScene::TwitchScene(ofxPuppet* puppet, HandSkeleton* handSkeleton, HandSkeleton* immutableHandSkeleton) {
	Scene::Scene();
	Scene::setup("Twitch", puppet, (Skeleton*)handSkeleton, (Skeleton*)immutableHandSkeleton);

	this->twitchSigmoidStrength = 0.8;
	this->choiceSigmoidStrength = 0.5;
	this->speedUp = 5;
	this->minAngle = 0.9;
}
void TwitchScene::setupGui() {
	TwitchScene::initializeGui();

	this->gui->addSlider("Twitch Sigmoid Strength", 0, 0.99, &twitchSigmoidStrength);
	this->gui->addSlider("Choice Sigmoid Strength", 0, 0.99, &choiceSigmoidStrength);
	this->gui->addSlider("Speed Up", 1, 10, &speedUp);
	this->gui->addSlider("Min Angle", 0, 1, &minAngle);
	this->pinky = this->gui->addToggle("Pinky", true);
	this->ring = this->gui->addToggle("Ring", true);
	this->middle = this->gui->addToggle("Middle", false);
	this->index = this->gui->addToggle("Index", false);
	this->thumb = this->gui->addToggle("Thumb", false);

	this->gui->autoSizeToFitWidgets();
}
void TwitchScene::setupMouseGui() {
	TwitchScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);

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

	switch(getSelection(mouseRadio)) {
		case 0: // palm position
			handSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			immutableHandSkeleton->setPosition(HandSkeleton::PALM, mouse, true);
			break;
		case 1: // palm rotation
			break;
	}
}
void TwitchScene::draw() {
}
