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
	Label label;
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
		for(int i = 0; i < size(); i++) {
			bones[i].label = (Bone::Label) i;
			if(topology[i] != Bone::NONE) {
				bones[i].setParent(bones[topology[i]]);
			}
		}
		for(int i = 0; i < size(); i++) {
			setPositionAbsolute((Bone::Label) i, mesh.getVertex(i));
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
			ofSetColor(ofColor::green);
			ofCircle(bones[i].getPosition(), 3);
			ofSetColor(ofColor::white);
			Bone& cur = bones[i];
			if(cur.getParent() != NULL) {
				ofNode& parent = *(cur.getParent());
				ofLine(cur.getGlobalPosition(), parent.getGlobalPosition());
			}
			//bones[i].draw();
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
