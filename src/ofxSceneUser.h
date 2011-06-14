/*
 *  ofxSceneUser.h
 *  emptyExample
 *
 *  Created by pach on 31/01/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once 

#include "ofMain.h"
// #include "ofxSyphonServer.h"

class ofxSceneUser {

public:
	ofxSceneUser (int userId, int idUserNi, int w, int h);
    ~ofxSceneUser ();
    
	void setup() ;
	void drawUser() ; 
    void drawUser(int x, int y, int w, int h) ; 
/*	
	ofVec3f getCenter () ; 
	ofRectangle getBoundingBox () ; 
	ofTexture getMask () ; 
*/	
	void addPoint (int x, int y, int z);
	void reset () ; 
	bool finalyze () ; 
    
	int idUserNi ;
    int idUser ;
	int xMin, yMin, zMin, xMax, yMax, zMax, centerX, centerY, centerZ, centerWorldX, centerWorldY, centerWorldZ ; 
    int tempX, tempY, tempZ ;
    float normX,normY, normZ;
	ofTexture mask ;
	int nbPoints ; 
	int width, height ;
    int isNew ;
	
private:
	bool isCalculating ;
	unsigned char * maskTexture ; 
    
//    ofxSyphonServer syphonText ;
    
};
