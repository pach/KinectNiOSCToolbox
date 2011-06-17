//
//  PointClouds.cpp
//  kinectNiOSCToolbox
//
//  Created by pach on 17/06/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "PointClouds.h"

PointClouds::PointClouds(){
    isInited = false;
}

void PointClouds::setup(int w, int h, ofxDepthGenerator * dg, ofxImageGenerator * ig){
    width = w ; 
    height = h ; 
    cloudColors = (ofColor *)malloc(sizeof(ofColor)*w*h);
    cloudPoints = (ofPoint *)malloc(sizeof(ofPoint)*w*h);
    depth = dg ;
    image = ig ;
    
    isInited = true ;
}

PointClouds::~PointClouds(){
    if (isInited){
        free(cloudColors);
        free(cloudPoints);
    }
}


void PointClouds::updateCloud(){
    
    if (isInited){
        int step = 1;
        int nIndex = 0;
        
        
        for (int nY = 0; nY < height; nY += step) {
            
            for (int nX = 0; nX < width; nX += step, nIndex += step) {
        
                cloudPoints[nIndex].x = nX;
                cloudPoints[nIndex].y = nY;
                cloudPoints[nIndex].z = depth->getPixelDepth(nX, nY);
                ofColor color = image->getPixelAt(nX, nY);
                cloudColors[nIndex].r = color.r;
                cloudColors[nIndex].g = color.g;
                cloudColors[nIndex].b = color.b;
                cloudColors[nIndex].a = 255;
                
            }
        }
    }
}

void PointClouds::updateCloudFromMask(ofTexture & mask){
    
}

void PointClouds::drawCloud(){
    if(isInited){
        glTranslatef(width, height/2, -500);
        
        
        glBegin(GL_POINTS);
        
        int step = 1;
        
        for(int y = 0 ; y < height ; y += step) {
            for(int x = 0 ; x < width ; x += step) {
                            
                ofPoint pos = cloudPoints[y*width+x];
                ofColor color = cloudColors[y*width+x];
                glColor4ub((unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b, (unsigned char)color.a);
                glVertex3f(pos.x, pos.y, pos.z);
            }
        }
        
        glEnd();
    }
}