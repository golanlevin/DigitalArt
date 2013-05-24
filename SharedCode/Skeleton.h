#pragma once

#include "ofMain.h"
#include "Bone.h"

class Skeleton {
protected:
	vector<Bone> bones;
	vector<int> cachedChildren;
	vector<ofVec2f> cachedPositions;
	vector<int> controlIndices;
	
public:	
	void setup(ofMesh& mesh, vector<int>& controlIndices, vector<int>& parents, vector<bool>& forwardOriented) {
		this->controlIndices = controlIndices;
		bones.clear();
		bones.resize(controlIndices.size());
		for(int i = 0; i < size(); i++) {
			bones[i].forwardOriented = forwardOriented[i];
			if(parents[i] > -1) {
				bones[i].setParent(bones[parents[i]]);
			}
		}
		for(int i = 0; i < size(); i++) {
			int controlPoint = controlIndices[i];
			ofVec2f curPosition = mesh.getVertex(controlPoint);
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
	int getControlIndex(int i) {
		return controlIndices[i];
	}
	vector<int>& getControlIndices() {
		return controlIndices;
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
			//ofVec2f dir = bone.getGlobalPosition() - bone.getParent()->getGlobalPosition();
			//dir.normalize();
			//dir *= distance;
			
			bone.setGlobalPosition(bone.getParent()->getGlobalPosition());
			
			//bone.move(dir);
			
			bone.move(distance, 0, 0);
		}
	}
	void setBoneLength(int i, ofVec2f distance) {
		Bone& bone = bones[i];
		if (bone.getParent() != NULL) {
			bone.setGlobalPosition(bone.getParent()->getGlobalPosition());
			//bone.move(distance);
			setPosition(i, distance, false, false);
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
	void setRotation(int i, float rotation, bool absolute = false, bool independent = false) {
		if(independent) {
			stashChildren(i);
		}
		if(absolute) {
			ofQuaternion orientation;
			orientation.makeRotate(rotation, 0, 0, 1);
			bones[i].setGlobalOrientation(orientation);
		} else {
			bones[i].rotate(rotation, ofVec3f(0, 0, 1));
		}
		if(independent) {
			applyChildren();
		}
	}
};