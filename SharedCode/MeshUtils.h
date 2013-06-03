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
	mesh.mergeDuplicateVertices();
	return;
	
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
	mergeCoincidentVertices(out); // could do this using stitch pairs instead?
	return dropUnusedVertices(out);
}

bool isLeft(ofVec2f a, ofVec2f b, ofVec2f c){
	return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)) > 0;
}

ofVec2f closestPointOnLine(const ofVec2f& p1, const ofVec2f& p2, const ofVec2f& p3) {
	if(p1 == p2) {
		return p1;
	}
	float u = (p3.x - p1.x) * (p2.x - p1.x);
	u += (p3.y - p1.y) * (p2.y - p1.y);
	float len = (p2 - p1).length();
	u /= (len * len);
	if(u > 1) {
		u = 1;
	} else if(u < 0) {
		u = 0;
	}
	return p1.getInterpolated(p2, u);
}

bool sideTest(ofPolyline& polyline, ofVec2f position) {
	ofPolyline reference = polyline.getResampledBySpacing(8);
	reference = reference.getSmoothed(4);
	ofVec2f a, b;
	float bestDistance = 0;
	for(int i = 0; i + 1 < reference.size(); i++) {
		ofVec2f v0 = reference[i], v1 = reference[i + 1];
		ofVec2f closest = closestPointOnLine(v0, v1, position);
		float distance = closest.distance(position);
		if(i == 0 || distance < bestDistance) {
			bestDistance = distance;
			a = v0, b = v1;
		}
	}
	if(bestDistance == 0) {
		return true;
	} else {
		return isLeft(a, b, position);
	}
}

void split(ofMesh& mesh, vector<ofIndexType>& indices) {
	int n = mesh.getNumIndices();
	vector<ofIndexType> newIndices;
	ofPolyline polyline;
	for(int i = 0; i < indices.size(); i++) {
		polyline.addVertex(mesh.getVertex(indices[i]));
	}
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
			if(i0 == indices[j] || i1 == indices[j] || i2 == indices[j]) {
				ofVec2f vi0 = mesh.getVertex(i0);
				ofVec2f vi1 = mesh.getVertex(i1);
				ofVec2f vi2 = mesh.getVertex(i2);
				ofVec2f avg = (vi0 + vi1 + vi2) / 3;
				
				if(sideTest(polyline, avg)) {
					for(int k = 0; k < 3; k++) {
						if(mesh.getIndex(i + k) == indices[j]) {
							mesh.setIndex(i + k, newIndices[j]);							
						}
					}
				}
			}
		}
	}
}
