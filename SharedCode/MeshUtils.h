#pragma once

#include "ofMain.h"

void addAttributes(const ofMesh& src, int index, ofMesh& dst);
void orientMesh(ofMesh& mesh,
                ofVec2f fromStart, ofVec2f fromEnd,
                ofVec2f toStart, ofVec2f toEnd);
void orientPolyline(ofPolyline& polyline,
                    ofVec2f fromStart, ofVec2f fromEnd,
                    ofVec2f toStart, ofVec2f toEnd);
ofPolyline buildPolyline(ofMesh& mesh, int indices[], int count);
void removeTriangles(ofMesh& mesh, const ofPolyline& region);
ofMesh dropUnusedVertices(ofMesh& mesh);
ofMesh copySubmesh(const ofMesh& mesh, const ofPolyline& region);
void mergeCoincidentVertices(ofMesh& mesh, float epsilon = 10e-5);
ofMesh stitch(ofMesh& mesh, vector<pair<ofIndexType, ofIndexType> >& stitch);
bool isLeft(ofVec2f a, ofVec2f b, ofVec2f c);
ofVec2f closestPointOnLine(const ofVec2f& p1, const ofVec2f& p2, const ofVec2f& p3);
bool sideTest(ofPolyline& polyline, ofVec2f position);
void split(ofMesh& mesh, vector<ofIndexType>& indices);