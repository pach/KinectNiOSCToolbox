/*
 *  ofxSceneAnalyzer.cpp
 *  emptyExample
 *
 *  Created by pach on 20/01/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxSceneAnalyzer.h"
#include "ofxOpenNIMacros.h"


// Setup the user generator.
//----------------------------------------
bool ofxSceneAnalyzer::setup(ofxOpenNIContext* pContext, ofxDepthGenerator* pDepthGenerator) {
	/*if(!pContext->isInitialized()) {
		return false;
	}*/
	
	
	depth_generator = pDepthGenerator;
	context			= pContext;
	XnStatus result = XN_STATUS_OK;
	
	// check if the SCENE ANALYZER exists.
	result = context->getXnContext().FindExistingNode(XN_NODE_TYPE_SCENE, g_SceneAnalyzer);
	SHOW_RC(result, "Find scene analyzer");
	if(result != XN_STATUS_OK) {
		// create user generator.
		result = g_SceneAnalyzer.Create(context->getXnContext());
		SHOW_RC(result, "Create scene analyzer");
		if(result != XN_STATUS_OK) {
			return false;
		}
	}	
	
	result = g_SceneAnalyzer.StartGenerating();
	SHOW_RC(result, "StartGenerating");
}

void ofxSceneAnalyzer::draw(){

}

void ofxSceneAnalyzer::drawUsers(){
	std::map<int, ofxSceneUser *>::iterator it = users.begin(); 
	std::map<int, ofxSceneUser *>::iterator itEnd = users.end(); 
	
	while (it != itEnd) {
		it->second->drawUser() ; 
		it ++;
	}
	
}

void ofxSceneAnalyzer::sceneAnalyze(){
//	printf(".................begin analyze.................\n");
	//reset users
	std::map<int, ofxSceneUser *>::iterator it = users.begin(); 
	std::map<int, ofxSceneUser *>::iterator itEnd = users.end(); 
	
	while (it != itEnd) {
		it->second->reset() ; 
		it ++;
	}
	

	// get scene users map and depth map
//	xn::SceneMetaData smd ; 
	xn::DepthMetaData dmd ; 
	
//	g_SceneAnalyzer.GetMetaData(smd);
	depth_generator->getXnDepthGenerator().GetMetaData(dmd);	
	
//	const XnLabel* pLabels = smd.Data() ;
	const XnLabel* pLabels = g_SceneAnalyzer.GetLabelMap() ;
	const XnDepthPixel* pDepth = dmd.Data() ; 
	
	// get size
	XnUInt16 g_nXRes = dmd.XRes();
	XnUInt16 g_nYRes = dmd.YRes();
	
	/* a remonter dans setup */
	width = g_nXRes ; 
	height = g_nYRes ; 
	
//	bool find = false ; 
	std::map<int, ofxSceneUser *>::iterator itUser ; 
	
	// read openNI labelled image
	for (int i=0; i<g_nYRes; i++) {
		for (int j=0; j<g_nXRes; j++) {
			XnLabel label = *pLabels;
			
			//if (label != 0)
			//	find = true;
			
			if (label != 0){
				// get user for this pixel
				int userId = label ; 
				
				itUser = users.find(userId);
				
				// if user doesn't exist, create it
				if (itUser == itEnd){
					newUser(userId);
					itUser = users.find(userId);
				}
							
				// add depth and pxl info to the user
				itUser->second->addPoint(j, i, *pDepth);
			}
			
			pLabels ++ ; 
			pDepth ++ ; 
		}
	}
	
	// finalyze users si users toujours n'as pas de pixels -> le supprime
	it = users.begin() ; 
	while (it != itEnd) {
		// finalyze users
		if(!it->second->finalyze()){
			// if return false delete user
			ofxSceneUser * user = it->second ; 
			
			it ++ ;
			
			// a modif -> devrait mettre user dans file d'attente pour x seconde au cas ou reapparaisse
			//lostUser(user->idUser);
			deleteUser(user->idUser);
			delete(user) ; 
			
		}
		else {
            XnVector3D coordImg, coordWorld ;
            coordImg.X = it->second->centerX ;
            coordImg.Y = it->second->centerY ;
            coordImg.Z = it->second->centerZ ;
            depth_generator->getXnDepthGenerator().ConvertProjectiveToRealWorld(1, &coordImg, &coordWorld);
            it->second->centerWorldX = coordWorld.X ;
            it->second->centerWorldY = coordWorld.Y ;
            it->second->centerWorldZ = coordWorld.Z ;
			it++ ; 
        }
	}
	
	//if (find) printf (".\n");
//	printf(".................end analyze.................\n.............................................\n\n");
}

void ofxSceneAnalyzer::update(){
	sceneAnalyze() ; 
}

void ofxSceneAnalyzer::newUser(int idUser) {
	users.insert(std::pair<int, ofxSceneUser *>(idUser, new ofxSceneUser(idUser, width, height))) ;			 
	ofNotifyEvent(newUserEvent, idUser, this);
}

void ofxSceneAnalyzer::deleteUser(int idUser) {
	users.erase(idUser);
	ofNotifyEvent(deleteUserEvent, idUser, this);
}

void ofxSceneAnalyzer::lostUser(int idUser) {
	//users.erase(idUser);
	ofNotifyEvent(lostUserEvent, idUser, this);
}

void ofxSceneAnalyzer::retrieveUser(int idUser) {
	ofNotifyEvent(retrieveUserEvent, idUser, this);
}

/*
ofEvent <int> newUserEvent ; 
ofEvent <int> disappearUserEvent ; 
ofEvent <int> lostUserEvent ; 
ofEvent <int> retrieveUserEvent ; 
*/
std::map<int, ofxSceneUser *> ofxSceneAnalyzer::getUsers() {
	return users ; 
}
