/*
 *  ofxSceneUser.cpp
 *  emptyExample
 *
 *  Created by pach on 31/01/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxSceneUser.h"

ofxSceneUser::ofxSceneUser (int userId, int userNi, int w, int h){
	idUser = userId ; 
    idUserNi = userNi ;
	isCalculating = true ; 
	width = w ; 
	height = h ; 
	mask.allocate(w, h, GL_RGBA);
	maskTexture = (unsigned char *)malloc(sizeof(unsigned char *)*width*height*4);
    
    isNew = 3 ;
	
   syphonText.setName("user"+ofToString(idUser));
    
	printf("new() user %i, wh : %i, %i\n", idUser, width, height);
}

/*
ofxSceneUser::ofxSceneUser (ofxSceneUser * us){
	idUser = us->idUser ; 
    idUserNi = us->idUserNi ;
	isCalculating = true ; 
	width = us->width ; 
	height = us->height ; 
	mask.allocate(width, height, GL_RGBA);
	maskTexture = (unsigned char *)malloc(sizeof(unsigned char *)*width*height*4);
    
    centerX = us->centerX;
    centerY = us->centerY;
    centerZ = us->centerZ;
    
    cout<<"new() user by copy "<<us->centerX<<", "<<us->centerY<<", "<<us->centerZ<<endl;
    
    isNew = false ;
    
    //   syphonText.setName(ofToString(idUser));
    printf("new() user %i, wh : %i, %i\n", idUser, width, height);
}
*/

ofxSceneUser::~ofxSceneUser (){
    if (maskTexture != NULL) free(maskTexture);
    printf("delete() user %i\n", idUser);
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
    
    syphonText.publishTexture(&mask);
	
}

void ofxSceneUser::drawUser(int x, int y, int w, int h){
	ofSetColor (0, 255, 0) ; 
	ofNoFill();
	ofRect (ofMap(xMin, 0, width, x, x+w), ofMap(yMin, 0, height, y, y+h), ofMap(xMax-xMin, 0, width, x, x+w), ofMap(yMax-yMin, 0, height, y, y+h));
    
	ofSetColor (255, 255, 255) ;
	mask.draw(x, y, w, h);
    
	ofSetColor (255, 0, 0) ; 
	ofFill();
	ofCircle(ofMap(centerX, 0, width, x, x+w), ofMap(centerY, 0, height, y, y+h), 20);
    ofSetColor (0, 255, 255) ; 
    ofDrawBitmapString(ofToString(idUser), ofMap(centerX-2, 0, width, x, x+w), ofMap(centerY-2, 0, height, y, y+h));
	
}


void ofxSceneUser::addPoint (int x, int y, int z){
	nbPoints ++ ; 
	tempX += x ; 
	tempY += y ; 
	tempZ += z ; 
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
	xMin = width ;
	yMin = height; 
	zMin = 65535 ; 
	xMax = yMax = zMax = 0 ; 
    tempX = tempY = tempZ = 0 ;
	isCalculating = true ; 
}

bool ofxSceneUser::finalyze () {
	mask.loadData(maskTexture, width, height, GL_RGBA);
    if (nbPoints > 0) {
        centerX = (int)(tempX/(float)nbPoints) ; 
        centerY = (int)(tempY/(float)nbPoints) ;
        centerZ = (int)(tempZ/(float)nbPoints) ; 

       // printf("finalize user %i, %i : %i, %i, %i, nbPoints %i\n", idUser, idUserNi, centerX, centerY, centerZ, nbPoints);

    }
	isCalculating = false ; 
//	printf("user %i finalyzed with %i points, center : %i, %i, %i, borne %i, %i : %i, %i : %i, %i\n", idUser, nbPoints, centerX, centerY, centerZ, xMin, xMax, yMin, yMax, zMin, zMax);
	
//    syphonText.publishTexture(&mask);
    
	if (nbPoints == 0)
		return false ; 
	else
		return true ; 
}
