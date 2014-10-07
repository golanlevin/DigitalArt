#include "MinusOne.h"

#include "ofxPuppet.h"

MinusOne::MinusOne() {
    // post-removal indices, not original indices
    int toStitchLeft[] = {99, 104, 109, 114, 120};
    int toStitchRight[] = {98, 103, 108, 113, 119};
    int toStitchCount = 5;
    stitchIndices.clear();
    for(int i = 0; i < toStitchCount; i++) {
        stitchIndices.push_back(pair<ofIndexType, ofIndexType>(toStitchLeft[i], toStitchRight[i]));
    }
}

void MinusOne::update(const ofMesh& mesh) {
    ofMesh handMesh = mesh;
    
    // build removal region
    int toRemove[] = {115, 120, 125, 130, 135, 142, 65, 68, 71, 74, 77, 80, 83, 82, 81, 78, 75, 72, 69, 66, 63, 140, 134, 129, 124, 119
    };
    int toRemoveCount = 26;
    removalRegion = buildPolyline(handMesh, toRemove, toRemoveCount);
    removalRegion.close();
    
    // make a copy of the removal region, to be used for blending
    int baseRegionCount = 14;
    int baseRegionIndices[] = {
        115, 119, 124, 129, 134, 140, 63, 64, 65, 142, 135, 130, 125, 120
    };
    ofPolyline baseRegion = buildPolyline(handMesh, baseRegionIndices, baseRegionCount);
    blendMesh = copySubmesh(handMesh, baseRegion);
    
    // remove the triangles for the remaining indices
    removeTriangles(handMesh, removalRegion);
    handMesh = dropUnusedVertices(handMesh);
    
    // stitch sides together
    handMesh = stitch(handMesh, stitchIndices);
    
    float opacity[] = {
        0,0,0,0, 0,0,0,0,
        0,1,1, 1,1,1,
        0};
    int blendIndices[] = {
        3, 4, 6, 8, 10, 12, 0, 1, 2, 14, 11, 9, 7, 5, 13};
    int handIndices[] = {
        94, 97, 102, 107, 112, 117, 42, 43, 44, 119, 113, 108, 103, 98, 118};
    int baseCount = 15;
    vector<ofFloatColor> colors(baseCount);
    for(int i = 0; i < baseCount; i++) {
        ofVec3f& from = handMesh.getVertices()[handIndices[i]];
        ofVec3f& to = blendMesh.getVertices()[blendIndices[i]];
        to = from;
        colors[blendIndices[i]] = ofFloatColor(1, opacity[i]);
    }
    blendMesh.addColors(colors);
    
    final = handMesh;
    
    // this step is required for subdivision
    // but it kills the texture mapping
//    mergeCoincidentVertices(final);
}

ofMesh& MinusOne::getModifiedMesh() {
    return final;
}

void MinusOne::drawBlends() {
    blendMesh.drawFaces();
}