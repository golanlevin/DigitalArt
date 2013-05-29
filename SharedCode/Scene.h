#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxPuppet.h"
#include "Skeleton.h"

class Scene {
protected:
	string name;
	ofxPuppet* puppet;
	Skeleton *skeleton, *immutableSkeleton;
	ofxUICanvas *gui, *mouseGui;
	ofxUIRadio* mouseRadio;

	bool beginSceneTimeSet;
	float beginSceneTime;

public:	
	Scene() {
		this->name = "";
		this->puppet = NULL;
		this->skeleton = NULL;
		this->immutableSkeleton = NULL;

		this->beginSceneTimeSet = false;
		this->beginSceneTime = 0;
	}
	void setup(string name, ofxPuppet* puppet, Skeleton* skeleton, Skeleton* immutableSkeleton) {
		this->name = name;
		this->puppet = puppet;
		this->skeleton = skeleton;
		this->immutableSkeleton = immutableSkeleton;

		this->beginSceneTimeSet = false;
		this->beginSceneTime = 0;
	}
	void initializeGui() {
		gui = new ofxUICanvas();
		gui->setFont("GUI/NewMedia Fett.ttf");
		gui->setPosition(555, 0);
		gui->setVisible(false);
	}
	virtual void setupGui(){}
	void initializeMouseGui() {
		mouseGui = new ofxUICanvas();
		mouseGui->setFont("GUI/NewMedia Fett.ttf");
		mouseGui->setPosition(0, 600);
		mouseGui->setVisible(false);
	}
	virtual void setupMouseGui() {}
	bool guiIsOn() {
		return this->gui->isVisible();
	}
	void turnOn() {
		if (!beginSceneTimeSet) {
			beginSceneTimeSet = true;
			beginSceneTime = ofGetElapsedTimef();
		}

		this->gui->setVisible(true);
	}
	void turnOff() {
		this->beginSceneTimeSet = false;

		this->gui->setVisible(false);
		this->turnOffMouse();
	}
	bool mouseGuiIsOn() {
		return this->mouseGui->isVisible();
	}
	void turnOnMouse() {
		this->mouseGui->setVisible(true);
	}
	void turnOffMouse() {
		this->mouseGui->setVisible(false);
	}
	virtual void update() {}
	virtual void updateMouse(float mx, float my) {}
	virtual void draw() {}
	string getName() {
		return this->name;
	}
	Skeleton* getSkeleton() {
		return this->skeleton;
	}
	bool isBeginSceneTimeSet() {
		return this->beginSceneTimeSet;
	}
	float getBeginSceneTime() {
		return this->beginSceneTime;
	}
	int getSelection(ofxUIRadio* radio) {
		vector<ofxUIToggle*> toggles = radio->getToggles();
		for(int i = 0; i < toggles.size(); i++) {
			if(toggles[i]->getValue()) {
				return i;
			}
		}
		return -1;
	}
};