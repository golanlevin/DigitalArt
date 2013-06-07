#pragma once

#include "Skeleton.h"

class WristSpineSkeleton : public Skeleton {
public:
	enum Label {
		HAND_TOP,
		HAND_MID,
		HAND_BASE,
		PALM,
		WRIST_TOP,
		WRIST_MID,
		WRIST_BASE
	};
	
	void setup(ofMesh& mesh) {
		int boneCount = 7;
		
		/*
		int controlIndicesRaw[] = {
			8, 104, 81, 1, 143, 144, 145
		};
		*/
		
		// new
		int controlIndicesRaw[] = {
			289, 366, 363, 293, 298, 303, 308
		};
		
		int parentsRaw[] = {
			-1, HAND_TOP, HAND_MID,
			HAND_BASE, PALM, WRIST_TOP, HAND_TOP,
		};
		bool forwardOrientedRaw[] = {
			true, false, false, false, false, false, false
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