#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	
	isLive			= true;
	isTracking		= false;
	isTrackingHands	= true;
	isRecording		= false;

	
	//setupRecording();
	
	ofBackground(0, 0, 0);
    
    recordTimestamp = false;
    isSendingOSC = false;
    kinectState = false;
    isSkeleton = false;
    replayPath = "";
    
    setupgui();
    
    cloudRotate = 0.;
    
    ofAddListener(liveScene.newUserEvent, this, &testApp::newUser);
    ofAddListener(playScene.newUserEvent, this, &testApp::newUser);
    ofAddListener(liveScene.deleteUserEvent, this, &testApp::deleteUser);
    ofAddListener(playScene.deleteUserEvent, this, &testApp::deleteUser);
    ofAddListener(liveScene.retrieveUserEvent, this, &testApp::retrieveUser);
    ofAddListener(playScene.retrieveUserEvent, this, &testApp::retrieveUser);
    ofAddListener(liveScene.lostUserEvent, this, &testApp::lostUser);
    ofAddListener(playScene.lostUserEvent, this, &testApp::lostUser);
    
    syphonImage.setName("kinectImage");
    syphonDepth.setName("kinectDepth");
}

void testApp::setupgui() {
    ofxControlPanel::setBackgroundColor(simpleColor(30, 30, 30, 150));
    ofxControlPanel::setTextColor(simpleColor(255, 255, 255, 255));
    
    gui.setup("kinect tools", 0, 0, 750, 500);
    gui.addPanel("kinect tools", 2, false);
    gui.addPanel("tracking", 1, false);
    gui.addPanel("skeleton", 1, false);
    
    gui.setWhichPanel(0);
    
    gui.setWhichColumn(0);
    gui.addSlider("motor pos", "MOTOR", 0, -31., 31., true);
    gui.addToggle("enable kinect", "KINECT_STATE", false);
    gui.addToggle("record", "RECORD_STATE", false);
    gui.addToggle("add timestamp to record files", "RECORD_TIME", false);
    recPath = gui.addTextInput("record path", "path", ofGetWidth()/3.-10);
    gui.addToggle("replay", "REPLAY_STATE", false);
    replayFile.listDir("record/");
    gui.addFileLister("replay path", &replayFile, 230., 50);
    
    gui.addToggle("activate tracking", "TRACKING_STATE", true);
    gui.addToggle("activate hand", "HAND_STATE", false);
    gui.addToggle("activate skeleton", "SKELETON_STATE", false);
    
    gui.setWhichColumn(1);
    gui.addToggle("enable OSC", "OSC_STATE", true);
    oscPath = gui.addTextInput("OSC addr", "OSC_ADDR", "localhost", ofGetWidth()/3.-10);
    oscPort = gui.addTextInput("OSC port", "OSC_PORT", "8888", ofGetWidth()/3.-10);
    gui.addToggle("update OSC", "OSC_UPDATE", false);
    vector<string> oscTrackMode ;
    oscTrackMode.push_back("woorld coord");
    oscTrackMode.push_back("normalize coord");
    oscTrackMode.push_back("full user");
    oscTrackMode.push_back("separate data (normalize coord)");
    gui.addMultiToggle("OSC tracking mode", "OSC_TRACKING_MODE", 0, oscTrackMode);
    
    gui.setWhichPanel(1);
    
    gui.setWhichColumn(0);    
    gui.addSlider("norm min x", "TRACK_MIN_X", -3000, -5000, 5000, true);
    gui.addSlider("norm max x", "TRACK_MAX_X", 3000, -5000, 5000, true);
    gui.addSlider("norm min y", "TRACK_MIN_Y", -3000, -5000, 5000, true);
    gui.addSlider("norm max y", "TRACK_MAX_Y", 3000, -5000, 5000, true);
    gui.addSlider("norm min z", "TRACK_MIN_Z", 0., 0, 10000, true);
    gui.addSlider("norm max z", "TRACK_MAX_Z", 5000, 0, 10000, true);
    
    gui.setWhichPanel(2);
    gui.setWhichColumn(0);    
    skeletonPath = gui.addTextInput("record path", "path", ofGetWidth()/3.-10);
    gui.addToggle("record skeleton data", "SKEL_RECORD", false);
    skeletonFile.listDir("skeleton/");
    gui.addFileLister("skeleton path", &skeletonFile, 450, 50);
    gui.addToggle("force reload skeleton data", "SKEL_FORCE_RELOAD", false);
    
    gui.loadSettings("controlPanelSettings.xml");
   
    gui.setupEvents();
	gui.enableEvents();
    
    ofAddListener(gui.guiEvent, this, &testApp::eventsgui);
    
    gui.reloadSettings();
}

