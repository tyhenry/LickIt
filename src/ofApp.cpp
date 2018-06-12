#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	// KINECT SETUP
	
	kinect.setRegistration(true);
	kinect.init();
	kinect.open();
	bHasKinect = kinect.isConnected();
	
	kinectNearThresh.set("kinect near thresh", 225, 0, 255);
	kinectFarThresh.set("kinect far thresh", 147, 0, 255);
	kinectRoiTL.set("kinect ROI TL", ofPoint(100,100), ofPoint(0,0), ofPoint(640,480));
	kinectRoiBR.set("kinect ROI BR", ofPoint(540,360), ofPoint(0,0), ofPoint(640,480));
	bUseKinect.set("use kinect", bHasKinect);
	bDrawKinect.set("draw kinect", false);
	
	
	ofBackground(0);
	ofSetVerticalSync(true);
	
	// game state
	bIsLicking = false;
	levelNum = 0;

	
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
	// level title anim
	for(int i =0; i < N_MORE_IMAGES; i++){
		moreAnimation[i].load("level_title/more_" + ofToString(i, 2, '0') + ".png");
	}
	
	// ADD LEVELS
	
	Level l0;
	l0.meltDuration = 194.5f;
	levels.push_back(l0);
	Level l1;
	l1.tongueVel = ofVec2f(0.5f, 0.f);
	l1.levelN = 1;
	levels.push_back(l1);
	Level l2;
	l2.tongueVel = ofVec2f(-0.5f, 0.f);
	l2.meltDuration = 7.f;
	l2.bHasChocolate = true;
	l2.levelN = 2;
	levels.push_back(l2);
	Level l3;
	l3.tongueVel = ofVec2f(2.f, 0.f);
	l3.meltDuration = 6.f;
	l3.numSprinkles = 15;
	l3.levelN = 3;
	levels.push_back(l3);
	Level l4;
	l4.tongueVel = ofVec2f(-2.f, 0.f);
	l4.meltDuration = 5.f;
	l4.bHasChocolate = true;
	l4.numSprinkles = 15;
	l4.levelN = 4;
	levels.push_back(l4);
	Level l5;
	l5.tongueVel = ofVec2f(3.f, 1.f);
	l5.meltDuration = 4.f;
	l5.bHasChocolate = true;
	l5.numSprinkles = 20;
	l5.iceCreamTint = ofColor(240,130,0);	// strawberry?
	l5.levelN = 5;
	levels.push_back(l5);
	for (int i=6; i<100; i++){
		Level l;
		l.tongueVel.x = ofMap(i, 6, 100, 3.f, 10.f);
		l.tongueVel.y = ofMap(i, 6, 100, 1.f, 5.f);
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
	
	controllerParams.setName("CONTROLLER PARAMS");
//	controllerParams.add(bUseKinect);
	controllerParams.add(bMouseControl.set("mouse control", !bHasKinect));
	controllerParams.add(bDrawKinect);
	controllerParams.add(kinectNearThresh);
	controllerParams.add(kinectFarThresh);
//	controllerParams.add(kinectRoiTL);
//	controllerParams.add(kinectRoiBR);
	controllerParams.add(kinectMinDepth.set("kinect min depth", 1050, 0, 2000));
	controllerParams.add(kinectMaxDepth.set("kinect max depth", 1800, 0, 2000));
	controllerParams.add(kinectMinX.set("kinect min x", 240, 0, 640));
	controllerParams.add(kinectMaxX.set("kinect max x", 440, 0, 640));
	controllerParams.add(controlBoundsTL.set("control bounds TL", glm::vec2(0,200), glm::vec2(0,0), glm::vec2(ofGetWidth(),ofGetHeight())));
	controllerParams.add(controlBoundsBR.set("control bounds BR", glm::vec2(ofGetWidth(),ofGetHeight()), glm::vec2(0,0), glm::vec2(ofGetWidth(),ofGetHeight())));
	controllerParams.add(frameBoundsTL.set("ice cream bounds TL", glm::vec2(ofGetWidth()*.15f,ofGetHeight()*.2314f), glm::vec2(0,0), glm::vec2(ofGetWidth(),ofGetHeight())));
	controllerParams.add(frameBoundsBR.set("ice cream bounds BR", glm::vec2(ofGetWidth()*.85f,ofGetHeight()*1.1f), glm::vec2(0,0), glm::vec2(ofGetWidth(),ofGetHeight()+300)));
	gui.add(controllerParams);
	
	soundParams.setName("SOUND PARAMS");
	soundParams.add(lickVolume);
	soundParams.add(musicVolume);
	gui.add(soundParams);
	
	vizParams.setName("VIZ PARAMS");
	vizParams.add(bgFps.set("bg fps", 7.f, 0.1f, 60.f) );
	vizParams.add(bDrawTongueTip.set("draw tongue tip", true));
	gui.add(vizParams);
	
	// -- LISTENERS
	
	lickVolume.addListener(this, &ofApp::lickVolumeChanged);
	musicVolume.addListener(this, &ofApp::musicVolumeChanged);
	bgFps.addListener(this, &ofApp::bgFpsChanged);
	frameBoundsTL.addListener(this, &ofApp::frameBoundsChanged);	// ice cream bounce bounds
	frameBoundsBR.addListener(this, &ofApp::frameBoundsChanged);
	
	
	// UI
	
	font.load(ofToDataPath("SFCompactRounded-Semibold.otf"), 48);
	levelTitleDur = 4.f;
	
	// ICE CREAM
	
	iceCream.setAnimFps(6.f);
	auto rect = ofRectangle(frameBoundsTL.get(), frameBoundsBR.get());
	iceCream.setFrameBounds(rect);
	
	restart();
	
}

