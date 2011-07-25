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


ofxSceneAnalyzer::ofxSceneAnalyzer () {
    printf("new() ofxSceneAnalyzer\n");
}
ofxSceneAnalyzer::~ofxSceneAnalyzer () {

    std::map<int, ofxSceneUser *>::iterator it = tmpUsers.begin();
    std::map<int, ofxSceneUser *>::iterator itEnd = tmpUsers.end();
    while (it != itEnd) {
        delete(it->second);
        it++;
    }
    tmpUsers.clear();
    
    it=users.begin();
    itEnd=users.end();
    while (it != itEnd) {
        delete(it->second);
        it++;
    }
    users.clear();
    
    std::vector<standbyUser *>::iterator its = standbyUsers.begin();
    std::vector<standbyUser *>::iterator itsEnd = standbyUsers.end();
    while(its != itsEnd){
        delete((*its)->user);
        delete(*its);
        its++;
    }
    standbyUsers.clear();
    
    printf("delete() ofxSceneAnalyzer\n");
}



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
    
    minX = minY = -4000;
    minZ = 500;
    maxX = maxY = maxZ = 4000;
    
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

void ofxSceneAnalyzer::drawUsers(int x, int y, int w, int h){
	std::map<int, ofxSceneUser *>::iterator it = users.begin(); 
	std::map<int, ofxSceneUser *>::iterator itEnd = users.end(); 
	
	while (it != itEnd) {
		it->second->drawUser(x, y, w, h) ; 
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
	it = tmpUsers.begin(); 
	itEnd = tmpUsers.end(); 
	while (it != itEnd) {
		it->second->reset() ; 
		it ++;
	}
    
    itEnd = users.end(); 

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
				// get id for this pixel
				int userIdNi = label ; 
				
				itUser = users.find(userIdNi);
                
				// if user doesn't exist, create it
				if (itUser == itEnd){
                    itUser = tmpUsers.find(userIdNi);
                    if (itUser == tmpUsers.end()){
                        newTmpUser(userIdNi);
                        itUser = tmpUsers.find(userIdNi);
                    }
				}
							
				// add depth and pxl info to the user
				itUser->second->addPoint(j, i, *pDepth);
			}
			
			pLabels ++ ; 
			pDepth ++ ; 
		}
	}
	
    //finalyze current users
	it = users.begin() ; 
    itEnd = users.end() ;
	while (it != itEnd) {
		// finalyze users
		if(!it->second->finalyze()){
			// if return false delete user
			ofxSceneUser * user = it->second ; 
            it ++ ;
			lostUser(*user);
		}
		else {
            XnVector3D coordImg, coordWorld ;
            coordImg.X = it->second->centerX ;
            coordImg.Y = it->second->centerY ;
            coordImg.Z = it->second->centerZ ;
            depth_generator->getXnDepthGenerator().ConvertProjectiveToRealWorld(1, &coordImg, &coordWorld);
            it->second->centerWorldX = coordWorld.X;
            it->second->centerWorldY = coordWorld.Y;
            it->second->centerWorldZ = coordWorld.Z;

            it->second->normX = ofMap(coordWorld.X, minX, maxX, 0., 1.) ;
            it->second->normY = ofMap(coordWorld.Y, minY, maxY, 0., 1.) ;
            it->second->normZ = ofMap(coordWorld.Z, minZ, maxZ, 0., 1.) ;
            
            //cout<<"user finalisé, coord : "<<it->second->centerX<<", "<<it->second->centerY<<"' "<<it->second->centerZ<<" ; "<<it->second->centerWorldX<<", "<<it->second->centerWorldY<<", "<<it->second->centerWorldZ<<" ; "<<it->second->normX<<", "<<it->second->normY<<", "<<it->second->normZ<<endl;

            
			it++ ; 
        }
	}
    // finalyze new users
    newUsersFinalize();

	
	//if (find) printf (".\n");
//	printf(".................end analyze.................\n.............................................\n\n");
}

void ofxSceneAnalyzer::update(){
	sceneAnalyze() ; 
    updateStandby() ;
//    reorderUsers(); //completely buggy -> to debug before use
}

