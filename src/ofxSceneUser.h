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
	ofxSceneUser (int userId, int w, int h);
	
	void setup() ;
	void drawUser() ; 
/*	
	ofVec3f getCenter () ; 
	ofRectangle getBoundingBox () ; 
	ofTexture getMask () ; 
*/	
	void addPoint (int x, int y, int z);
	void reset () ; 
	bool finalyze () ; 
	
	int idUser ;
	int xMin, yMin, zMin, xMax, yMax, zMax, centerX, centerY, centerZ, centerWorldX, centerWorldY, centerWorldZ ; 
	ofTexture mask ;
	int nbPoints ; 
	int width, height ;
	
private:
	bool isCalculating ;
	unsigned char * maskTexture ; 
    
//    ofxSyphonServer syphonText ;
    
};