void testApp::eventsgui(guiCallbackData & data){
    
    if (data.getDisplayName() == "replay path"){
        replayPath = data.getString(2);
        cout<<"replay path :"<<replayPath<<endl;
    }
    
    string event = data.getXmlName();
    if (event == "MOTOR" && kinectState){
        hardware.setTiltAngle(data.getInt(0));
    } 
    else if (event == "KINECT_STATE"){
        bool currentState = kinectState ;
        kinectState = data.getInt(0);
        if (kinectState && currentState != kinectState){
            setupRecording();
        }else if (!kinectState){
            liveContext.shutdown();
        }
    }
    else if (event == "RECORD_STATE"){
        isRecording = data.getInt(0);
            
        if (isRecording){
            string _root = "record/" + recPath->getValueText();
            
            string _timestamp = "";
            if (recordTimestamp){
                _timestamp += "." +
                    ofToString(ofGetDay()) + 
                    ofToString(ofGetMonth()) +
                    ofToString(ofGetYear()) +
                    ofToString(ofGetHours()) +
                    ofToString(ofGetMinutes()) +
                    ofToString(ofGetSeconds());
            }
            
            string _filename = (_root + _timestamp + ".oni");
            
            oniRecorder.startRecord(_filename);
        }
        else{
            oniRecorder.stopRecord();
            replayFile.listDir("record");
        }
    } 
    else if (event == "RECORD_TIME"){
       recordTimestamp = data.getInt(0);        
    } 
    else if (event == "REPLAY_STATE"){
        isLive = !(bool)data.getInt(0);
        if (!isLive){
            string _filename = "record/" + replayPath ;    
            setupPlayback(_filename);
        }
    } 
    else if (event == "TRACKING_STATE"){
        isTracking =  data.getInt(0);
    } 
    else if (event == "SKELETON_STATE"){
        isSkeleton =  data.getInt(0);
    }
    else if (event == "HAND_STATE"){
        isTrackingHands = data.getInt(0);
    } 
    else if (event == "OSC_STATE"){
        isSendingOSC = data.getInt(0); 
        if (isSendingOSC){
            std::cout<<"connecting osc : "<< oscPath->getValueText() << ", "<< oscPort->getValueText()<<std::endl;
            oscSend.connect(oscPath->getValueText(), ofToInt(oscPort->getValueText()));
        }
    } 
    else if (event == "OSC_UPDATE"){
        if (data.getInt(0) == 1){
            oscSend.connect(oscPath->getValueText(), ofToInt(oscPort->getValueText()));
            std::cout<<"update osc : "<< oscPath->getValueText() << ", "<< oscPort->getValueText()<<std::endl;
            gui.setValueB("OSC_UPDATE", false);
        }
    } 
    else if (event == "OSC_TRACKING_MODE"){
        oscTrackingMode = data.getInt(0);
    } 
    else if (event == "TRACK_MIN_X"){
        liveScene.setNormalizeMinX(data.getInt(0));
        playScene.setNormalizeMinX(data.getInt(0));
    }
    else if (event == "TRACK_MIN_Y"){
        liveScene.setNormalizeMinY(data.getInt(0));
        playScene.setNormalizeMinY(data.getInt(0));
    }
    else if (event == "TRACK_MIN_Z"){
        liveScene.setNormalizeMinZ(data.getInt(0));
        playScene.setNormalizeMinZ(data.getInt(0));
    }
    else if (event == "TRACK_MAX_X"){
        liveScene.setNormalizeMaxX(data.getInt(0));
        playScene.setNormalizeMaxX(data.getInt(0));
    }
    else if (event == "TRACK_MAX_Y"){
        liveScene.setNormalizeMaxY(data.getInt(0));
        playScene.setNormalizeMaxY(data.getInt(0));
    }
    else if (event == "TRACK_MAX_Z"){
        liveScene.setNormalizeMaxZ(data.getInt(0));
        playScene.setNormalizeMaxZ(data.getInt(0));
    }
    else if (event == "SKEL_RECORD"){
        if(isSkeleton) {
            string path = "skeleton/" + skeletonPath->getValueText();
            if(isLive) {
//                liveUser.recordCalibrationData(path);
            }else {
//                playUser.recordCalibrationData(path);
            }
        }
    }
    else if (event == "SKEL_FORCE_RELOAD"){
        
    }
    
}

