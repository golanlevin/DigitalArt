#pragma once

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
	string getLabelName() {
		return getLabelName(label);
	}
	Label label;
	bool forwardOriented;
};

class Skeleton {
protected:
	vector<Bone> bones;
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
			setPositionAbsolute((Bone::Label) i, curPosition);
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
			bones[i].draw();
		}
		for(int i = 0; i < size(); i++) {
			ofVec2f cur = bones[i].getGlobalPosition();
			//ofDrawBitmapStringHighlight(Bone::getLabelName((Bone::Label) i), cur);
		}
		ofPopStyle();
	}
	ofVec2f getPositionAbsolute(Bone::Label label) {
		return bones[label].getGlobalPosition();
	}
	void setPositionAbsolute(Bone::Label label, ofVec2f position) {
		bones[label].setGlobalPosition(position);
	}
	void setPositionRelativeToSelf(Bone::Label label, ofVec2f position) {
		setPositionAbsolute(label, position + getPositionAbsolute(label));
	}
	void setPositionRelativeToParent(Bone::Label label, ofVec2f position) {
		bones[label].setPosition(position);
	}
	void setRotation(Bone::Label label, float rotation) {
		bones[label].setOrientation(ofVec3f(0, 0, rotation));
	}
};
