#pragma once

#include "Skeleton.h"

class PalmSkeleton : public Skeleton {
public:
	enum Label {
		BASE = 0,
		CENTROID,
		TOP,
		RIGHT_BASE, RIGHT_MID, RIGHT_TOP,
		LEFT_BASE, LEFT_MID, LEFT_TOP
	};
	
	void setup(ofMesh& mesh) {
		int boneCount = 9;
		
		/*
		// old
		int controlIndicesRaw[] = {
			1, 79, 8,
			118, 184, 85,
			122, 125, 97
		};
		*/
		
		// NEW (actual)
		int controlIndicesRaw[] = {
			293, 367, 289,
			325, 326, 336,
			345, 348, 356 
		};
		
		
		int parentsRaw[] = {
			-1, BASE, CENTROID,
			CENTROID, CENTROID, CENTROID,
			CENTROID, CENTROID, CENTROID
		};
		bool forwardOrientedRaw[] = {
			true, false, false,
			false, false, false,
			false, false, false
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