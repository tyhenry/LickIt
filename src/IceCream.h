//
//  IceCream.h
//  tongueGame
//
//  Created by Miri Park on 12/8/15.
//
//

#include "ofMain.h"

// animations
static const int N_ICECREAM_IMAGES = 24;
static const int N_MELT_IMAGES = 75;
static const int N_REFILL_IMAGES = 14;
static const int N_CHOCOPOUR_IMAGES = 22;
static const int N_CHOCOLICK_IMAGES = 3;
//static const int N_WIN_IMAGES = 17;
// sprites
static const int N_SPRINKLE_SPRITES = 4;

// scaling from images
//static const float CONE_SCALE = 0.58f;
static const float CONE_HEIGHT_PCT = 0.769f;	// pct of screen height
static const float SPRINKLE_SCALE = 0.6f;



class IceCream {
public:
	
    IceCream();
	void update();
	bool checkCollision(ofVec2f pos);
	void lick();	// increase lickState, trigger animation
	
	void refill();
	void resetMelt();
	void startMelt(float timerSecs);
	
	void addChocolate() { bHasChocolate = true; }	// triggers pour animation
	void addSprinkles(int num = 30);
	
	void setFrameBounds(ofRectangle bounds) {
		frameBounds = bounds;
	}
	void setAnimFps(float fps) {
		animFrameDur = 1.f / ofClamp(fps, .1f, 60.f);
	}

    void draw();
	void drawSprinkles();
    void drawChoco();
	void drawColliders();	// debug
	
	void loadAssets();
	
	void reset();
	
	bool isFilled() { return bIsFilled; }		// refill animation complete
	bool isReady() {
		bool b = isFilled();
		if (bHasChocolate) b = b && bChocoPoured;	// refilled with chocolate
		return b;
	}
	
	void pause(bool enable) { bPaused = enable; }
	
	// position
	ofVec2f origPos;
	ofVec2f pos;
	ofVec2f size;
	ofVec2f vel;	// movement direction, speed
	ofRectangle frameBounds;
	float scaleFactor; // converts from image to screen rect
	
	// colliders
	vector<pair<ofRectangle, bool>> colliders;	// collider, on/off
	
	ofColor iceCreamTint = ofColor(255);
	
	// melt timing
	float meltStartedTime;
	float meltDuration;
	bool bMelting;
	
	// animation indices, -1 == off
	int meltIdx = -1;
	int refillIdx = -1;
	int chocoPourIdx = -1;
	int chocoLickIdx = -1;
	int winIdx = -1;
	
	float animFrameDur;
	float lastAnimFrameT;
	
	// toppings
	bool bHasChocolate;
	bool bHasSprinkles;
	
	bool bDripDeath;	// lose
	bool bAteCone;		// win
	
	bool bIsFilled, bChocoPoured, bPaused;
	
	int lickState;	// # licks (0-10)
	int lickMax;	// 10 total licks
	int lickAnimIdx;
	int lickAnimLen;
	int lickAnimEnd;
//	float lickAnimFps;
	
	
	// images
	ofImage iceCreamAnimation[N_ICECREAM_IMAGES];
	ofImage meltAnimation[N_MELT_IMAGES];
	
	ofImage refillAnimation[N_REFILL_IMAGES];	// soft serve flow
	ofImage chocoPourAnimation[N_CHOCOPOUR_IMAGES];	// chocolate pour
	ofImage chocoLickAnimation[N_CHOCOLICK_IMAGES];
	
//	ofImage winAnimation[N_WIN_IMAGES];		// win level star
	
	ofImage sprinkleSprites[N_SPRINKLE_SPRITES];
	vector<pair<int, ofVec2f>> sprinkles;	// sprite idx, pos
	
	ofImage coneImg;
	ofImage coneFrontImg;
	
	// TODO: move win / lose anim to ofApp?
	ofSoundPlayer winSound;
	ofSoundPlayer loseSound;
	
	
//    void melt();
////    void move();
//    void drawSprinkles();
//    void drawChoco();
//    void flow();
//
//    //gameLevel 1 = level1
//    void level1();
//    //gameLevel 3 = level2
//    void level2();
//    //gameLevel 6 = level3
//    void level3();
//    //gameLevel 7 = level4
//    void level4();
//
//    void level8();
//
//    void win();
	
//    void brainFreeze();
//
//    //when lose, resetting the whole game to level 0
//    void resetWholeGame();
    //void win();
    
   // bool won = false;
//    bool brainFrozen = false;
//    bool gotLick = false;
//    bool flowing = true;
////    int meltIndex = 0;
//    int lickIndex = 0;
//    int flowIndex = 0;
//    int winkIndex = 0;
//
//    int chocoIndex = 0;
//    int chocoSpeed = 9;
	
//    float moveIncrement = 5;
//
//    //sprinkles position
//
//
//    //ice cream colliders, 0-3
//    ofRectangle icLevels[4];
//    int lvlX[4]; // x offset
//
//    //keyframes of icecream disappearance: 29, 38, 43, 47, 51, 56, 60, 64, 69, 77
//
//    bool progressLevel = false;
//    int gameLevel = 0;
//
//    int speedLimit = 200;
//    int savedLickFrame;
//    int currentLickFrame;
//
//    int meltRate = 15;
//
//    int coneAlignment = 3;
//    //////flowAlignment needs to be fudged around with
//    int flowAlignment = 30;
//    int flowSpeed = 10;
//
////    bool dripDeath = false;
//    bool won = false;
};