void testApp::setupRecording(string _filename) {

#ifdef TARGET_OSX // only working on Mac at the moment
	hardware.setup();				// libusb direct control of motor, LED and accelerometers
	hardware.setLedOption(LED_OFF); // turn off the led just for yacks (or for live installation/performances ;-)
#endif

	liveContext.setup();	// all nodes created by code -> NOT using the xml config file at all
	//recordContext.setupUsingXMLFile();
	liveDepth.setup(&liveContext);
	liveImage.setup(&liveContext);
	
//	liveUser.setup(&liveContext);
//	liveHandTracker.setup(&liveContext, 4);
    liveScene.setup(&liveContext, &liveDepth);
	
	liveContext.toggleRegisterViewport();
	liveContext.toggleMirror();
	
    cloud.setup(liveDepth.getWidth(), liveDepth.getHeight(), &liveDepth, &liveImage);
    
	oniRecorder.setup(&liveContext, ONI_STREAMING);	
	//oniRecorder.setup(&recordContext, ONI_CYCLIC, 60); 
	//read the warning in ofxOpenNIRecorder about memory usage with ONI_CYCLIC recording!!!
	
}

void testApp::setupPlayback(string _filename) {
	
	playContext.shutdown();
	playContext.setupUsingRecording(ofToDataPath(_filename));
	playDepth.setup(&playContext);
	playImage.setup(&playContext);
	
//	playUser.setup(&playContext);
//  playHandTracker.setup(&playContext, 4);
    playScene.setup(&playContext, &playDepth);
	
	playContext.toggleRegisterViewport();
	playContext.toggleMirror();
    
    cloud.setup(playDepth.getWidth(), playDepth.getHeight(), &playDepth, &playImage);
	
}

