#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofBackground(0);
	ofSetVerticalSync(true);
	
	// game state
	bIsLicking = false;
	
	
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

	lickSound.load("sounds/slurp2.wav");
	lickSound.setMultiPlay(false);
	lickVolume.set("lick volume", 0.85f, 0.0f, 1.0f);
	lickSound.setVolume(lickVolume);

	music.load("sounds/ICE CREAM LICK (Original Mix) - Final Mix 1 - Siyoung 2015 (24 Bit MSTR).wav");
	music.setLoop(true);
	musicVolume.set("music volume", 0.7f, 0.0f, 1.0f);
	music.setVolume(musicVolume);
    music.play();
	
	
	// PARAMS
	
	bgFps.set("bg fps", 7.f, 0.1f, 60.f);
	
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
	vizParams.add(bgFps);
	vizParams.add(bDrawTongueTip);
	gui.add(vizParams);
	
	// -- LISTENERS
	
	lickVolume.addListener(this, &ofApp::lickVolumeChanged);
	musicVolume.addListener(this, &ofApp::musicVolumeChanged);
	bgFps.addListener(this, &ofApp::bgFpsChanged);
	
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
    
    // check if tongue is giving a lick
    if (tongue.isLicking){
        
        // if hasn't gotten ice cream yet on this lick
        if (!bIsLicking){
            
            /* check if tongue is touching ice cream */
            if (iceCream.checkCollision(tongue.pos)){
				
				iceCream.lick();
//				iceCream.lickState++;
//                cout << "licked ice cream: " << iceCream.lickState << endl;
//				
//                iceCream.gotLick = true;
				bIsLicking == true;
                lickSound.play();
            }
        }
        
    } else if (tongue.isMovingDown){
		bIsLicking = false;
    }
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	bg.draw();
	
	// ICE CREAM
	// TODO: SEPARATE LEVEL LOGIC FROM DRAW FUNCTIONS
	// (IceCream should save state in update)
	
	if (iceCream.gameLevel == 0 ||iceCream.gameLevel == 2||iceCream.gameLevel == 4 || iceCream.gameLevel ==6){
		iceCream.flowing = true;
		iceCream.bDripDeath = false;
	}
	if (iceCream.gameLevel >= 8){
		iceCream.gameLevel = 0;
	}
	
	
	if (iceCream.gameLevel == 0){
		if (iceCream.flowing){
			iceCream.flow();
		}
	}
	if (iceCream.gameLevel ==1){
		//stationary and no toppings
		iceCream.level1();
	} else if (iceCream.gameLevel ==2){
		//ice cream flow anmation
		if (iceCream.flowing){
			iceCream.flow();
		}
	} else if (iceCream.gameLevel == 3){
		//moving and sprinkles
		iceCream.level2();
	} else if (iceCream.gameLevel == 4){
		//ice cream flow animation
		if (iceCream.flowing){
			iceCream.flow();
		}
	} else if (iceCream.gameLevel == 5){
		//choco flow animation
		iceCream.level3();
	} else if (iceCream.gameLevel == 6){
		//moving and choco
		iceCream.level4();
	} else if (iceCream.gameLevel ==7){
		iceCream.win();
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
		ofSetColor(255,106,159);
		ofDrawCircle(tongue.pos, 7);
		ofSetColor(255);
	}
	
	// TODO:  level display on screen
	cout << "game level: " <<iceCream.gameLevel<<endl;
	
	if (bDrawGui){
		gui.draw();
	}
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
            
        case ('1'):
            icLevelNum = 0;
            break;
        case ('2'):
            icLevelNum = 1;
            break;
        case ('3'):
            icLevelNum = 2;
            break;
        case ('4'):
            icLevelNum = 3;
            break;
            
        case (OF_KEY_UP):
            cout << "lvlX[" << icLevelNum << "] is " << ++(iceCream.lvlX[icLevelNum]) << endl;
            break;
        
        case (OF_KEY_DOWN):
            cout << "lvlX[" << icLevelNum << "] is " << --(iceCream.lvlX[icLevelNum]) << endl;
            break;
			
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
