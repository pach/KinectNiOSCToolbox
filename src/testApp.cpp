#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	
	isLive			= true;
	isTracking		= false;
	isTrackingHands	= true;
	isRecording		= false;
	isCPBkgnd		= true;

	
	//setupRecording();
	
	ofBackground(0, 0, 0);
    
    recordTimestamp = false;
    isSendingOSC = false;
    kinectState = false;
    isSkeleton = false;
    replayPath = "";
    
    setupgui();
    

    
}

void testApp::setupgui() {
    ofxControlPanel::setBackgroundColor(simpleColor(30, 30, 30, 150));
    ofxControlPanel::setTextColor(simpleColor(255, 255, 255, 255));
    
    gui.setup("kinect tools", 0, 0, ofGetWidth(), 500);
    gui.addPanel("kinect tools", 3, false);
    
    gui.setWhichPanel(0);
    gui.setWhichColumn(0);
    
    gui.addSlider("motor pos", "MOTOR", 0, -31., 31., true);
    gui.addToggle("enable kinect", "KINECT_STATE", false);
    gui.addToggle("record", "RECORD_STATE", false);
    gui.addToggle("add timestamp to record files", "RECORD_TIME", false);
    recPath = gui.addTextInput("record path", "path", ofGetWidth()/3.-10);
    gui.addToggle("replay", "REPLAY_STATE", false);
    replayFile.listDir("record/");
    gui.addFileLister("replay path", &replayFile, ofGetWidth()/3.-10, 50);
    
    gui.setWhichColumn(1);
    gui.addToggle("activate tracking", "TRACKING_STATE", true);
    gui.addToggle("activate hand", "HAND_STATE", false);
    gui.addToggle("activate skeleton", "SKELETON_STATE", false);
    
    gui.setWhichColumn(3);
    gui.addToggle("enable OSC", "OSC_STATE", true);
    oscPath = gui.addTextInput("OSC addr", "localhost", ofGetWidth()/3.-10);
    oscPort = gui.addTextInput("OSC port", "8888", ofGetWidth()/3.-10);
    gui.addToggle("update OSC", "OSC_UPDATE", false);
    vector<string> oscTrackMode ;
    oscTrackMode.push_back("normal user"); 
    oscTrackMode.push_back("full user");
    oscTrackMode.push_back("separate data");
    gui.addMultiToggle("OSC tracking mode", "OSC_TRACKING_MODE", 0, oscTrackMode);
    
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
        kinectState = data.getInt(0);
        if (kinectState){
            setupRecording();
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
            printf("connecting osc\n");
            oscSend.connect(oscPath->getValueText(), ofToInt(oscPort->getValueText()));
        }
    } 
    else if (event == "OSC_UPDATE"){
        oscSend.connect(oscPath->getValueText(), ofToInt(oscPort->getValueText()));
        std::cout<<"update osc : "<< oscPath->getValueText() << ", "<< oscPort->getValueText()<<std::endl;
        gui.setValueB("OSC_UPDATE", false);
    } 
    else if (event == "OSC_TRACKING_MODE"){
        oscTrackingMode = data.getInt(0);
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
	
	liveUser.setup(&liveContext);
	liveHandTracker.setup(&liveContext, 4);
    liveScene.setup(&liveContext, &liveDepth);
	
	liveContext.toggleRegisterViewport();
	liveContext.toggleMirror();
	
    
    
	oniRecorder.setup(&liveContext, ONI_STREAMING);	
	//oniRecorder.setup(&recordContext, ONI_CYCLIC, 60); 
	//read the warning in ofxOpenNIRecorder about memory usage with ONI_CYCLIC recording!!!
	
}

