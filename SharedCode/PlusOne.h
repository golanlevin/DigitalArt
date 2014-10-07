#pragma once

#include "TopologyModifier.h"
#include "MeshUtils.h"
#include "ofxPuppet.h"

class PlusOne : public TopologyModifier {
protected:
    ofPolyline extraRegion;
    ofxPuppet handPuppet;
    ofxPuppet fingerPuppet;
    
    ofMesh leftBaseMesh, rightBaseMesh;
    
    ofPolyline splitPath;
    ofPolyline extraLeftPath, extraRightPath;
    ofPolyline splitLeftPath, splitRightPath;
    
    ofMesh extraMesh;
    
public:
    void update(const ofMesh& mesh);
    ofMesh& getModifiedMesh();
    void drawBlends();
};