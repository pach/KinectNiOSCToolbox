/*
 *  OSCUserSender.h
 *  emptyExample
 *
 *  Created by pach on 04/03/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once

#include "ofxOsc.h"
#include "ofxSceneUser.h"

class OSCUserSender {

public:
	
	void connect (std::string host, int port);
	
	void sendNewUser (int idUser) ; 
	void sendUser (int idUser, ofxSceneUser & user);
   	void sendUserSeparate (int idUser, ofxSceneUser & user);
	void sendFullUser (int idUser, ofxSceneUser & user);
	void sendDeleteUser (int idUser) ; // lost user, wait to see if retrieving or not
	void sendLostUser (int idUser) ;
	void sendRetrieveUser (int idUser) ; 
    
    void sendHand(int idHand, ofPoint pos);
    void sendHandSeparate(int idHand, ofPoint pos);
private:
	std::string host ; 
	int port ; 
	
	ofxOscSender sender ; 
};
