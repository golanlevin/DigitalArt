#pragma once

#include "Skeleton.h"

class HandWithFingertipsSkeleton : public Skeleton {
public:
	enum Label {
		WRIST = 0,
		PALM,
		PINKY_BASE, PINKY_MID, PINKY_TOP, PINKY_TIP,
		RING_BASE, RING_MID, RING_TOP, RING_TIP,
		MIDDLE_BASE, MIDDLE_MID, MIDDLE_TOP, MIDDLE_TIP,
		INDEX_BASE, INDEX_MID, INDEX_TOP, INDEX_TIP,
		THUMB_BASE, THUMB_MID, THUMB_TOP, THUMB_TIP
	};
	
	void setup(ofMesh& mesh) {
		int boneCount = 22;
		
		/*
		 // These are for the old (CG) hand
		int controlIndicesRaw[] = {
			0, 1,
			2, 3, 4, 188,
			5, 6, 7, 226,
			8, 9, 10, 270,
			11, 12, 13, 308,
			14, 15, 16, 161
		};
		*/
		
		// These are for the new (Generated!) hand
		int controlIndicesRaw[] = {
			293, 366,
			287, 80,  100, 110,
			288, 137, 157, 167,
			289, 194, 214, 224, 
			290, 251, 271, 281,
			320, 2,   27,  47
		};
		
		
		int parentsRaw[] = {
			-1, WRIST,
			PALM, PINKY_BASE, PINKY_MID, PINKY_TOP,
			PALM, RING_BASE, RING_MID, RING_TOP,
			PALM, MIDDLE_BASE, MIDDLE_MID, MIDDLE_TOP,
			PALM, INDEX_BASE, INDEX_MID, INDEX_TOP,
			PALM, THUMB_BASE, THUMB_MID, THUMB_TOP
		};
		bool forwardOrientedRaw[] = {
			true, false,
			true, true, true, false,
			true, true, true, false,
			true, true, true, false,
			true, true, true, false,
			true, true, true, false
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
