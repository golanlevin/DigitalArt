#pragma once

#include "TopologyModifier.h"
#include "MeshUtils.h"
#include "ofxPuppet.h"

class PlusOne : public TopologyModifier {
protected:
    ofPolyline extraRegion;
    ofMesh extraMesh;
    
    ofxPuppet handPuppet;
    ofxPuppet fingerPuppet;
    
    ofMesh leftBaseMesh, rightBaseMesh;
    
    ofPolyline splitPath;
    ofPolyline extraLeftPath, extraRightPath;
    ofPolyline splitLeftPath, splitRightPath;
    
    ofMesh final;
    
public:
    void update(const ofMesh& mesh);
    ofMesh& getModifiedMesh();
    void drawBlends();
};