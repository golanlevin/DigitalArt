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
	int label;
	bool forwardOriented;
};

class Skeleton {
protected:
	vector<Bone> bones;
	vector<int> cachedChildren;
	vector<ofVec2f> cachedPositions;
	
public:	
	void setup(ofMesh& mesh, vector<int>& indices, vector<int>& parents, vector<bool>& forwardOriented) {
		bones.clear();
		bones.resize(indices.size());
		for(int i = 0; i < size(); i++) {
			bones[i].label = i;
			bones[i].forwardOriented = forwardOriented[i];
			if(parents[i] > -1) {
				bones[i].setParent(bones[parents[i]]);
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
			setPosition(i, curPosition, true);
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
			bones[i].draw();
		}
		ofPopStyle();
	}
	ofVec2f getPositionAbsolute(int i) {
		return bones[i].getGlobalPosition();
	}
	Bone& getBone(int i) {
		return bones[i];
	}
	void setBoneLength(int i, float distance) {
		Bone& bone = bones[i];
		if(bone.getParent() != NULL) {
			bone.setGlobalPosition(bone.getParent()->getGlobalPosition());
			bone.move(distance, 0, 0);
		}
	}
	void stashChildren(int i) {
		Bone& bone = bones[i];
		for(int j = 0; j < size(); j++) {
			Bone* parent = (Bone*) bones[j].getParent();
			if(parent == &bone) {
				cachedChildren.push_back(j);
				cachedPositions.push_back(bones[j].getGlobalPosition());
			}
		}
	}
	void applyChildren() {
		for(int i = 0; i < cachedChildren.size(); i++) {
			bones[cachedChildren[i]].setGlobalPosition(cachedPositions[i]);
		}		
	}
	void setPosition(int i, ofVec2f position, bool absolute = true, bool independent = false) {
		Bone& bone = bones[i];
		if(independent) {
			stashChildren(i);
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
	void setRotation(int i, float rotation, bool absolute = false) {
		if(absolute) {
			ofQuaternion orientation;
			orientation.makeRotate(rotation, 0, 0, 1);
			bones[i].setGlobalOrientation(orientation);
		} else {
			bones[i].setOrientation(ofVec3f(0, 0, rotation));
		}
	}
};

class HandSkeleton : public Skeleton {
public:
	enum Label {
		WRIST = 0,
		PALM,
		PINKY_BASE, PINKY_MID, PINKY_TIP,
		RING_BASE, RING_MID, RING_TIP,
		MIDDLE_BASE, MIDDLE_MID, MIDDLE_TIP,
		INDEX_BASE, INDEX_MID, INDEX_TIP,
		THUMB_BASE, THUMB_MID, THUMB_TIP
	};
	
	void setup(ofMesh& mesh) {
		int boneCount = 17;
		int indicesRaw[] = {
			0, 1,
			2, 3, 4,
			5, 6, 7,
			8, 9, 10,
			11, 12, 13,
			14, 15, 16
		};
		int parentsRaw[] = {
			-1, WRIST,
			PALM, PINKY_BASE, PINKY_MID,
			PALM, RING_BASE, RING_MID,
			PALM, MIDDLE_BASE, MIDDLE_MID,
			PALM, INDEX_BASE, INDEX_MID,
			PALM, THUMB_BASE, THUMB_MID
		};
		bool forwardOrientedRaw[] = {
			true, false,
			true, true, false,
			true, true, false,
			true, true, false,
			true, true, false,
			true, true, false
		};
		vector<int> indices, parents;
		vector<bool> forwardOriented;
		for(int i = 0; i < boneCount; i++) {
			indices.push_back(indicesRaw[i]);
			parents.push_back(parentsRaw[i]);
			forwardOriented.push_back(forwardOrientedRaw[i]);
		}
		Skeleton::setup(mesh, indices, parents, forwardOriented);
	}
};
