#ifndef _TEST_APP
#define _TEST_APP

// #define USE_IR // Uncomment this to use infra red instead of RGB cam...

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxSceneAnalyzer.h"
#include "ofxControlPanel.h"
#include "OSCUserSender.h"

#include "PointClouds.h"

class testApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
    
    void setupgui();
	
    void eventsgui(guiCallbackData & data);
    
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
    
    void newUser (int & idUser) ; 
	void lostUser (int & idUser) ; 
	void deleteUser (int & idUser) ; 
	void retrieveUser (int & idUser) ; 
    void sendUser ();
    void sendHands ();
    void sendSkeleton ();
    
    void drawPointClouds(int x, int y);
	
	void	setupRecording(string _filename = "");
	void	setupPlayback(string _filename);
	string	generateFileName();
	
	bool				isLive, isTracking, isRecording, isCPBkgnd;
	bool				isTrackingHands;
	
	ofxOpenNIContext	liveContext, playContext;
	ofxDepthGenerator	liveDepth, playDepth;

#ifdef USE_IR
	ofxIRGenerator		liveImage, playImage;
#else
	ofxImageGenerator	liveImage, playImage; 
#endif
	
	ofxHandGenerator	liveHandTracker, playHandTracker;
	
	ofxUserGenerator	liveUser, playUser;
	ofxOpenNIRecorder	oniRecorder;
    
    ofxSceneAnalyzer    liveScene, playScene ;
    
#ifdef TARGET_OSX // only working on Mac at the moment
	ofxHardwareDriver	hardware;
#endif

    
    OSCUserSender oscSend;
    bool recordTimestamp, isSendingOSC, isSkeleton;
    bool kinectState;
    
    ofxControlPanel gui;
    simpleFileLister replayFile;
    string replayPath ;
    
    guiTypeTextInput *recPath ;
    guiTypeTextInput *oscPath ;
    guiTypeTextInput *oscPort ;
    int oscTrackingMode ;
    
    PointClouds cloud ;

};

#endif
