#pragma once

#include "ofxPuppet.h"

void addAttributes(const ofMesh& src, int index, ofMesh& dst) {
    if(src.hasVertices()) {
        dst.addVertex(src.getVertex(index));
    }
    if(src.hasTexCoords()) {
        dst.addTexCoord(src.getTexCoord(index));
    }
    if(src.hasNormals()) {
        dst.addNormal(src.getNormal(index));
    }
    if(src.hasColors()) {
        dst.addColor(src.getColor(index));
    }
}

void orientMesh(ofMesh& mesh,
                ofVec2f fromStart, ofVec2f fromEnd,
                ofVec2f toStart, ofVec2f toEnd) {
    ofVec2f translation = toStart - fromStart;
    ofVec2f fromVec = fromEnd - fromStart;
    ofVec2f toVec = toEnd - toStart;
    float angle = fromVec.angle(toVec); // degrees
    ofVec3f z(0, 0, 1);
    for(int i = 0; i < mesh.getNumVertices(); i++) {
        ofVec3f& cur = mesh.getVertices()[i];
        cur.rotate(angle, fromStart, z);
        cur += translation;
    }
}

void orientPolyline(ofPolyline& polyline,
                    ofVec2f fromStart, ofVec2f fromEnd,
                    ofVec2f toStart, ofVec2f toEnd) {
    ofVec2f translation = toStart - fromStart;
    ofVec2f fromVec = fromEnd - fromStart;
    ofVec2f toVec = toEnd - toStart;
    float angle = fromVec.angle(toVec); // degrees
    ofVec3f z(0, 0, 1);
    for(int i = 0; i < polyline.size(); i++) {
        ofVec3f& cur = polyline[i];
        cur.rotate(angle, fromStart, z);
        cur += translation;
    }
}

ofPolyline buildPolyline(ofMesh& mesh, int indices[], int count) {
    ofPolyline line;
    for(int i = 0; i < count; i++) {
        line.addVertex(mesh.getVertex(indices[i]));
    }
    return line;
}

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

// preserves ordering of vertices and indices
ofMesh dropUnusedVertices(ofMesh& mesh) {
    ofMesh out;
    int nv = mesh.getNumVertices();
    int ni = mesh.getNumIndices();
    // mark all used vertices
    vector<bool> used(nv, false);
	for(int i = 0; i < ni; i++) {
        int oldIndex = mesh.getIndex(i);
        used[oldIndex] = true;
    }
    // add vertices and track new indices
    int total = 0;
    vector<ofIndexType> newIndex(nv);
    for(int i = 0; i < nv; i++) {
        if(used[i]) {
            addAttributes(mesh, i, out);
            newIndex[i] = total++;
        }
    }
    // add all new indices
    for(int i = 0; i < ni; i++) {
        int oldIndex = mesh.getIndex(i);
        out.addIndex(newIndex[oldIndex]);
    }
	return out;
}

ofMesh copySubmesh(ofMesh& mesh, ofPolyline& region) {
    ofMesh copyMesh;
    copyMesh = mesh;
    copyMesh.clearIndices();
    for(int i = 0; i < mesh.getNumIndices(); i += 3) {
        int i0 = mesh.getIndex(i + 0), i1 = mesh.getIndex(i + 1), i2 = mesh.getIndex(i + 2);
        ofVec2f vi0 = mesh.getVertex(i0), vi1 = mesh.getVertex(i1), vi2 = mesh.getVertex(i2);
        ofVec2f avg = (vi0 + vi1 + vi2) / 3;
        if(region.inside(avg)) {
            copyMesh.addIndex(i0);
            copyMesh.addIndex(i1);
            copyMesh.addIndex(i2);
        }
    }
    return dropUnusedVertices(copyMesh);
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

ofMesh stitch(ofMesh& mesh, vector<pair<ofIndexType, ofIndexType> >& stitch) {
	ofMesh out = mesh;
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
	return dropUnusedVertices(out);
}

// is c left of the line a-b
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
    
	ofPolyline polyline;
	for(int i = 0; i < indices.size(); i++) {
		polyline.addVertex(mesh.getVertex(indices[i]));
	}
    
    // duplicate vertices along the split
    vector<ofIndexType> newIndices;
	for(int i = 0; i < indices.size(); i++) {
		newIndices.push_back(mesh.getNumVertices());
		int index = indices[i];
        addAttributes(mesh, index, mesh);
	}
    
    ofVec2f front = mesh.getVertex(indices.front());
    ofVec2f back = mesh.getVertex(indices.back());
    
    // with all the triangles
    bool referenceSet = false;
    bool reference;
	for(int i = 0; i < n; i += 3) {
		ofIndexType i0 = mesh.getIndex(i + 0);
		ofIndexType i1 = mesh.getIndex(i + 1);
		ofIndexType i2 = mesh.getIndex(i + 2);
		
        // if one of the indices matches a split index
		for(int j = 0; j < indices.size(); j++) {
			if(i0 == indices[j] || i1 == indices[j] || i2 == indices[j]) {
                
                // and we made a boolean decision that's only true for one side
				ofVec2f vi0 = mesh.getVertex(i0);
				ofVec2f vi1 = mesh.getVertex(i1);
				ofVec2f vi2 = mesh.getVertex(i2);
                ofVec2f avg = (vi0 + vi1 + vi2) / 3;
                // sideTest is slower than isLeft but works for nonlinear splits
//                bool reassign = sideTest(polyline, avg);
                bool reassign = isLeft(front, back, avg);
                
                // this flips the reassignments based on the side of the first
                // this preserves topology based on indices instead of
                // only orientation
                if(!referenceSet) {
                    reference = reassign;
                    referenceSet = true;
                }
                if(reference) {
                    reassign = !reassign;
                }
            
				if(reassign) {
                    // swap all matching indices to other side
					for(int k = 0; k < 3; k++) {
                        ofIndexType& index = mesh.getIndices()[i + k];
						if(index == indices[j]) {
                            index = newIndices[j];
						}
					}
				}
			}
		}
	}
}