/* completely buggy */
void ofxSceneAnalyzer::reorderUsers(){
    if (ordering == 1){
        //par taille de users
        map<int, ofxSceneUser *>::iterator it = users.begin();
        map<int, ofxSceneUser *>::iterator itEnd = users.end();
        while (it != itEnd) {
            it->second->idOrder = 1;        
            it++;
        }
        
        while (it != itEnd) {
            map<int, ofxSceneUser *>::iterator itTri = users.begin();
            int itSize = it->second->nbPoints;
            while (itTri != itEnd) {
                if (itTri != it){
                    if (itTri->second->nbPoints < itSize)
                        itTri->second->idOrder ++ ;
                    else 
                        it->second->idOrder ++ ;
                }
                itTri ++ ;
            }
            it++;
        }
    }
    else if (ordering == 2){
        // par position
        map<int, ofxSceneUser *>::iterator it = users.begin();
        map<int, ofxSceneUser *>::iterator itEnd = users.end();
        while (it != itEnd) {
            it->second->idOrder = 1;        
            it++;
        }
        
        while (it != itEnd) {
            map<int, ofxSceneUser *>::iterator itTri = users.begin();
            int itPos = it->second->centerZ;
            while (itTri != itEnd) {
                if (itTri != it){
                    if (itTri->second->centerZ < itPos)
                        itTri->second->idOrder ++ ;
                    else 
                        it->second->idOrder ++ ;
                }
                itTri ++ ;
            }
            it++;
        }
    }
    else{
        // par ordre d'apparition (pas de reorder)
        map<int, ofxSceneUser *>::iterator it = users.begin();
        map<int, ofxSceneUser *>::iterator itEnd = users.end();
        while (it != itEnd) {
            it->second->idOrder = it->second->idUser ;        
            it++;
        }
    }
}

void ofxSceneAnalyzer::updateStandby(){
	// parcours les standby
    vector<standbyUser *>::iterator it = standbyUsers.begin();
    vector<standbyUser *>::iterator itEnd = standbyUsers.end();
    
    while (it != itEnd) {
        // met a jour le temps de standby
        (*it)->updateUser();
        // si temps sup a tempsMaxStandby, delete le user    
        if ((*it)->isOld()){
            deleteUser(*((*it)->user));
            ofxSceneUser* tmpUser = (*it)->user ;
            vector<standbyUser *>::iterator itTmp = it ;
            it++;
            if((*itTmp) != NULL)delete((*itTmp));
            standbyUsers.erase(itTmp);
            if(tmpUser != NULL) delete(tmpUser);
            
            printCorresp();
        }else{
            it ++;
        }
    }
    
}

void ofxSceneAnalyzer::newTmpUser(int idUser){
    cout<<"new tmp user"<<endl;
    tmpUsers.insert(std::pair<int, ofxSceneUser *>(idUser, new ofxSceneUser(idUser, idUser, width, height))) ;
}

void ofxSceneAnalyzer::newUsersFinalize() {
    
    //parcours les nouveaus users
    map<int,ofxSceneUser*>::iterator itU = tmpUsers.begin();
    map<int,ofxSceneUser*>::iterator itUEnd = tmpUsers.end();
    int idUserNi ;
    while (itU != itUEnd) {
        // finalize le user
        if(itU->second->isNew == 0 && itU->second->finalyze()){
            XnVector3D coordImg, coordWorld ;
            coordImg.X = itU->second->centerX ;
            coordImg.Y = itU->second->centerY ;
            coordImg.Z = itU->second->centerZ ;
            depth_generator->getXnDepthGenerator().ConvertProjectiveToRealWorld(1, &coordImg, &coordWorld);
            itU->second->centerWorldX = coordWorld.X;
            itU->second->centerWorldY = coordWorld.Y;
            itU->second->centerWorldZ = coordWorld.Z;
            
            itU->second->normX = ofMap(coordWorld.X, minX, maxX, 0., 1.) ;
            itU->second->normY = ofMap(coordWorld.Y, minY, maxY, 0., 1.) ;
            itU->second->normZ = ofMap(coordWorld.Z, minZ, maxZ, 0., 1.) ;
            
            //cout<<"new user finalisé, coord : "<<itU->second->centerX<<", "<<itU->second->centerY<<"' "<<itU->second->centerZ<<" ; "<<itU->second->centerWorldX<<", "<<itU->second->centerWorldY<<", "<<itU->second->centerWorldZ<<" ; "<<itU->second->normX<<", "<<itU->second->normY<<", "<<itU->second->normZ<<endl;
            
            idUserNi = itU->second->idUserNi ;

            // recherche dans standby si user porte num idUser et ou proche de la position de reapparition 
            vector<standbyUser *>::iterator it = standbyUsers.begin();
            vector<standbyUser *>::iterator itEnd = standbyUsers.end();
            bool found = false ;
            int idOld ; 
            while (it != itEnd && !found ) {
                idOld = (*it)->isUser(idUserNi, coordWorld.X, coordWorld.Y, coordWorld.Z);
                if (idOld != -1){
                    found = true;
                }else {
                    it++;
                }
            }
            if (found){      
                itU->second->idUser = idOld ;
                itU->second->idUserNi = idUserNi ;
                users.insert(std::pair<int, ofxSceneUser *>(idUserNi,  itU->second)) ;
                map<int,int>::iterator itID = correspID.find(idOld);
                if (itID != correspID.end()) {
                    itID->second = idUserNi;
                }else{
                    correspID.insert(pair<int, int>(idOld, idUserNi));
                }
                standbyUser * usTmp = (*it);
                standbyUsers.erase(it);
				if (usTmp != NULL) {
					if(usTmp->user != NULL) delete(usTmp->user);
					delete (usTmp);
				}
                ofNotifyEvent(retrieveUserEvent, idOld, this);
                cout<<"retrieve user "<<idOld<<" idNi "<<idUserNi<<endl;
            }else{
                // sinon cree nouvel user
                //verifie que id non utilisé
                int idUser = idUserNi;
                if(correspID.find(idUser)!=correspID.end()){
                    idUser = getFreeId();
                }
                //cree user
                itU->second->idUser = idUser ; 
                itU->second->idUserNi = idUserNi ; 
                users.insert(std::pair<int, ofxSceneUser *>(idUserNi, itU->second)) ;	
                correspID.insert(pair<int, int>(idUser, idUserNi));
                ofNotifyEvent(newUserEvent, idUser, this);
                
                cout<<"new user "<<idUser<<" idNi "<<idUserNi<<endl;
                
            }   
            printCorresp();
            
        }else{
            //cout<<"new user "<<itU->second->idUser<<" not finalize or too new"<<endl;
        }
        
        itU ++;
    }
    
    // passe isNew false sur nouveau user et delete old users 
    itU = tmpUsers.begin();
    while(itU != itUEnd){
        if (itU->second->isNew){
            itU->second->isNew --;
            itU ++;
        }else{
            map<int, ofxSceneUser*>::iterator itTmp = itU;
            itU++ ;
            tmpUsers.erase(itTmp);
        }
        
    }
    //tmpUsers.clear();
}

