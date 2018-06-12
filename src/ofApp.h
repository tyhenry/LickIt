#pragma once

#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"


#include "ofMain.h"
#include "Background.h"
#include "IceCream.h"
#include "UI.h"
#include "Tongue.h"

// TODO: move to params
// TOP MOUNT KINECT VALUES:

#define DEPTH_TOP 1050
#define DEPTH_BOTTOM 1800
#define FULL_LEFT 240
#define FULL_RIGHT 440

#define CROP_TOP 100
#define CROP_BOTTOM 360
#define CROP_LEFT 100
#define CROP_RIGHT 540

static const int N_WIN_IMAGES = 17;

struct Level {
	int levelN = 0;
	ofVec2f tongueVel = ofVec2f(0.f);
	float meltDuration = 10.f;	// seconds
	bool bHasChocolate = false;
	int numSprinkles = 0;
	ofColor iceCreamTint = ofColor(255);
};

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
	void exit() { kinect.close(); }
	
	void restart();
	
	// param listeners
	void lickVolumeChanged(float& vol);
	void musicVolumeChanged(float& vol);
	void bgFpsChanged(float& fps);
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
	
	ofTrueTypeFont font;
    
    Background bg;
    IceCream iceCream;
    UI teeth;
	
	vector<Level> levels;
    
    Tongue tongue;
    //bool gotLick = false;
    
    ofxKinect kinect;
	
	int levelNum = 0;
	int winDuration = 120;
	int lickCounter = 0;
	
	bool bIsLicking, bLevelIntro, bDrawWin, bDrawLevelTitle, bGameOver;
	
	// OPENCV
    
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImage; 		// grayscale depth image
    ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage grayThreshFar; 	// the far thresholded image
	ofPolyline tongueOutline;
	ofPolyline tongueOutlineSmooth;
	ofPoint tip;
	ofPoint rawTip;
	float rawDepth;
	float tipDepth;
    
    ofxCvContourFinder contourFinder;
	
    
	// AUDIO
	
	ofSoundPlayer lickSound, winSound, gameOverSound;
    ofSoundPlayer music;
	
	// ANIMATIONS
	
	float lastAnimFrameT = 0.f;
	float animFrameDur = 1.f / 24.f;
	
	ofImage winAnimation[N_WIN_IMAGES];		// win level star
	int winIdx = -1;
	
	float levelTitleStartT = 0.f;
	float levelTitleDur = 1.f;				// seconds
	
	// PARAMS
	
	ofxPanel gui;
	ofParameterGroup kinectParams;
	ofxLabel kinectStatusLabel;
	ofParameter<int> kinectNearThresh, kinectFarThresh;
	ofParameter<int> kinectMinDepth, kinectMaxDepth;
	ofParameter<ofVec2f> kinectRoiTL, kinectRoiBR;
	ofParameter<bool> bUseKinect, bDrawKinect, bMouseControl;
	
	ofParameterGroup soundParams;
	ofParameter<float> lickVolume, musicVolume;
	
	ofParameterGroup vizParams;
	ofParameter<float> bgFps;
	ofParameter<bool> bDrawTongueTip, bDrawColliders;
	
	bool bHasKinect, bDrawGui;
    ofVec2f tonguePos;
    bool useMouse = true;
};
