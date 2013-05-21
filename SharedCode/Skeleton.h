#pragma once

#include "ofMain.h"

class Bone : public ofNode {
public:
	enum Label {
		WRIST = 0,
		PALM,
		PINKY_BASE, PINKY_MID, PINKY_TIP,
		RING_BASE, RING_MID, RING_TIP,
		MIDDLE_BASE, MIDDLE_MID, MIDDLE_TIP,
		INDEX_BASE, INDEX_MID, INDEX_TIP,
		THUMB_BASE, THUMB_MID, THUMB_TIP,
		BONE_COUNT,
		NONE
	};
	static string getLabelName(Label label) {
		switch(label) {
			case WRIST: return "WRIST";
			case PALM: return "PALM";
			case PINKY_BASE: return "PINKY_BASE"; case PINKY_MID: return "PINKY_MID"; case PINKY_TIP: return "PINKY_TIP";
			case RING_BASE: return "RING_BASE"; case RING_MID: return "RING_MID"; case RING_TIP: return "RING_TIP";
			case MIDDLE_BASE: return "MIDDLE_BASE"; case MIDDLE_MID: return "MIDDLE_MID"; case MIDDLE_TIP: return "MIDDLE_TIP";
			case INDEX_BASE: return "INDEX_BASE"; case INDEX_MID: return "INDEX_MID"; case INDEX_TIP: return "INDEX_TIP";
			case THUMB_BASE: return "THUMB_BASE"; case THUMB_MID: return "THUMB_MID"; case THUMB_TIP: return "THUMB_TIP";
			case BONE_COUNT: return "BONE_COUNT";
			case NONE: return "NONE";
		}
	}
	void customDraw() {
		ofPushStyle();
		ofSetColor(ofColor::red);
		ofLine(0, 0, 20, 0);
		ofSetColor(ofColor::green);
		ofLine(0, 0, 0, 20);
		ofPopStyle();
	}
	string getLabelName() {
		return getLabelName(label);
	}
	Label label;
	bool forwardOriented;
};

// add specialized skeletons: hand skeleton, three point skeleton

class Skeleton {
protected:
	vector<Bone> bones;
	vector<Bone::Label> cachedChildren;
	vector<ofVec2f> cachedPositions;
	
public:	
	void setup(ofMesh& mesh) {
		bones.clear();
		bones.resize(Bone::BONE_COUNT);
		Bone::Label topology[] = {
			Bone::NONE,
			Bone::WRIST,
			Bone::PALM, Bone::PINKY_BASE, Bone::PINKY_MID,
			Bone::PALM, Bone::RING_BASE, Bone::RING_MID,
			Bone::PALM, Bone::MIDDLE_BASE, Bone::MIDDLE_MID,
			Bone::PALM, Bone::INDEX_BASE, Bone::INDEX_MID,
			Bone::PALM, Bone::THUMB_BASE, Bone::THUMB_MID
		};
		bool forwardOriented[] = {
			true,
			false,
			true, true, false,
			true, true, false,
			true, true, false,
			true, true, false,
			true, true, false
		};
		for(int i = 0; i < size(); i++) {
			bones[i].label = (Bone::Label) i;
			bones[i].forwardOriented = forwardOriented[i];
			if(topology[i] != Bone::NONE) {
				bones[i].setParent(bones[topology[i]]);
			}
		}
		for(int i = 0; i < size(); i++) {
			ofVec2f curPosition = mesh.getVertex(i);
			Bone& cur = bones[i];
			if(cur.getParent() != NULL) {
				Bone& parent = *((Bone*) cur.getParent());
				ofVec2f parentPosition = parent.getGlobalPosition();
				float rotation = ofVec2f(1, 0).angle(curPosition - parentPosition);
				ofQuaternion orientation;
				orientation.makeRotate(rotation, 0, 0, 1);
				if(parent.forwardOriented) {
					parent.setGlobalOrientation(orientation);
				}
				if(!cur.forwardOriented) {
					cur.setGlobalOrientation(orientation);
				}
			} 
			setPosition((Bone::Label) i, curPosition, true);
		}
	}
	int size() {
		return bones.size();
	}
	void draw() {
		ofPushStyle();
		ofFill();
		ofSetLineWidth(2);
		for(int i = 0; i < size(); i++) {
			Bone& cur = bones[i];
			ofSetColor(ofColor::green);
			ofCircle(cur.getGlobalPosition(), 3);
			ofSetColor(ofColor::white);
			if(cur.getParent() != NULL) {
				ofNode& parent = *(cur.getParent());
				ofLine(cur.getGlobalPosition(), parent.getGlobalPosition());
			}
		}
		for(int i = 0; i < size(); i++) {
			ofVec2f cur = bones[i].getGlobalPosition();
			//ofDrawBitmapStringHighlight(Bone::getLabelName((Bone::Label) i), cur);
			bones[i].draw();
		}
		ofPopStyle();
	}
	ofVec2f getPositionAbsolute(Bone::Label label) {
		return bones[label].getGlobalPosition();
	}
	Bone& getBone(Bone::Label label) {
		return bones[label];
	}
	void setBoneLength(Bone::Label label, float distance) {
		Bone& bone = bones[label];
		if(bone.getParent() != NULL) {
			bone.setGlobalPosition(bone.getParent()->getGlobalPosition());
			bone.move(distance, 0, 0);
		}
	}
	void stashChildren(Bone::Label label) {
		Bone& bone = bones[label];
		for(int i = 0; i < size(); i++) {
			Bone* parent = (Bone*) bones[i].getParent();
			if(parent == &bone) {
				cachedChildren.push_back((Bone::Label) i);
				cachedPositions.push_back(bones[i].getGlobalPosition());
			}
		}
	}
	void applyChildren() {
		for(int i = 0; i < cachedChildren.size(); i++) {
			bones[cachedChildren[i]].setGlobalPosition(cachedPositions[i]);
		}		
	}
	void setPosition(Bone::Label label, ofVec2f position, bool absolute = true, bool independent = false) {
		Bone& bone = bones[label];
		if(independent) {
			stashChildren(label);
		}
		if(absolute) {
			bone.setGlobalPosition(position);
		} else {
			bone.setGlobalPosition(bone.getGlobalPosition() + position);
		}
		if(independent) {
			applyChildren();
		}
	}
	void setRotation(Bone::Label label, float rotation, bool absolute = false) {
		if(absolute) {
			ofQuaternion orientation;
			orientation.makeRotate(rotation, 0, 0, 1);
			bones[label].setGlobalOrientation(orientation);
		} else {
			bones[label].setOrientation(ofVec3f(0, 0, rotation));
		}
	}
};
