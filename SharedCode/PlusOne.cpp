#include "PlusOne.h"

void PlusOne::update(const ofMesh& mesh) {
    ofMesh handMesh = mesh;
    
    // copy of the ring finger
    int toCopy[] = {
        115, 119, 124, 129, 134, 140, 63, 66, 69, 72, 75, 78, 81, 82, 83, 80, 77, 74, 71, 68, 65, 142, 135, 130, 125, 120};
    extraRegion = buildPolyline(handMesh, toCopy, 26);
    extraRegion.close();
    extraMesh = copySubmesh(handMesh, extraRegion);

    // place the extra finger in approximately the right place
    int extraRootIndex = 21, extraKnuckleIndex = 31;
    int splitRootIndex = 115, splitCrotchIndex = 140;
    orientMesh(extraMesh,
               extraMesh.getVertex(extraRootIndex),
               extraMesh.getVertex(extraKnuckleIndex),
               handMesh.getVertex(splitRootIndex),
               handMesh.getVertex(splitCrotchIndex));

    int leftCount = 6, rightCount = 6;

    // find the sides of the copied mesh
    int extraLeftIndices[] = {21, 22, 24, 26, 28, 30};
    extraLeftPath = buildPolyline(extraMesh, extraLeftIndices, leftCount);
    int extraRightIndices[] = {21, 23, 25, 27, 29, 32};
    extraRightPath = buildPolyline(extraMesh, extraRightIndices, rightCount);

    // split path
    int toSplit[] = {119, 124, 129, 134, 140};
    int toSplitCount = 5;
    splitPath = ofPolyline();
    vector<ofIndexType> indices;
    for(int i = 0; i < toSplitCount; i++) {
        splitPath.addVertex(handMesh.getVertex(toSplit[i]));
        indices.push_back(toSplit[i]);
    }
    split(handMesh, indices);

    // find the sides of the split mesh
    int splitLeftIndices[] = {115, 119, 124, 129, 134, 140};
    int splitRightIndices[] = {115, 151, 152, 153, 154, 155};

    splitLeftPath = buildPolyline(handMesh, splitLeftIndices, leftCount);
    splitRightPath = buildPolyline(handMesh, splitRightIndices, rightCount);

    orientPolyline(splitLeftPath,
                   *splitLeftPath.begin(),
                   *splitLeftPath.rbegin(),
                   *extraLeftPath.begin(),
                   *extraLeftPath.rbegin());

    orientPolyline(splitRightPath,
                   *splitRightPath.begin(),
                   *splitRightPath.rbegin(),
                   *extraRightPath.begin(),
                   *extraRightPath.rbegin());

    handPuppet = ofxPuppet();
    handPuppet.setup(handMesh);

    // put control points along hand's left seam
    for(int i = 0; i < leftCount; i++) {
        int splitIndex = splitLeftIndices[i];
        ofVec2f splitVertex = splitLeftPath[i];
        handPuppet.setControlPoint(splitIndex, splitVertex);
    }

    // put control points along hand's right seam
    for(int i = 0; i < rightCount; i++) {
        int splitIndex = splitRightIndices[i];
        ofVec2f splitVertex = splitRightPath[i];
        handPuppet.setControlPoint(splitIndex, splitVertex);
    }

    // bends the hand into shape
    handPuppet.update();
    handMesh = handPuppet.getDeformedMesh();

    fingerPuppet.setup(extraMesh);

    // put control points along finger's left seam
    for(int i = 0; i < leftCount; i++) {
        int extraIndex = extraLeftIndices[i];
        ofVec2f splitVertex = splitLeftPath[i];
        fingerPuppet.setControlPoint(extraIndex, splitVertex);
    }

    // put control points along finger's right seam
    for(int i = 0; i < rightCount; i++) {
        int extraIndex = extraRightIndices[i];
        ofVec2f splitVertex = splitRightPath[i];
        fingerPuppet.setControlPoint(extraIndex, splitVertex);
    }

    // make extra fingernail bisect adjacent fingers
    ofVec2f ringFingernail = handMesh.getVertex(58);
    ofVec2f middleFingernail = handMesh.getVertex(79);
    ofVec2f bisector = (middleFingernail + ringFingernail) / 2;
    ofVec2f extraKnuckle = extraMesh.getVertex(31);
    ofVec2f extraDirection = (bisector - extraKnuckle).normalize();
    ofVec2f middleKnuckle = handMesh.getVertex(141);
    float extraLength = middleKnuckle.distance(middleFingernail);
    fingerPuppet.setControlPoint(16, extraKnuckle + extraDirection * extraLength);

    // bend the finger into shape
    fingerPuppet.update();
    extraMesh = fingerPuppet.getDeformedMesh();

    // extract a mesh from the base of the ring finger
    int baseRegionCount = 14;
    int leftBaseRegionIndices[] = {
        115, 118, 123, 128, 133, 138, 42, 43, 44, 140, 134, 129, 124, 119};
    ofPolyline leftBaseRegion = buildPolyline(handMesh, leftBaseRegionIndices, baseRegionCount);
    leftBaseMesh = copySubmesh(handMesh, leftBaseRegion);

    // extract a mesh from the base of the middle finger
    int rightBaseRegionIndices[] = {
        115, 151, 152, 153, 154, 155, 63, 64, 65, 142, 135, 130, 125, 120};
    ofPolyline rightBaseRegion = buildPolyline(handMesh, rightBaseRegionIndices, baseRegionCount);
    rightBaseMesh = copySubmesh(handMesh, rightBaseRegion);

    // set the colors of the left base to fade from right to left
    // and the right base to fade from left to right
    // the indices start at the base, go around clockwise (for a left hand)
    // and the last vertex is the kunckle
    int leftBaseIndices[] = {3, 4, 6, 8, 10, 12, 0, 1, 2, 14, 11, 9, 7, 5, 13};
    int rightBaseIndices[] = {3, 10, 11, 12, 13, 14, 0, 1, 2, 9, 7, 6, 5, 4, 8};
    float leftBaseOpacity[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0};
    float rightBaseOpacity[] = {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int baseCount = 15;
    vector<ofFloatColor> leftBaseColors(baseCount), rightBaseColors(baseCount);
    for(int i = 0; i < baseCount; i++) {
        leftBaseColors[leftBaseIndices[i]] = ofFloatColor(1, leftBaseOpacity[i]);
        rightBaseColors[rightBaseIndices[i]] = ofFloatColor(1, rightBaseOpacity[i]);
    }
    leftBaseMesh.addColors(leftBaseColors);
    rightBaseMesh.addColors(rightBaseColors);
    
    // make sure the vertices of the index base match the position of the extra finger
    int extraMeshBaseIndices[] = {21, 22, 24, 26, 28, 30, 0, 1, 2, 32, 29, 27, 25, 23, 31};
    for(int i = 0; i < baseCount; i++) {
        int fromIndex = extraMeshBaseIndices[i];
        ofVec3f& fromVertex = extraMesh.getVertices()[fromIndex];
        leftBaseMesh.getVertices()[leftBaseIndices[i]] = fromVertex;
        rightBaseMesh.getVertices()[rightBaseIndices[i]] = fromVertex;
    }
    
    final = handPuppet.getDeformedMesh();
    final.append(extraMesh);
    
    // this step is required for subdivision
    // but it kills the texture mapping
    mergeCoincidentVertices(final);
}

ofMesh& PlusOne::getModifiedMesh() {
    return final;
}

void PlusOne::drawBlends() {
    // this can be used to draw over poor texture mapping
    extraMesh.drawFaces();
    
    // these are necessary for proper blending at the base
    leftBaseMesh.drawFaces();
    rightBaseMesh.drawFaces();
}