void testApp::setupPlayback(string _filename) {
	
	playContext.shutdown();
	playContext.setupUsingRecording(ofToDataPath(_filename));
	playDepth.setup(&playContext);
	playImage.setup(&playContext);
	
	playUser.setup(&playContext);
    playHandTracker.setup(&playContext, 4);
    playScene.setup(&playContext, &playDepth);
	
	playContext.toggleRegisterViewport();
	playContext.toggleMirror();
    
    
	
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
            if (isSkeleton) liveUser.update();

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
		if (isSkeleton) playUser.update();
        
        if (isTracking)
             playScene.update();
		
	}
    
    if (isSendingOSC){
        if (isTracking) 
            sendUser();
        if (isTrackingHands)
            sendHands();
    }
    
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
            

            if (isSkeleton) {
                liveUser.draw();
                
            }
            if (isTrackingHands)
                liveHandTracker.drawHands();
           
            if (isTracking){
                liveScene.drawUsers();
            }
        }
		
	} else {
		
		playDepth.draw(0,0,640,480);
		playImage.draw(640, 0, 640, 480);
			
		if (isSkeleton) {
			playUser.draw();
        
        }
		if (isTrackingHands)
			playHandTracker.drawHands();
       
        if (isTracking){
            playScene.drawUsers();
        }
	}
	
	glPopMatrix();
	
	ofSetColor(255, 255, 0);
	
	string statusPlay		= (string)(isLive ? "LIVE STREAM" : "PLAY STREAM");
	string statusRec		= (string)(!isRecording ? "READY" : "RECORDING");
    string statusTracking		= (string)(isTracking ? "TRACKING USERS : " + (string)(isLive ? ofToString((int)liveScene.getUsers().size()) : ofToString((int)playScene.getUsers().size())) + "" : "NOT TRACKING");
	string statusSkeleton	= (string)(isSkeleton ? "SKELETON USERS: " + (string)(isLive ? ofToString(liveUser.getNumberOfTrackedUsers()) : ofToString(playUser.getNumberOfTrackedUsers())) + "" : "NOT SKELETON USERS");
	string statusSmoothSkel = (string)(isLive ? ofToString(liveUser.getSmoothing()) : ofToString(playUser.getSmoothing()));
	string statusHands		= (string)(isTrackingHands ? "TRACKING HANDS: " + (string)(isLive ? ofToString(liveHandTracker.getNumTrackedHands()) : ofToString(playHandTracker.getNumTrackedHands())) + ""  : "NOT TRACKING");
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
	<< "    skeleton tracking     : " << statusSkeleton << endl
	<< "( / ) : smooth skely (openni) : " << statusSmoothSkel << endl
	<< "    hand tracking         : " << statusHands << endl
	<< endl
	<< "File  : " << oniRecorder.getCurrentFileName() << endl
	<< "FPS   : " << ofToString(ofGetFrameRate()) << "  " << statusHardware << endl;
	
	ofDrawBitmapString(msg.str(), 20, 560);
    
    gui.draw();
	
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	float smooth;
	
	switch (key) {
#ifdef TARGET_OSX // only working on Mac at the moment
		case 357: // up key
			hardware.setTiltAngle(hardware.tilt_angle++);
			break;
		case 359: // down key
			hardware.setTiltAngle(hardware.tilt_angle--);
			break;
#endif
		case 'b':
		case 'B':
			isCPBkgnd = !isCPBkgnd;
			break;
		case '9':
		case '(':
			smooth = liveUser.getSmoothing();
			if (smooth - 0.1f > 0.0f) {
				liveUser.setSmoothing(smooth - 0.1f);
				playUser.setSmoothing(smooth - 0.1f);
			}
			break;
		case '0':
		case ')':
			smooth = liveUser.getSmoothing();
			if (smooth + 0.1f <= 1.0f) {
				liveUser.setSmoothing(smooth + 0.1f);
				playUser.setSmoothing(smooth + 0.1f);
			}
			break;
		case ';':
		case ':':
			smooth = liveHandTracker.getSmoothing();
			if (smooth - 0.1f > 0.0f) {
				liveHandTracker.setSmoothing(smooth -  0.1f);
				playHandTracker.setSmoothing(smooth -  0.1f);
			}
			break;
		case '\'':
		case '\"':
			smooth = liveHandTracker.getSmoothing();
			if (smooth + 0.1f <= 1.0f) {
				liveHandTracker.setSmoothing(smooth +  0.1f);
				playHandTracker.setSmoothing(smooth +  0.1f);
			}
			break;
		case 'r':		
			liveContext.toggleRegisterViewport();
			break;
        default:
			break;
	}	
    
    bool control_panel_ate_key = gui.keyPressed( key );
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
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
                oscSend.sendUser(it->first, *(it->second));
                break;
            case 1 :
                oscSend.sendFullUser(it->first, *(it->second));
                break;
            case 2 :
                oscSend.sendUserSeparate(it->first, *(it->second));
                break;
            default:
                oscSend.sendUser(it->first, *(it->second));
                break;
        }
		it ++ ; 
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
	vector<ofxTrackedHand*> hands ;
    if (isLive){
        hands = liveHandTracker.tracked_hands;
    }else {
        hands = playHandTracker.tracked_hands;
    }
    
    vector<ofxTrackedHand*>::iterator it = hands.begin();
    vector<ofxTrackedHand*>::iterator itEnd = hands.end();
    
    while (it!=itEnd && isTrackingHands) {
        switch (oscTrackingMode) {
            case 0 :
                oscSend.sendHand((*it)->nID, (*it)->progPos);
                break;
            case 1 :
                oscSend.sendHand((*it)->nID, (*it)->progPos);
                break;
            case 2 :
                oscSend.sendHandSeparate((*it)->nID, (*it)->progPos);
                break;
            default:
                oscSend.sendHand((*it)->nID, (*it)->progPos);
                break;
        }
        it ++;
    }
}

