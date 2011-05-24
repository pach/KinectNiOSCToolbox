/*
 *  ofxSceneUser.cpp
 *  emptyExample
 *
 *  Created by pach on 31/01/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxSceneUser.h"

ofxSceneUser::ofxSceneUser (int userId, int w, int h){
	idUser = userId ; 
	isCalculating = true ; 
	width = w ; 
	height = h ; 
	mask.allocate(w, h, GL_RGBA);
	maskTexture = (unsigned char *)malloc(sizeof(unsigned char *)*width*height*4);
	
//   syphonText.setName(ofToString(idUser));
    
//	printf("new user %i, wh : %i, %i\n", idUser, width, height);
}

void ofxSceneUser::drawUser(){
	ofSetColor (0, 255, 0) ; 
	ofNoFill();
	ofRect (xMin, yMin, xMax-xMin, yMax-yMin);

	ofSetColor (255, 255, 255) ;
	mask.draw(0, 0);

	ofSetColor (255, 0, 0) ; 
	ofFill();
	ofCircle(centerX, centerY, 20);
    ofSetColor (0, 255, 255) ; 
    ofDrawBitmapString(ofToString(idUser), centerX-2, centerY-2);
	
}


void ofxSceneUser::addPoint (int x, int y, int z){
	nbPoints ++ ; 
	centerX += x ; 
	centerY += y ; 
	centerZ += z ; 
	if (xMin > x) xMin = x ; 
	if (yMin > y) yMin = y ; 
	if (zMin > z) zMin = z ; 
	if (xMax < x) xMax = x ; 
	if (yMax < y) yMax = y ; 
	if (zMax < z) zMax = z ; 
	
	maskTexture[width*y*4+x*4] = 255 ; 
	maskTexture[width*y*4+x*4+1] = 255 ; 
	maskTexture[width*y*4+x*4+2] = 255 ; 
	maskTexture[width*y*4+x*4+3] = 150 ; 
}

void ofxSceneUser::reset () { 
	for (int i=0 ; i<width*height*4 ; i++){
		maskTexture[i] = 0 ; 
	}
	
	nbPoints = 0 ; 
	xMin = 640 ;
	yMin = 480 ; 
	zMin = 65535 ; 
	xMax = yMax = zMax = 0 ; 
	centerX = centerY = centerZ = 0 ; 
	isCalculating = true ; 
}

bool ofxSceneUser::finalyze () {
	mask.loadData(maskTexture, width, height, GL_RGBA);
	centerX = (int)(centerX/(float)nbPoints) ; 
	centerY = (int)(centerY/(float)nbPoints) ;
	centerZ = (int)(centerZ/(float)nbPoints) ; 
	isCalculating = false ; 
//	printf("user %i finalyzed with %i points, center : %i, %i, %i, borne %i, %i : %i, %i : %i, %i\n", idUser, nbPoints, centerX, centerY, centerZ, xMin, xMax, yMin, yMax, zMin, zMax);
	
//    syphonText.publishTexture(&mask);
    
	if (nbPoints == 0)
		return false ; 
	else
		return true ; 
}
