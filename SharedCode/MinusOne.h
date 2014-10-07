#pragma once

#include "TopologyModifier.h"
#include "MeshUtils.h"

class MinusOne : public TopologyModifier {
protected:
    
    ofPolyline removalRegion;
    vector<pair<ofIndexType, ofIndexType> > stitchIndices;
    
    ofMesh blendMesh;
    
    ofMesh final;
    
public:
    MinusOne();
    void update(const ofMesh& mesh);
    ofMesh& getModifiedMesh();
    void drawBlends();
};