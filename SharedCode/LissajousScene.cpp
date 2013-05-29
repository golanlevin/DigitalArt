#pragma once

#include "LissajousScene.h"

LissajousScene::LissajousScene(ofxPuppet* puppet, ThreePointSkeleton* threePointSkeleton, ThreePointSkeleton* immutableThreePointSkeleton) {
	Scene::Scene();
	Scene::setup("Lissajous", puppet, (Skeleton*)threePointSkeleton, (Skeleton*)immutableThreePointSkeleton);

	this->amplitude = 10;
	this->frequency = 1;
	this->lissajousStyle.push_back("Circle");
	this->lissajousStyle.push_back("Ellipse");
	this->lissajousStyle.push_back("Parabola");
	this->lissajousStyle.push_back("Bow Tie");
	this->lissajousStyle.push_back("Vertical Line");
}
void LissajousScene::setupGui() {
	LissajousScene::initializeGui();

	this->gui->addSlider("Amplitude", 0, 100, &amplitude);
	this->gui->addSlider("Frequency", 0, 5, &frequency);
	this->lissajousRadio = this->gui->addRadio("Lissajous Style Options", lissajousStyle);
	this->lissajousRadio->getToggles()[0]->setValue(true);

	this->gui->autoSizeToFitWidgets();
}
void LissajousScene::setupMouseGui() {
	LissajousScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);

	this->mouseGui->autoSizeToFitWidgets();
}
void LissajousScene::update() {
	ThreePointSkeleton* threePointSkeleton = (ThreePointSkeleton*)this->skeleton;

	float t = frequency * ofGetElapsedTimef() * TWO_PI;
	ofVec2f position;
	switch(getSelection(lissajousRadio)) {
		case 0: // circle
			position.set(sin(t + PI/2), sin(t));
			break;
		case 1: // ellipse
			position.set(sin(t + PI/4), sin(t));
			break;
		case 2: // parabola
			position.set(sin(t + PI/2), sin(2*t));
			break;
		case 3: // bow tie
			position.set(sin(2*t), sin(t));
			break;
		case 4: // vertical line
			position.set(0, sin(t));
			break;
	}
	threePointSkeleton->setPosition(ThreePointSkeleton::MIDDLE_HAND, amplitude * position, false, true);
}
void LissajousScene::updateMouse(float mx, float my) {
	ofVec2f mouse(mx, my);

	ThreePointSkeleton* threePointSkeleton = (ThreePointSkeleton*)this->skeleton;
	ThreePointSkeleton* immutableHandSkeleton = (ThreePointSkeleton*)this->immutableSkeleton;

	switch(getSelection(mouseRadio)) {
		case 0: // palm position
			threePointSkeleton->setPosition(ThreePointSkeleton::PALM, mouse, true);
			immutableHandSkeleton->setPosition(ThreePointSkeleton::PALM, mouse, true);
			break;
		case 1: // palm rotation
			break;
	}
}
void LissajousScene::draw() {
}