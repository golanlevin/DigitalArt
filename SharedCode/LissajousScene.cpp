#pragma once

#include "LissajousScene.h"

LissajousScene::LissajousScene(ofxPuppet* puppet, ThreePointSkeleton* threePointSkeleton, ThreePointSkeleton* immutableThreePointSkeleton) {
	Scene::Scene();
	Scene::setup("Lissajous", "Lissajous (Three Point)", puppet, (Skeleton*)threePointSkeleton, (Skeleton*)immutableThreePointSkeleton);

	this->maxPalmAngleLeft = 60;
	this->maxPalmAngleRight = -60;

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
	this->gui->addSpacer();
	this->gui->addSlider("Frequency", 0, 5, &frequency);
	this->gui->addSpacer();
	this->gui->addLabel("Lissajous Style Options", 2);
	this->lissajousRadio = this->gui->addRadio("Lissajous Style Options", lissajousStyle);
	this->lissajousRadio->getToggles()[0]->setValue(true);
	this->gui->addSpacer();

	this->gui->autoSizeToFitWidgets();
}
void LissajousScene::setupMouseGui() {
	LissajousScene::initializeMouseGui();

	vector<string> mouseOptions;
	mouseOptions.push_back("Palm Position");
	mouseOptions.push_back("Palm Rotation");
	this->mouseRadio = this->mouseGui->addRadio("Mouse Control Options", mouseOptions);
	this->mouseRadio->getToggles()[0]->setValue(true);
	this->mouseGui->addSpacer();

	this->mouseGui->autoSizeToFitWidgets();
}
void LissajousScene::update() {
	ThreePointSkeleton* threePointSkeleton = (ThreePointSkeleton*)this->skeleton;

	float timeVal = ofGetElapsedTimef();
	if (bUseFrameBasedAnimation){
		timeVal = (float)ofGetFrameNum()/ 60.0;
	}

	float t = frequency * timeVal * TWO_PI;
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
	ThreePointSkeleton* immutableThreePointSkeleton = (ThreePointSkeleton*)this->immutableSkeleton;

	switch(getSelection(mouseRadio)) {
		case 0: // palm position
			threePointSkeleton->setPosition(ThreePointSkeleton::PALM, mouse, true);
			immutableThreePointSkeleton->setPosition(ThreePointSkeleton::PALM, mouse, true);
			break;
		case 1: // palm rotation
			ofVec2f xAxis(1, 0);

			int wrist = ThreePointSkeleton::PALM;
			int palm = ThreePointSkeleton::MIDDLE_HAND;

			ofVec2f origWristPos = puppet->getOriginalMesh().getVertex(threePointSkeleton->getControlIndex(wrist));
			ofVec2f origPalmPos = puppet->getOriginalMesh().getVertex(threePointSkeleton->getControlIndex(palm));

			ofVec2f origPalmDir = origPalmPos - origWristPos;
			
			float curRot = origPalmDir.angle(xAxis);

			float newRot;
			if (mx <= 384) {
				newRot = ofMap(mx, 0, 384, -(curRot+maxPalmAngleLeft), -(curRot));
			}
			else {
				newRot = ofMap(mx, 384, 768, -(curRot), -(curRot+maxPalmAngleRight));
			}

			threePointSkeleton->setRotation(palm, newRot, true, false);
			immutableThreePointSkeleton->setRotation(palm, newRot, true, false);
			break;
	}
}
void LissajousScene::draw() {
}