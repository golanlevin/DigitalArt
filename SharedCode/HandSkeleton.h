#pragma once

#include "Skeleton.h"

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
		int controlIndicesRaw[] = {
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
		vector<int> controlIndices, parents;
		vector<bool> forwardOriented;
		for(int i = 0; i < boneCount; i++) {
			controlIndices.push_back(controlIndicesRaw[i]);
			parents.push_back(parentsRaw[i]);
			forwardOriented.push_back(forwardOrientedRaw[i]);
		}
		Skeleton::setup(mesh, controlIndices, parents, forwardOriented);
	}
};
