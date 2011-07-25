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


#define STANDBY_TIME 3000

class standbyUser {
public:  
	ofxSceneUser* user ; 
	int standbyTime ;
    int lifeTime;
    int birthTime;
    int oldTime ;
    
    int gapX, gapY, gapZ ;
  
    standbyUser(ofxSceneUser* us, int maxTime) {
        user = us ;
        standbyTime = 0 ;
        lifeTime = maxTime;
        birthTime = ofGetElapsedTimeMillis();
        
        gapX = gapY = gapZ = 1000;
        
        cout<<"new() standby "<<user->idUser<<" pos : "<<user->centerX<<", "<<user->centerY<<", "<<user->centerZ<<endl;
    }
    
    ~standbyUser(){
        cout<<"delete() standbyUser "<<user->idUser<<endl;
    }
    
    int isUser(int idNewU, int posX, int posY, int posZ){
        if (idNewU == user->idUserNi){
            printf("retrieve user by same ni id\n");
            return user->idUser;
        }else{
            if (user->centerWorldX > posX-gapX && user->centerWorldX < posX+gapX &&
                user->centerWorldY > posY-gapY && user->centerWorldY < posY+gapY &&
                user->centerWorldZ > posZ-gapZ && user->centerWorldZ < posZ+gapZ){
                printf("retrieve user by pos %i, %i, %i\n",posX, posY, posZ);
                return(user->idUser) ;
            }else {
                printf("user not retrieve pos old %i, %i, %i - new %i, %i, %i\n", user->centerX, user->centerY, user->centerZ, posX, posY, posZ); 
                return -1;
            }
        }
            
    }
    
    bool updateUser () {
        //printf("update stby user %i, %i\n",user->idUser, user->idUserNi);
        standbyTime = ofGetElapsedTimeMillis() - birthTime;
    }
    
    void setLifeTime(int maxTime) {
        lifeTime = maxTime ;
    }
    
    bool isOld() {
        if (standbyTime > lifeTime) 
            return true ;
        else
            return false ;
    }
    
    
};

class ofxSceneAnalyzer {
public:
    ofxSceneAnalyzer () ;
    ~ofxSceneAnalyzer () ;
    
	bool setup(ofxOpenNIContext* pContext, ofxDepthGenerator* pDepthGenerator) ; 
	
	void draw();
	void drawUsers() ;
    void drawUsers(int x, int y, int w, int h) ;
	void update();
    void updateStandby();
	
	/* not implemented */
	void newUsersFinalize() ;
    void newTmpUser(int idUser) ;
    
	void deleteUser(ofxSceneUser & user) ; 
	void lostUser(ofxSceneUser & user) ; 
	void retrieveUser(ofxSceneUser & user) ; 

	ofEvent <int> newUserEvent ; 
	ofEvent <int> deleteUserEvent ; 
	ofEvent <int> lostUserEvent ; 
	ofEvent <int> retrieveUserEvent ; 
	
	std::map<int, ofxSceneUser *> getUsers() ; 
    
    void setNormalize(int minX, int minY, int minZ, int maxX, int maxY, int maxZ);
    void setNormalizeMinX(int minX);
    void setNormalizeMinY(int minY);
    void setNormalizeMinZ(int minZ);
    void setNormalizeMaxX(int maxX);
    void setNormalizeMaxY(int maxY);
    void setNormalizeMaxZ(int maxZ);
	/* ----- */
	
    int getFreeId();
    
    void drawPointClouds();
    
    int ordering ; //0 = apparition, 1 = taille, 2 = position
    
    string printUsers(){
        stringstream str;
        map<int, ofxSceneUser *>::iterator it = users.begin();
        map<int, ofxSceneUser *>::iterator itEnd = users.end();
        while(it!=itEnd){
            str<<it->second->idUser<<" . "<<it->second->normX<<", "<<it->second->normY<<", :"<<it->second->normZ<<endl;
            it++;
        }
        return str.str();
    }
    
private:
	void sceneAnalyze () ; 
	void reorderUsers();
    
	int width, height ; 
    
    int minX, maxX, minY, maxY, minZ, maxZ;
	
	ofxOpenNIContext* context;
	ofxDepthGenerator* depth_generator;
	
	xn::SceneAnalyzer g_SceneAnalyzer;
	
    std::map<int, int> correspID ; //map de correspondance des id : idUser, idLabel (-1 si standby)
	std::map<int, ofxSceneUser *> users ; // identifié par leur id dans labelMap (pas forcement identique à id user
    std::map<int, ofxSceneUser *> tmpUsers ;
    std::map<int, ofxSceneUser *> newUsers ;
	std::vector<standbyUser *> standbyUsers ;
    
    
    
    void printCorresp(){
        cout<<"corresp id : ";
        map<int,int>::iterator it = correspID.begin();
        map<int,int>::iterator itEnd = correspID.end();
        while(it!=itEnd){
            cout<<it->first<<" "<<it->second<<" | ";
            it++;
        }
        cout<<"\n";
        
        printStby();
    }
    
    void printStby(){
        cout<<"stby : ";
        vector<standbyUser *>::iterator it = standbyUsers.begin();
        vector<standbyUser *>::iterator itEnd = standbyUsers.end();
        while(it!=itEnd){
            cout<<(*it)->user->idUser<<" "<<(*it)->user->idUserNi<<" | ";
            it++;
        }
        cout<<"\n";
    }
    
    
};