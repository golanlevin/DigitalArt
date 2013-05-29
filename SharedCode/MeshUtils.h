#pragma once

#include "ofxPuppet.h"

void removeTriangles(ofMesh& mesh, ofPolyline& region) {
	int n = mesh.getNumIndices();
	vector<ofIndexType> indices;
	for(int i = 0; i < n;) {
		ofIndexType i0 = mesh.getIndex(i++);
		ofIndexType i1 = mesh.getIndex(i++);
		ofIndexType i2 = mesh.getIndex(i++);
		ofVec2f v0 = mesh.getVertex(i0);
		ofVec2f v1 = mesh.getVertex(i1);
		ofVec2f v2 = mesh.getVertex(i2);
		ofVec2f avg = (v0 + v1 + v2) / 3.;
		if(!region.inside(avg)) {
			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);
		}
	}
	mesh.clearIndices();
	mesh.addIndices(indices);
}

ofMesh dropUnusedVertices(ofMesh& mesh) {
	int n = mesh.getNumIndices();
	ofMesh out;
	vector<bool> used(n);
	for(int i = 0; i < n; i++) {
		used[mesh.getIndex(i)] = true;
	}
	int total = 0;
	vector<ofIndexType> newIndex(n);
	for(int i = 0; i < n; i++) {
		if(used[i]) {
			newIndex[i] = total++;
			out.addVertex(mesh.getVertex(i));
			if(mesh.hasTexCoords()) {
				out.addTexCoord(mesh.getTexCoord(i));
			}
		}
	}
	for(int i = 0; i < n; i++) {
		out.addIndex(newIndex[mesh.getIndex(i)]);
	}
	return out;
}

void mergeCoincidentVertices(ofMesh& mesh, float epsilon = 10e-5) {
	int n = mesh.getNumVertices();
	vector<ofIndexType> newIndex(n);
	float epsilonSquared = epsilon * epsilon;
	for(int i = 0; i < n; i++) {
		ofVec2f cur = mesh.getVertex(i);
		newIndex[i] = i;
		for(int j = 0; j < i; j++) {
			ofVec2f other = mesh.getVertex(j);
			if(cur.squareDistance(other) < epsilonSquared) {
				newIndex[i] = j;
				break;
			}
		}
	}
	int m = mesh.getNumIndices();
	for(int i = 0; i < m; i++) {
		ofIndexType index = mesh.getIndex(i);
		mesh.setIndex(i, newIndex[index]);
	}
}

ofMesh removeAndStitch(ofMesh& mesh, ofPolyline& removalRegion, vector<pair<ofIndexType, ofIndexType> >& stitch) {
	ofMesh out = mesh;
	removeTriangles(out, removalRegion);
	out = dropUnusedVertices(out);
	ofxPuppet puppet;
	puppet.setup(out);
	puppet.update();
	for(int i = 0; i < stitch.size(); i++) {
		ofIndexType left = stitch[i].first, right = stitch[i].second;
		ofVec2f avg = (out.getVertex(left) + out.getVertex(right)) / 2;
		puppet.setControlPoint(left, avg);
		puppet.setControlPoint(right, avg);
	}
	puppet.update();
	out = puppet.getDeformedMesh();
	mergeCoincidentVertices(out);
	return dropUnusedVertices(out);
}

bool isLeft(ofVec2f a, ofVec2f b, ofVec2f c){
	return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)) > 0;
}

void split(ofMesh& mesh, vector<ofIndexType>& indices) {
	int n = mesh.getNumIndices();
	vector<ofIndexType> newIndices;
	for(int i = 0; i < indices.size(); i++) {
		newIndices.push_back(mesh.getNumVertices());
		int index = indices[i];
		mesh.addVertex(mesh.getVertex(index));
		if(mesh.hasTexCoords()) {
			mesh.addTexCoord(mesh.getTexCoord(index));
		}
	}
	cout << ofToString(newIndices) << endl;
	for(int i = 0; i < n; i += 3) {
		ofIndexType i0 = mesh.getIndex(i + 0);
		ofIndexType i1 = mesh.getIndex(i + 1);
		ofIndexType i2 = mesh.getIndex(i + 2);
		
		for(int j = 0; j < indices.size(); j++) {
			ofVec2f first, second, adjacent;
			
			int si = indices[j];
			if(si == i0 || si == i1 || si == i2) {
				int diffIndex = i0;
				if(si != i0) {
					diffIndex = i0;
				} else if(si != i1) {
					diffIndex = i1;
				} else if(si != i2) {
					diffIndex = i2;
				}
				adjacent = mesh.getVertex(diffIndex);
				
				if(j + 1 < indices.size()) {
					first = mesh.getVertex(indices[j]);
					second = mesh.getVertex(indices[j + 1]);
				} else {
					first = mesh.getVertex(indices[j - 1]);
					second = mesh.getVertex(indices[j]);
				}
				
				if(isLeft(first, second, adjacent)) {
					int sameIndex;
					if(si == i0) {
						sameIndex = 0;
					} else if(si == i1) {
						sameIndex = 1;
					} else if(si == i2) {
						sameIndex = 2;
					}
					mesh.setIndex(i + sameIndex, newIndices[j]);
				}
			}
		}
	}
}
