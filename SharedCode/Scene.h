#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxPuppet.h"
#include "Skeleton.h"

class Scene {
protected:
	string name, nameWithSkeleton;
	ofxPuppet* puppet;
	Skeleton *skeleton, *immutableSkeleton;
	ofxUICanvas *gui, *mouseGui;
	ofxUIRadio* mouseRadio;

	bool beginSceneTimeSet;
	float beginSceneTime;
	bool bUseFrameBasedAnimation;

	bool startShowImage, startShowWireframe, startShowSkeleton, startMouseControl;

public:	
	Scene() {
		this->name               = "";
		this->nameWithSkeleton   = "";
		this->puppet             = NULL;
		this->skeleton           = NULL;
		this->immutableSkeleton  = NULL;

		this->beginSceneTimeSet  = false;
		this->beginSceneTime     = 0;

		this->startMouseControl  = true;
		this->startShowWireframe = false;
		this->startShowSkeleton  = true;
		this->startMouseControl  = false;
		this->bUseFrameBasedAnimation = false;
	}
	void setup(string name, string nameWithSkeleton, ofxPuppet* puppet, Skeleton* skeleton, Skeleton* immutableSkeleton) {
		this->name = name;
		this->nameWithSkeleton = nameWithSkeleton;
		this->puppet = puppet;
		this->skeleton = skeleton;
		this->immutableSkeleton = immutableSkeleton;

		this->beginSceneTimeSet = false;
		this->beginSceneTime = 0;

		this->startMouseControl = true;
		this->startShowWireframe = false;
		this->startShowSkeleton = false;
		this->startMouseControl = false;
	}
	void initializeGui() {
		gui = new ofxUICanvas();
		gui->setFont("GUI/NewMedia Fett.ttf");
		gui->setPosition(540, 0);
		gui->setVisible(false);
		gui->addLabel("Scene Adjustment Options");
		gui->addSpacer();
	}
	virtual void setupGui(){}
	void initializeMouseGui() {
		mouseGui = new ofxUICanvas();
		mouseGui->setFont("GUI/NewMedia Fett.ttf");
		mouseGui->setPosition(320, 0);
		mouseGui->setVisible(false);
		mouseGui->addLabel("Mouse Control Options");
		mouseGui->addSpacer();
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
	string getNameWithSkeleton() {
		return this->nameWithSkeleton;
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
	bool isStartShowImage() {
		return this->startShowImage;
	}
	bool isStartShowWireframe() {
		return this->startShowWireframe;
	}
	bool isStartShowSkeleton() {
		return this->startShowSkeleton;
	}
	bool isStartMouseControl() {
		return this->startMouseControl;
	}
	bool isUsingFrameBasedAnimation(){
		return this->bUseFrameBasedAnimation;
	}
	void setFrameBasedAnimation (bool fba){
		this->bUseFrameBasedAnimation = fba;
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