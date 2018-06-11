#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofBackground(0);
	ofSetVerticalSync(true);
	
	// game state
	bIsLicking = false;
	levelNum = 0;
	
    // KINECT SETUP
    
	kinect.setRegistration(true);
	bHasKinect = kinect.init() && kinect.open();
	
	kinectNearThresh.set("kinect near thresh", 225, 0, 255);
	kinectFarThresh.set("kinect far thresh", 147, 0, 255);
	kinectRoiTL.set("kinect ROI TL", ofPoint(100,100), ofPoint(0,0), ofPoint(640,480));
	kinectRoiBR.set("kinect ROI BR", ofPoint(540,360), ofPoint(0,0), ofPoint(640,480));
	bUseKinect.set("use kinect", bHasKinect);
	bDrawKinect.set("draw kinect", false);
	
	
	// OPEN CV
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	
	// AUDIO

	lickSound.load("sounds/lick.wav");
	lickSound.setMultiPlay(false);
	lickVolume.set("lick volume", 0.85f, 0.0f, 1.0f);
	lickSound.setVolume(lickVolume);
	
	winSound.load("sounds/win.wav");
	winSound.setMultiPlay(false);
	winSound.setVolume(lickVolume);

	music.load("sounds/ICE CREAM LICK (Original Mix) - Final Mix 1 - Siyoung 2015 (24 Bit MSTR).wav");
	music.setLoop(true);
	musicVolume.set("music volume", 0.7f, 0.0f, 1.0f);
	music.setVolume(musicVolume);
    music.play();
	
	// ANIMATIONS
	
	// level clear / win anim
	for(int i =0; i < N_WIN_IMAGES; i++){
		winAnimation[i].load("special_fx/wink_" + ofToString(i, 2, '0') + ".png");
	}
	
	// ADD LEVELS
	
	Level l0;
	levels.push_back(l0);
	Level l1;
	l1.tongueVel = ofVec2f(3.f, 0.f);
	l1.levelN = 1;
	levels.push_back(l1);
	Level l2;
	l2.tongueVel = ofVec2f(-3.f, 0.f);
	l2.meltDuration = 4.f;
	l2.bHasChocolate = true;
	l2.levelN = 2;
	levels.push_back(l2);
	Level l3;
	l3.tongueVel = ofVec2f(5.f, 0.f);
	l3.meltDuration = 5.f;
	l3.numSprinkles = 15;
	l3.levelN = 3;
	levels.push_back(l3);
	Level l4;
	l4.tongueVel = ofVec2f(-5.f, 0.f);
	l4.meltDuration = 4.f;
	l4.bHasChocolate = true;
	l4.numSprinkles = 15;
	l4.levelN = 4;
	levels.push_back(l4);
	Level l5;
	l5.tongueVel = ofVec2f(5.f, 2.f);
	l5.meltDuration = 4.f;
	l5.bHasChocolate = true;
	l5.numSprinkles = 20;
	l5.iceCreamTint = ofColor(240,130,0);	// strawberry?
	l5.levelN = 5;
	levels.push_back(l5);
	for (int i=6; i<100; i++){
		Level l;
		l.tongueVel.x = ofMap(i, 6, 100, 5.f, 10.f);
		l.tongueVel.y = ofMap(i, 6, 100, 2.f, 5.f);
		if (i%2==0) l.tongueVel *= -1;	// reverse dir
		l.meltDuration = ofMap(i, 6, 100, 4.f, 1.f);
		l.bHasChocolate = true;
		l.numSprinkles = ofMap(i, 6, 100, 20, 40);
		float hue = ofRandom(0, 255);
		l.iceCreamTint = ofColor::fromHsb(hue, 200, 255);
		l.levelN = i;
		levels.push_back(l);
	}
	
	// PARAMS
	
	gui.setup();
	gui.setName("SETTINGS");
	gui.setHeaderBackgroundColor(ofColor(50,0,50));
	gui.setDefaultBackgroundColor(ofColor(50,0,50));
	
	gui.add(kinectStatusLabel.setup("kinect status", (bHasKinect ? "OK" : "NONE")));
	kinectStatusLabel.setBackgroundColor(bHasKinect ? ofColor(0,75,25) : ofColor(75,0,25));
	
	kinectParams.setName("KINECT PARAMS");
	kinectParams.add(bUseKinect);
	kinectParams.add(bDrawKinect);
	kinectParams.add(kinectNearThresh);
	kinectParams.add(kinectFarThresh);
	kinectParams.add(kinectRoiTL);
	kinectParams.add(kinectRoiBR);
	gui.add(kinectParams);
	
	soundParams.setName("SOUND PARAMS");
	soundParams.add(lickVolume);
	soundParams.add(musicVolume);
	gui.add(soundParams);
	
	vizParams.setName("VIZ PARAMS");
	vizParams.add(bgFps.set("bg fps", 7.f, 0.1f, 60.f) );
	vizParams.add(bDrawTongueTip.set("draw tongue tip", true));
	vizParams.add(bDrawColliders.set("draw colliders", false));
	gui.add(vizParams);
	
	// -- LISTENERS
	
	lickVolume.addListener(this, &ofApp::lickVolumeChanged);
	musicVolume.addListener(this, &ofApp::musicVolumeChanged);
	bgFps.addListener(this, &ofApp::bgFpsChanged);
	
	restart();
	
	iceCream.setAnimFps(6.f);
	
}