//--------------------------------------------------------------
void testApp::update(){

#ifdef TARGET_OSX // only working on Mac at the moment
	if (kinectState)
        hardware.update();
#endif

	if (isLive) {
		if (kinectState) {
            // update all nodes
            liveContext.update();
            liveDepth.update();
            liveImage.update();
            
             // update tracking/recording nodes
//            if (isSkeleton) liveUser.update();

            if (isTracking)
                liveScene.update();
            
            if (isRecording) oniRecorder.update();
            
        }
	} else {
		
		// update all nodes
		playContext.update();
		playDepth.update();
		playImage.update();
		
		// update tracking/recording nodes
//		if (isSkeleton) playUser.update();
        
        if (isTracking)
             playScene.update();
		
	}
    
    if (isSendingOSC){
        if (isTracking) 
            sendUser();
        if (isTrackingHands)
            sendHands();
        if (isSkeleton)
            sendSkeleton();
    }
    
    cloud.updateCloud();
    
    gui.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofSetColor(255, 255, 255);
	
	glPushMatrix();
	glScalef(0.75, 0.75, 0.75);
    glTranslatef(0, 50, 0);
	
	if (isLive) {
        if (kinectState){
            
            liveDepth.draw(0,0,640,480);
            liveImage.draw(640, 0, 640, 480);
            

/*            if (isSkeleton) {
                liveUser.draw();
                
            }
            if (isTrackingHands)
                liveHandTracker.drawHands();
           
*/            
            if (isTracking){
                liveScene.drawUsers();
            }
            
            syphonDepth.publishTexture(liveDepth.getTexture());
            syphonImage.publishTexture(liveImage.getTexture());
        }
		
	} else {
		
		playDepth.draw(0,0,640,480);
		playImage.draw(640, 0, 640, 480);
			
/*		if (isSkeleton) {
			playUser.draw();
        
        }
		if (isTrackingHands)
			playHandTracker.drawHands();
*/       
        if (isTracking){
            playScene.drawUsers();
        }
        
        syphonDepth.publishTexture(playDepth.getTexture());
        syphonImage.publishTexture(playImage.getTexture());
	}
	
	glPopMatrix();
	
    drawPointClouds(0, 480);
    
	ofSetColor(255, 255, 0);
	
	string statusPlay		= (string)(isLive ? "LIVE STREAM" : "PLAY STREAM");
	string statusRec		= (string)(!isRecording ? "READY" : "RECORDING");
    string statusTracking		= (string)(isTracking ? "TRACKING USERS : " + (string)(isLive ? ofToString((int)liveScene.getUsers().size()) : ofToString((int)playScene.getUsers().size())) + "" : "NOT TRACKING");
//	string statusSkeleton	= (string)(isSkeleton ? "SKELETON USERS: " + (string)(isLive ? ofToString(liveUser.getNumberOfTrackedUsers()) : ofToString(playUser.getNumberOfTrackedUsers())) + "" : "NOT SKELETON USERS");
//	string statusHands		= (string)(isTrackingHands ? "TRACKING HANDS: " + (string)(isLive ? ofToString(liveHandTracker.getNumTrackedHands()) : ofToString(playHandTracker.getNumTrackedHands())) + ""  : "NOT TRACKING");
	string statusHardware;

#ifdef TARGET_OSX // only working on Mac at the moment
	ofPoint statusAccelerometers = hardware.getAccelerometers();
	stringstream	statusHardwareStream;
	
	statusHardwareStream 
	<< "ACCELEROMETERS:"
	<< " TILT: " << hardware.getTiltAngle() << "/" << hardware.tilt_angle
	<< " x - " << statusAccelerometers.x
	<< " y - " << statusAccelerometers.y
	<< " z - " << statusAccelerometers.z;
	
	statusHardware = statusHardwareStream.str();
#endif

	stringstream msg;
	
	msg
	<< "    playback/live streams : " << statusPlay << endl
    << "    users tracking     : " << statusTracking << endl
//	<< "    skeleton tracking     : " << statusSkeleton << endl
//	<< "    hand tracking         : " << statusHands << endl
	<< endl
	<< "File  : " << oniRecorder.getCurrentFileName() << endl
	<< "FPS   : " << ofToString(ofGetFrameRate()) << "  " << statusHardware << endl;
	
	ofDrawBitmapString(msg.str(), 20, 600);
    
    gui.draw();
	
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
    bool control_panel_ate_key = gui.keyPressed( key );
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	cloudRotate = y;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	gui.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	gui.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	gui.mouseReleased();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

void testApp::sendUser() {
    if (isSendingOSC){
        std::map<int, ofxSceneUser *> users ;
        if (isLive)
            users = liveScene.getUsers() ; 
        else
            users = playScene.getUsers() ; 
        std::map<int, ofxSceneUser *>::iterator it = users.begin() ; 
        std::map<int, ofxSceneUser *>::iterator itEnd = users.end() ; 
        while (it != itEnd) {
            switch (oscTrackingMode) {
                case 0 :
                    oscSend.sendUser(*(it->second));
                    break;
                case 1 :
                    oscSend.sendNormalizeUser(*(it->second));
                    break;
                case 2 :
                    oscSend.sendFullUser(*(it->second));
                    break;
                case 3 :
                    oscSend.sendUserSeparate(*(it->second));
                    break;
                default:
                    oscSend.sendUserSeparate(*(it->second));
                    break;
            }
            it ++ ; 
        }
    }
}


void testApp::sendSkeleton (){
    if (isSendingOSC && isSkeleton){
        ofxUserGenerator * us ;
/*        if (isLive)
            us = &liveUser ;
        else 
            us = &playUser ;
*/
        // pour chaque user tracké
        for (int i=0; i < us->getNumberOfTrackedUsers(); i++) {
            ofxTrackedUser *u = us->getTrackedUser(i);
            //envoi de l'user en osc
            oscSend.sendSkeleton(u, i);
        }
    }
}

void testApp::newUser (int & idUser) {
    if (isSendingOSC)
        oscSend.sendNewUser(idUser);
}
void testApp::lostUser (int & idUser) {
    if (isSendingOSC)
        oscSend.sendLostUser(idUser);
}
void testApp::deleteUser (int & idUser) {
    if (isSendingOSC)
        oscSend.sendDeleteUser(idUser);
}
void testApp::retrieveUser (int & idUser) {
    if (isSendingOSC)
        oscSend.sendRetrieveUser(idUser);
}

void testApp::sendHands() {
/*	vector<ofxTrackedHand*> hands ;
    int nbHands ;
    if (isLive){
        hands = liveHandTracker.tracked_hands;
        nbHands = liveHandTracker.getNumTrackedHands();
    }else {
        hands = playHandTracker.tracked_hands;
        nbHands = playHandTracker.getNumTrackedHands();
    }
    
    vector<ofxTrackedHand*>::iterator it = hands.begin();
    vector<ofxTrackedHand*>::iterator itEnd = hands.end();
    int num = 0 ; 
    
    while (it!=itEnd && isTrackingHands && num < nbHands) {
        switch (oscTrackingMode) {
            case 0 :
                oscSend.sendHand((*it)->handID, (*it)->projectPos);
                break;
            case 1 :
                oscSend.sendHand((*it)->handID, (*it)->progPos);
                break;
            case 2 :
                oscSend.sendHand((*it)->nID, (*it)->progPos);
                break;
            case 3 :
                oscSend.sendHandSeparate((*it)->handID, (*it)->progPos);
                break;
            default:
                oscSend.sendHand((*it)->handID, (*it)->progPos);
                break;
        }
        it ++;
        num ++;
    }
 */
}


void testApp::drawPointClouds(int x, int y){
    glPushMatrix();
	
    glTranslatef(x, y, -500);
    glTranslatef(320, 240, 0);
    glRotatef(cloudRotate, 0, 1, 0);
    glTranslatef(-320, -240, 0);
    cloud.drawCloud();
    /*
	int w = liveDepth.getWidth();
	int h = liveDepth.getHeight();
	
	glTranslatef(w, h/2, -500);
	
	
	glBegin(GL_POINTS);
	
	int step = 1;
	
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
            XnVector3D coordImg, coordWorld ;
            coordImg.X = x ;
            coordImg.Y = y ;
            coordImg.Z = liveDepth.getPixelDepth(x,y) ;
            liveDepth.getXnDepthGenerator().ConvertProjectiveToRealWorld(1, &coordImg, &coordWorld);
            
			ofPoint pos = ofPoint(coordWorld.X, coordWorld.Y, coordWorld.Z);
			ofColor color = liveImage.getXnImageGenerator().getWorldColorAt(x, y);
			glColor4ub((unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b, (unsigned char)color.a);
			glVertex3f(pos.x, pos.y, pos.z);
		}
	}
	
	glEnd();
	*/
	glColor3f(1.0f, 1.0f, 1.0f);
	
	glPopMatrix();
}

