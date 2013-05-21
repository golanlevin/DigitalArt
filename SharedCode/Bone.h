#pragma once

#include "ofMain.h"

class Bone : public ofNode {
public:
	void customDraw() {
		ofPushStyle();
		ofSetColor(ofColor::red);
		ofLine(0, 0, 20, 0);
		ofSetColor(ofColor::green);
		ofLine(0, 0, 0, 20);
		ofPopStyle();
	}
	bool forwardOriented;
};