//--------------------------------------------------------------
void ofApp::update(){

    bg.update();
    iceCream.update();

	
	// CONTROLLER POSITION

    ofVec2f tonguePos = ofVec2f(ofGetMouseX(), ofGetMouseY());
	
	// USE KINECT IF ATTACHED
	
    if (bHasKinect){
        kinect.update();
        
        //tonguePos = kinect.tongueTip pos
        
        if(kinect.isFrameNew()) {

            //grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
			grayImage.setFromPixels(kinect.getDepthPixels());
			grayThreshNear = grayImage;
            grayThreshFar = grayImage;
            grayThreshNear.threshold(kinectNearThresh, true);
            grayThreshFar.threshold(kinectFarThresh);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
            
			auto pix = grayImage.getPixels();
			
			
			// CROP KINECT IMAGE
			// TODO: change to cv ROI
			int pixIndex = 0;
			for (int y = 0; y < grayImage.getHeight(); y++){
				for (int x = 0; x < grayImage.getWidth(); x++){
					
					// erase out of bounds pix
					if 			(x < kinectRoiTL.get().x){
						pix[pixIndex] = 0;
					} else if 	(x > kinectRoiBR.get().x) {
						pix[pixIndex] = 0;
					} else if 	(y < kinectRoiTL.get().y) {
						pix[pixIndex] = 0;
					} else if 	(y > kinectRoiBR.get().y) {
						pix[pixIndex] = 0;
					}
					
					pixIndex++;
				}
			}
			
			// update the cv images
			grayImage.flagImageChanged();
			
			contourFinder.findContours(grayImage, 200, (kinect.width*kinect.height)/2, 1, false);
			
			if (contourFinder.nBlobs > 0){
				
				tongueOutline = ofPolyline(contourFinder.blobs[0].pts);
				tongueOutline.close();
				tongueOutlineSmooth = tongueOutline.getSmoothed(20,1);
				tongueOutlineSmooth.close();
				
//				if (!(DO_KINECT_FRONT)){
				// note: assume top mounted kinect
				
				ofPoint tipTarget = contourFinder.blobs[0].centroid;
				tipTarget.y = 0;
				
				tip = tongueOutlineSmooth.getClosestPoint(tipTarget);
				tip.y += 10;
				rawTip = tip;
				
				//get depth of tip
				float d = kinect.getDistanceAt(tip.x, tip.y);
				if (d > 0){
					
					//cout << "raw tip depth: " << d;
					//cout << ", raw tip x: " << tip.x << endl;
					
					// TODO: move to params
					tip.x = ofMap(tip.x, FULL_LEFT, FULL_RIGHT, 0, ofGetWidth(), true);
					tip.y = ofMap(d, DEPTH_TOP, DEPTH_BOTTOM, 320, ofGetHeight(), true);
					
					tonguePos.set(tip);
				}
				
//				} else {
//
//					ofPoint tip = contourFinder.blobs[0].centroid;
//					// invert x
//					tip.x = ofMap(tip.x, FULL_LEFT, FULL_RIGHT, ofGetWidth(), 0, true);
//					tip.y = ofMap(tip.y, DEPTH_BOTTOM, DEPTH_TOP, ofGetHeight(), 0, true);
//
//					tonguePos.set(tip);
//				}
			}
        }
        
    }
    
    // update tongue coordinates
    tongue.update(tonguePos.x,tonguePos.y);
	
	// win
	if (iceCream.bAteCone){
		
		if (!bLevelIntro){
			// player beat level, start win animation
			bDrawWin = true;
			winIdx = 0;
			winSound.play();
			lastAnimFrameT = ofGetElapsedTimef();
			bLevelIntro = true;
			++levelNum;
			if (levelNum >= levels.size()){
				// win game?
				restart();
			} else {
				iceCream.refill();
				// update iceCream with level settings
				auto& l = levels[levelNum];
				iceCream.vel = l.tongueVel;
				if (l.bHasChocolate) iceCream.addChocolate();
				iceCream.addSprinkles(l.numSprinkles);
				iceCream.pause(true);
			}
		}
	}
	else if (iceCream.bDripDeath){
		bGameOver = true;
	}
	
	if (bLevelIntro) {
		float t = ofGetElapsedTimef();
		
		// 1. play win animation
		if (bDrawWin){
			bool bNextFrame = t - animFrameDur >= lastAnimFrameT;
			if (bNextFrame) {
				++winIdx;
				if (winIdx >= N_WIN_IMAGES){
					winIdx = -1;
					// 2. play level title
					levelTitleStartT = t;
					bDrawWin = false;
					bDrawLevelTitle = true;
				}
				lastAnimFrameT = t;
			}
		} else if (bDrawLevelTitle){
			if (t - levelTitleDur >= levelTitleStartT){
				bDrawLevelTitle = false;
			}
		}
		// 3. play pour animation
		else if (!iceCream.isReady()){
			iceCream.pause(false);
		}
		// 4. start melt timer / game play
		else if (!iceCream.bMelting){
			iceCream.startMelt(levels[levelNum].meltDuration);
			bLevelIntro = false;	// intro done
		}
	}

    
    // check if tongue is giving a lick
	if (!bIsLicking){
		if (tongue.isLicking()){
			
			/* check if tongue is touching ice cream */
			if (iceCream.checkCollision(tongue.pos)){
				
				iceCream.lick();
				++lickCounter;
				bIsLicking = true;
				lickSound.play();
				
				ofLogNotice("ofApp")	<< "licked!" << endl
				<< "\t\t lick: " << iceCream.lickState << " / " << iceCream.lickMax
				<< ", total licks: " << lickCounter;
			}
		}
	}
    else if (tongue.isMovingDown()){
		bIsLicking = false;
    }
	
	
	iceCream.update();

    
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	bg.draw();
	
	if (bDrawWin){
		if (winIdx >= 0 && winIdx < N_WIN_IMAGES){
			auto& w = winAnimation[winIdx];
			float scale = ofGetWidth() / w.getWidth();
			winAnimation[winIdx].draw(0,0,w.getWidth()*scale, w.getHeight()*scale);
		}
	}
	else if (bDrawLevelTitle){
		
		// TODO: replace with TTF
		ofDrawBitmapStringHighlight("LEVEL " + ofToString(levelNum), ofGetWidth()*.5, ofGetHeight()*.5);
		
	}
	else {
		iceCream.draw();
	}
	
	// DRAW TEETH over ice cream
	
    teeth.draw();
	
	
    
	if (bDrawKinect){
		grayImage.draw(10, 320, 400, 300);
		contourFinder.draw(10, 320, 400, 300);
		//tongueOutline.draw();
		tongueOutlineSmooth.draw();
		//ofSetColor(tipDepth, tipDepth, 0);
		ofDrawCircle(tip, 5);
		//ofSetColor(255);
	}
	if (bDrawTongueTip){
		if (tongue.isLicking()){
			ofSetColor(255,106,159);
		} else {
			ofSetColor(122, 53, 79);
		}
		ofDrawCircle(tongue.pos, 7);

		ofSetColor(255);
	}
	if (bDrawColliders){
		iceCream.drawColliders();
	}
	
	// TODO:  level display on screen
//	cout << "game level: " <<iceCream.gameLevel<<endl;
	
	if (bDrawGui){
		gui.draw();
	}
}

