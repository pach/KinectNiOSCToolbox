/*
 *  ofxSceneAnalyzer.h
 *  emptyExample
 *
 *  Created by pach on 20/01/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"

#include <vector>

#include "ofEvents.h"

#include "ofxSceneUser.h"

struct standbyUser {
	ofxSceneUser* user ; 
	int idUser ;
	int standbyTime ;
};

class ofxSceneAnalyzer {
public:
	bool setup(ofxOpenNIContext* pContext, ofxDepthGenerator* pDepthGenerator) ; 
	
	void draw();
	void drawUsers() ; 
	void update();
	
	/* not implemented */
	void newUser(int idUser) ; 
	void deleteUser(int idUser) ; 
	void lostUser(int idUser) ; 
	void retrieveUser(int idUser) ; 

	ofEvent <int> newUserEvent ; 
	ofEvent <int> deleteUserEvent ; 
	ofEvent <int> lostUserEvent ; 
	ofEvent <int> retrieveUserEvent ; 
	
	std::map<int, ofxSceneUser *> getUsers() ; 
	/* ----- */
	
private:
	void sceneAnalyze () ; 
	
	int width, height ; 
	
	ofxOpenNIContext* context;
	ofxDepthGenerator* depth_generator;
	
	xn::SceneAnalyzer g_SceneAnalyzer;
	
	std::map<int, ofxSceneUser *> users ; 
	std::vector<standbyUser *> standbyUsers ; 
};