//--------------------------------------------------------------
void ofApp::update(){

    bg.update();
    iceCream.update();
	
	// CONTROLLER POSITION
	
    if (bMouseControl || !bHasKinect){
		
		// mouse control
		tonguePos.x = ofClamp(ofGetMouseX(), controlBoundsTL.get().x, controlBoundsBR.get().x);
		tonguePos.y = ofClamp(ofGetMouseY(), controlBoundsTL.get().y, controlBoundsBR.get().y);

	} else {

		// USE KINECT IF ATTACHED

        kinect.update();
        
        //tonguePos = kinect.tongueTip pos
        
        if(kinect.isFrameNew()) {

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
					tip.x = ofMap(tip.x, kinectMinX, kinectMaxX, controlBoundsTL.get().x, controlBoundsBR.get().x, true);
					tip.y = ofMap(d, kinectMinDepth, kinectMaxDepth, controlBoundsTL.get().y, controlBoundsBR.get().y, true);
					
					tonguePos.set(tip);
				}
			}
        }
	}
    
    // update tongue coordinates
    tongue.update(tonguePos.x,tonguePos.y);
	
	// win
	if (iceCream.bAteCone){
		
		if (!bLevelIntro){
			
			bLevelIntro = true;
			++levelNum;
			
			// player beat level, start win animation
			bDrawWin = true;
			winIdx = 0;
			winSound.play();
			lastAnimFrameT = ofGetElapsedTimef();
			
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
				iceCream.iceCreamTint = l.iceCreamTint;
				iceCream.pause(true);
			}
		}
	}
	else if (iceCream.bDripDeath){
		if (!bGameOver){
			iceCream.pause(true);
			bGameOver = true;
			teeth.beginClose(8.f);
		} else {
			if (teeth.bMouthClosed){
				
				// TODO: init game over screen
				
				teeth.setMouthOpen(true);
				restart();
			}
		}
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
					moreIdx = 0;
				}
				lastAnimFrameT = t;
			}
		} else if (bDrawLevelTitle){
			moreIdx = ofMap(t, levelTitleStartT, levelTitleStartT + levelTitleDur, 0, N_MORE_IMAGES-1, true);
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
	else if (!bGameOver) {
		
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
	}
	
	
	iceCream.update();
	
	teeth.update();

    
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
		
		if (moreIdx >= 0 && moreIdx < N_MORE_IMAGES){
			moreAnimation[moreIdx].draw(0,0,ofGetWidth(), ofGetHeight());
		}
		
		ofPushStyle();
		ofColor c = ofColor::fromHex(0x5c3317);
		c.a = ofMap(moreIdx, float(N_MORE_IMAGES) * .33f, float(N_MORE_IMAGES) * .66f, 0, 255, true);
		ofSetColor(c);
		string levelStr = "Level " + ofToString(levelNum+1) + "!";
		glm::vec2 levelTitlePos;
		levelTitlePos.x = ofGetWidth() * 0.5f - font.stringWidth(levelStr) * 0.5f + ofNoise(ofGetElapsedTimef() + 1000.f) * 2.f;
		levelTitlePos.y = ofGetHeight() * 0.75f + ofNoise(ofGetElapsedTimef() * 2.f) * 10.f;
		
		font.drawString(levelStr, levelTitlePos.x, levelTitlePos.y);
		ofPopStyle();
	}
	else {
		iceCream.draw();
	}
	
	// DRAW TEETH over ice cream
	
    teeth.draw();
    
	if (bDrawKinect){
		
		grayImage.draw(ofGetWidth()-480, 0, 480, 360);
		contourFinder.draw(ofGetWidth()-480, 0, 480, 360);
//		tongueOutlineSmooth.draw();
		//ofSetColor(tipDepth, tipDepth, 0);
//		ofDrawCircle(tip, 5);
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
	
	if (bDrawGui){
		gui.draw();
		
		iceCream.drawColliders();
		
		// draw ice cream frame bounds
		ofPushStyle();
		ofNoFill();
		ofSetColor(0,0,255);
		ofDrawRectangle(iceCream.frameBounds);
		ofDrawBitmapStringHighlight("ice cream bounds", iceCream.frameBounds.getTopLeft()+glm::vec2(0,20), ofColor(0,0,255));
		ofPopStyle();
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
		
			
		case ('f'):
			ofToggleFullscreen();
			break;
			
		case ('g'):
			bDrawGui = !bDrawGui;
			break;
			
        case ('m'):
            bMouseControl = !bMouseControl;
            break;

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
