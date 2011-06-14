/*
 *  OSCUserSender.cpp
 *  emptyExample
 *
 *  Created by pach on 04/03/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "OSCUserSender.h"


void OSCUserSender::connect (std::string host, int port) {
	sender.setup(host, port);
}

void OSCUserSender::sendNewUser (int idUser) {
	ofxOscMessage m;
	m.setAddress("/user/new");
	m.addIntArg(idUser);
	sender.sendMessage(m);
    
    m.clear();
    
	printf("osc send new user %i\n", idUser);
}

void OSCUserSender::sendUserSeparate(ofxSceneUser & user) {
	ofxOscMessage m; 
	std::string addr = "/user/data/" ;
    addr += ofToString(user.idUser);
    addr += "/x";
	m.setAddress(addr);
	m.addFloatArg(user.normX);
    sender.sendMessage(m);
    
    m.clear();
    
    addr = "/user/data/" ;
    addr += ofToString(user.idUser);
    addr += "/y";
	m.setAddress(addr);
	m.addFloatArg(user.normY);
    sender.sendMessage(m);

    m.clear();
    
    addr = "/user/data/" ;
    addr += ofToString(user.idUser);
    addr += "/z";
	m.setAddress(addr);
	m.addFloatArg(user.normZ);
    sender.sendMessage(m);	
    
    m.clear();
	
//	printf("send user %i : %i, %i, %i -> %i, %i, %i\n", idUser, user.centerWorldX, user.centerWorldY, user.centerWorldZ, user.centerX, user.centerY, user.centerZ);
    
}

void OSCUserSender::sendUser (ofxSceneUser & user) {
	ofxOscMessage m; 
	std::string addr = "/user/data/" ;
    addr += ofToString(user.idUser);
	m.setAddress(addr);
    //m.addIntArg(idUser);
	m.addIntArg(user.centerWorldX);
	m.addIntArg(user.centerWorldY);
	m.addIntArg(user.centerWorldZ);
	
	sender.sendMessage(m);

    m.clear();
    
//	printf("send user %i : %i, %i, %i -> %i, %i, %i\n", idUser, user.centerWorldX, user.centerWorldY, user.centerWorldZ, user.centerX, user.centerY, user.centerZ);

}

void OSCUserSender::sendNormalizeUser (ofxSceneUser & user) {
	ofxOscMessage m; 
	std::string addr = "/user/data/" ;
    addr += ofToString(user.idUser);
	m.setAddress(addr);
    //m.addIntArg(idUser);
	m.addFloatArg(user.normX);
	m.addFloatArg(user.normY);
	m.addFloatArg(user.normZ);
	
	sender.sendMessage(m);
	
    m.clear();
    
	//printf("send user %i : %f, %f, %f\n", idUser, user.normX, user.normY, user.normZ);
    
}


void OSCUserSender::sendFullUser (ofxSceneUser & user) {
	ofxOscMessage m; 
	std::string addr = "/user/fulldata" ; 
    addr += ofToString(user.idUser);
	m.setAddress(addr);
    //m.addIntArg(idUser);
	m.addFloatArg(user.centerX/(float)user.width);
	m.addFloatArg(user.centerY/(float)user.height);
	m.addFloatArg(user.centerZ/5000.);
	m.addFloatArg(user.xMin/(float)user.width);
	m.addFloatArg(user.xMax/(float)user.width);
	m.addFloatArg(user.yMin/(float)user.height);
	m.addFloatArg(user.yMax/(float)user.height);
	m.addFloatArg(user.zMin/5000.);
	m.addFloatArg(user.zMax/5000.);
	
	sender.sendMessage(m);
    
    m.clear();
	
//	printf("send full user %i : %i, %i, %i\n", idUser, user.centerX, user.centerY, user.centerZ);
}

void OSCUserSender::sendDeleteUser (int idUser) {
	ofxOscMessage m;
	m.setAddress("/user/delete");
	m.addIntArg(idUser);
	sender.sendMessage(m);
    
    m.clear();
	
	printf("osc send delete user %i\n", idUser);
}

void OSCUserSender::sendLostUser (int idUser) {
	ofxOscMessage m;
	m.setAddress("/user/lost");
	m.addIntArg(idUser);
	sender.sendMessage(m);
    
    m.clear();
	
	printf("osc send lost user %i\n", idUser);
}

void OSCUserSender::sendRetrieveUser (int idUser) {
	ofxOscMessage m;
	m.setAddress("/user/retrieve");
	m.addIntArg(idUser);
	sender.sendMessage(m);
	
    m.clear();
    
	printf("osc send retrieve user %i\n", idUser);
}

void OSCUserSender::sendMsg(string msg, float data){
    ofxOscMessage m;
	m.setAddress(msg);
	m.addFloatArg(data);
	sender.sendMessage(m);
    
    m.clear();
}

void OSCUserSender::sendMsg(string msg, int data){
    ofxOscMessage m;
	m.setAddress(msg);
	m.addIntArg(data);
	sender.sendMessage(m);
    
    m.clear();
}

void OSCUserSender::sendHand (int idHand, ofPoint pos) {
	ofxOscMessage m;
    string addr = "/hand/" + ofToString(idHand) + "";
    
	m.setAddress(addr);
	m.addIntArg(idHand);
    m.addFloatArg(pos.x);
    m.addFloatArg(pos.y);
    m.addFloatArg(pos.z);
	sender.sendMessage(m);
    
}

void OSCUserSender::sendHandSeparate (int idHand, ofPoint pos) {
	ofxOscMessage m;
    string addr = "/hand/" + ofToString(idHand) + "/x";
    m.setAddress(addr);
	m.addFloatArg(pos.x);
    sender.sendMessage(m);
    m.clear();
    
    addr = "/hand/" + ofToString(idHand) + "/y";
    m.setAddress(addr);
	m.addFloatArg(pos.y);
    sender.sendMessage(m);
    m.clear();
    
    addr = "/hand/" + ofToString(idHand) + "/z";
    m.setAddress(addr);
	m.addFloatArg(pos.x);
    sender.sendMessage(m);
    m.clear();
}