int ofxSceneAnalyzer::getFreeId(){
    bool found = false ;
    int id=1 ;
    while (!found) {
        // si l'id n'existe pas...
        if (correspID.find(id)==correspID.end()){
            found = true ;
        }else{
            id ++ ;
        }
    }
    return id ;
}

void ofxSceneAnalyzer::deleteUser(ofxSceneUser & user) {
    printf("delete user %i, %i \n", user.idUser, user.idUserNi);
	ofNotifyEvent(deleteUserEvent, user.idUser, this);
    correspID.erase(correspID.find(user.idUser));
}

void ofxSceneAnalyzer::lostUser(ofxSceneUser & user) {
    std::map<int, ofxSceneUser*>::iterator tmpUser = users.find(user.idUserNi) ;
    if (tmpUser != users.end()){
        standbyUsers.push_back(new standbyUser(tmpUser->second, STANDBY_TIME));
        printf("lost user %i, %i - ", user.idUser, user.idUserNi);
        users.erase(tmpUser);
    }else{
        printf("doesn't find user %i in users map\n", user.idUser);
    }
	ofNotifyEvent(lostUserEvent, user.idUser, this);
    map<int,int>::iterator itID = correspID.find(user.idUser);
    if (itID != correspID.end()){
        itID->second = -1 ;
    }
    printCorresp();
}

void ofxSceneAnalyzer::retrieveUser(ofxSceneUser & user) {
    printf("retrieve user %i, %i \n", user.idUser, user.idUserNi);
    printCorresp();
	ofNotifyEvent(retrieveUserEvent, user.idUser, this);
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

void ofxSceneAnalyzer::setNormalize(int minX, int minY, int minZ, int maxX, int maxY, int maxZ){
    this->minX = minX ;
    this->minY = minY ;
    this->minZ = minZ ;
    this->maxX = maxX ;
    this->maxY = maxY ;
    this->maxZ = maxZ ;
}
void ofxSceneAnalyzer::setNormalizeMinX(int minX){
    this->minX = minX ;
}
void ofxSceneAnalyzer::setNormalizeMinY(int minY){
    this->minY = minY ;
}
void ofxSceneAnalyzer::setNormalizeMinZ(int minZ){
    this->minZ = minZ ;
}
void ofxSceneAnalyzer::setNormalizeMaxX(int maxX){
    this->maxX = maxX ;
}
void ofxSceneAnalyzer::setNormalizeMaxY(int maxY){
    this->maxY = maxY ;
}
void ofxSceneAnalyzer::setNormalizeMaxZ(int maxZ){
    this->maxZ = maxZ ;
}
