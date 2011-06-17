//
//  PointClouds.h
//  kinectNiOSCToolbox
//
//  Created by pach on 17/06/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include "ofxDepthGenerator.h"
#include "ofxImageGenerator.h"

class PointClouds {
public:
    PointClouds();
    void setup(int w, int h, ofxDepthGenerator * dg, ofxImageGenerator * ig);
    ~PointClouds();
    void updateCloud();
    void updateCloudFromMask(ofTexture & mask);
    void drawCloud();
    
private:
    ofxDepthGenerator * depth;
    ofxImageGenerator * image;
    
    ofPoint * cloudPoints;
    ofColor * cloudColors;
    
    int width, height;
    
    bool isInited;
};