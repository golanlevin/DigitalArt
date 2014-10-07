#pragma once

#include "ofMesh.h"
#include "ofTexture.h"

class TopologyModifier {
public:
    virtual void update(const ofMesh& mesh) = 0;
    virtual ofMesh& getModifiedMesh() = 0;
    virtual void drawBlends() = 0;
    
    void draw(const ofTexture& texture) {
        texture.bind();
        getModifiedMesh().drawFaces();
        drawBlends();
        texture.unbind();
    }
};