//--------------------------------------------------------------
void ofApp::restart(){
	iceCream.refill();
	
	bIsLicking = false;
	levelNum = 0;
	
	music.setPosition(0.f);	// restart music
	bLevelIntro = true;
	bDrawWin = false;
	bDrawLevelTitle = false;
	bGameOver = false;
	
	winIdx = -1;
	lickCounter = 0;
}

//--------------------------------------------------------------
void ofApp::lickVolumeChanged(float& vol){
	lickSound.setVolume(vol);
}

//--------------------------------------------------------------
void ofApp::musicVolumeChanged(float& vol){
	music.setVolume(vol);
}

//--------------------------------------------------------------
void ofApp::bgFpsChanged(float& fps){
	bg.setFrameRate(fps);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch(key){
            
//        case ('1'):
//            icLevelNum = 0;
//            break;
//        case ('2'):
//            icLevelNum = 1;
//            break;
//        case ('3'):
//            icLevelNum = 2;
//            break;
//        case ('4'):
//            icLevelNum = 3;
//            break;
//            
//        case (OF_KEY_UP):
//            cout << "lvlX[" << icLevelNum << "] is " << ++(iceCream.lvlX[icLevelNum]) << endl;
//            break;
//        
//        case (OF_KEY_DOWN):
//            cout << "lvlX[" << icLevelNum << "] is " << --(iceCream.lvlX[icLevelNum]) << endl;
//            break;
			
		case ('f'):
			ofToggleFullscreen();
			break;
			
		case ('g'):
			bDrawGui = !bDrawGui;
			break;
			
			// REMOVED FOR GUI CONTROL:
			
//        case ('k'):
//            if (bHasKinect){
//                bDrawKinect = !bDrawKinect;
//                cout << "draw kinect: " << bDrawKinect << endl;
//            }
//            break;
			
        // KINECT THRESHOLDING
//        case ('='):
//            kinectNearThresh++;
//            if (kinectNearThresh > 255){
//                kinectNearThresh = 255;
//            }
//            cout << "kinectNearThresh: " << kinectNearThresh << endl;
//            break;
//        case ('-'):
//            kinectNearThresh--;
//            if (kinectNearThresh < 0){
//                kinectNearThresh = 0;
//            }
//            cout << "kinectNearThresh: " << kinectNearThresh << endl;
//            break;
//        case ('0'):
//            kinectFarThresh++;
//            if (kinectFarThresh > 255){
//                kinectFarThresh = 255;
//            }
//            cout << "kinectFarThresh: " << kinectFarThresh << endl;
//            break;
//        case ('9'):
//            kinectFarThresh--;
//            if (kinectFarThresh < 0){
//                kinectFarThresh = 0;
//            }
//            cout << "kinectFarThresh: " << kinectFarThresh << endl;
//            break;

    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
