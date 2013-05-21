#pragma once

#include "Skeleton.h"

class ThreePointSkeleton : public Skeleton {
public:
	enum Label {
		PALM = 0,
		MIDDLE_HAND,
		MIDDLE_BASE
	};
	
	void setup(ofMesh& mesh) {
		int boneCount = 3;
		int controlIndicesRaw[] = {
			1, 104, 8
		};
		int parentsRaw[] = {
			-1, PALM, MIDDLE_HAND
		};
		bool forwardOrientedRaw[] = {
			true, false, false
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