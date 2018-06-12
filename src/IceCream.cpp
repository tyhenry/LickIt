#include "IceCream.h"

IceCream::IceCream(){
    
    // LOAD IMAGES & ANIMATIONS
	
	loadAssets();
	
	// POSITIONING
	
	size.x = iceCreamAnimation[0].getWidth() * CONE_SCALE;
    size.y = iceCreamAnimation[0].getHeight() * CONE_SCALE;
	
    origPos.x = ofGetWidth() * 0.5f - size.x * 0.5f;
	origPos.y = ofGetHeight() - size.y;// * 1.1f;
	
	pos = origPos;
	
	vel = ofVec2f(0.f);	// stationary at start
	
	frameBounds = ofRectangle(0,0, ofGetWidth(), ofGetHeight());
	
	
	// COLLIDERS
	// positions relative to cone pos
	
	ofRectangle c0 = ofRectangle(150.f, 50.f, 150.f, 100.f);
	ofRectangle c1 = ofRectangle(101.f, 150.f, 250.f, 80.f);
	ofRectangle c2 = ofRectangle(57.f, 230.f, 350.f, 80.f);
	ofRectangle c3 = ofRectangle(38.f, 310.f, 400.f, 70.f);
	
	colliders.emplace_back(c0, true);
	colliders.emplace_back(c1, true);
	colliders.emplace_back(c2, true);
	colliders.emplace_back(c3, true);
	
	
	// AUDIO
    
    winSound.load("sounds/win.wav");
    winSound.setVolume(0.85f);
    winSound.setMultiPlay(false);
    
    loseSound.load("sounds/lose.wav");
    loseSound.setVolume(0.85f);
    loseSound.setMultiPlay(false);
	
	
	// SETUP
	
	bMelting = false;
	meltStartedTime = 0.f;
	meltDuration = -1.f;
	
	bDripDeath = false;
	
	lickState = 0;
	lickMax = 10;	// 10 licks to finish
	
	lickAnimIdx = 0;
	lickAnimLen = float(N_ICECREAM_IMAGES / float(lickMax));
		// # of frames to play per lick
	lickAnimEnd = lickAnimIdx + lickAnimLen;
	
	bIsFilled = false;
	bHasChocolate = false;
	bHasSprinkles = false;
	sprinkles.clear();
	
	frameBounds = ofRectangle(0,ofGetHeight()*.33,ofGetWidth(), ofGetHeight()*.66);
	
	
}

// ---------------------------------------------------------
void IceCream::update(){
	
	if (!bPaused){
	
		if (isReady()){
			// move
			pos += vel;
			
			// bounce off bounds
			if ( pos.x > frameBounds.getRight() - size.x ) {
				pos.x = frameBounds.getRight() - size.x;
				vel.x *= -1;
			} else if ( pos.x < frameBounds.getLeft() ) {
				pos.x = frameBounds.getLeft();
				vel.x *= -1;
			}
			if ( pos.y > frameBounds.getBottom() - size.y ) {
				pos.y = frameBounds.getBottom() - size.y;
				vel.y *= -1;
			} else if ( pos.y < frameBounds.getTop() ) {
				pos.y = frameBounds.getTop();
				vel.y *= -1;
			}
		}
		
		// increment melt
		// check for drip death / game over
		if (bMelting){
			if (!bDripDeath){
				float t = ofGetElapsedTimef();
				meltIdx =
				ofMap(t, meltStartedTime, meltStartedTime+meltDuration, 0, N_MELT_IMAGES, true);
				if (meltIdx >= N_MELT_IMAGES){
					meltIdx = N_MELT_IMAGES-1;
					bDripDeath = true;	// checked in ofApp for game over
					ofLogNotice("IceCream") << "ice cream melted!" << endl;
				}
			}
		}
		
		// animate licks
		
		if (lickAnimIdx < lickAnimEnd) ++lickAnimIdx;

		// animate sequences
		
		float t = ofGetElapsedTimef();
		if (t > lastAnimFrameT + animFrameDur){
			lastAnimFrameT = t;
			
			if (!isFilled() && refillIdx >= 0){
				++refillIdx;
				if (refillIdx >= N_REFILL_IMAGES){
					refillIdx = -1;	// reset
					bIsFilled = true;
				}
			}
			else if (isFilled() && bHasChocolate && !bChocoPoured){
				++chocoPourIdx;
				if (chocoPourIdx >= N_CHOCOPOUR_IMAGES){
					bChocoPoured = true;
					chocoPourIdx = -1;	// reset
				}
			}
		}
	}
	
//	switch (lickState){
//		case 0:
//			break;
//		case 1:
//			colliders[0].second = false;		// remove collider
//			if (lickIndex < 29) ++lickIndex;	// animate
//			break;
//		case 2:
//			if (lickIndex < 38) ++lickIndex;
//			break;
//		case 3:
//			colliders[1].second = false;		// - 2nd collider
//			if (lickIndex < 43) ++lickIndex;
//			break;
//		case 4:
//			if (lickIndex < 47) ++lickIndex;
//			break;
//		case 5:
//			if (lickIndex < 51) ++lickIndex;
//			break;
//		case 6:
//			colliders[2].second = false;		// - 3rd collider
//			if (lickIndex < 56) ++lickIndex;
//			break;
//		case 7:
//			if (lickIndex < 60) ++lickIndex;
//			break;
//		case 8:
//			if (lickIndex < 64) ++lickIndex;
//			break;
//		case 9:
//			if (lickIndex < 69) ++lickIndex;
//			break;
//		case 10:
//			if (lickIndex < 74) ++lickIndex;
//			gameLevel++;
//			lickState = 0;
//			reset();
//			break;
//
//	}

	
}

bool IceCream::checkCollision(ofVec2f p){
	
	for (auto& collider : colliders){
		if (collider.second){	// active
			auto lclP = p - pos;
			if(collider.first.inside(lclP)){
				return true;
			}
		}
	}
	return false;
}

void IceCream::lick(){
	
	++lickState;
	
	// update colliders / game state
	
	// make variable # colliders?
	if (lickState == 1){
		colliders[0].second = false;
		if (bHasChocolate) chocoLickIdx = 1;
	}
	else if (lickState == 3){
		colliders[1].second = false;
		if (bHasChocolate) chocoLickIdx == 2;
	}
	else if (lickState == 6){
		colliders[2].second = false;
		if (bHasChocolate) chocoLickIdx = -1;
	}
	else if (lickState >= lickMax){
		// win game flag
		ofLogNotice("IceCream") << "player ate the cone!";
		bAteCone = true;
	}
	
	// remove sprinkles based on lickState
	if (bHasSprinkles){
		float lickPct 	= lickState / float(lickMax);
		float yMin		= ofMap(lickPct, 0.f, 1.f, colliders[0].first.getTop(), colliders.back().first.getBottom(), true);
		vector<pair<int, ofVec2f>> sprinklesTmp;
		for (auto& sprinkle : sprinkles){
			if (sprinkle.second.y >= yMin) sprinklesTmp.push_back(sprinkle);
		}
		sprinkles = sprinklesTmp;
	}
	
	
	// trigger lick state animation
	lickAnimEnd = ofMap(lickState, 0, lickMax, 0, N_ICECREAM_IMAGES-1, true);
	
}

void IceCream::refill(){
	reset();
	refillIdx = 0;
}

void IceCream::reset(){
	
	lickState = 0;
	lickMax = 10;	// 10 licks to finish
	
	lickAnimIdx = 0;
	lickAnimEnd = ofMap(lickState, 0, lickMax, 0, N_ICECREAM_IMAGES-1, true);
	
	pos = origPos;
	vel = ofVec2f(0.f);
	
	bDripDeath = false;
	bAteCone = false;
	
	// melting
	resetMelt();
	
	// toppings
	bHasChocolate = false;
	chocoLickIdx = -1;
	
	bHasSprinkles = false;
	sprinkles.clear();
	
	// reset animations
	refillIdx = -1;
	chocoPourIdx = -1;
	
	bIsFilled = false;
	bChocoPoured = false;
	
	iceCreamTint = ofColor(255);
	
	bPaused = false;
	
}

void IceCream::resetMelt(){
	bMelting = false;
	meltIdx = 0;
	meltStartedTime = 0.f;
	meltDuration = -1.f;
}

void IceCream::startMelt(float timerSecs){
	bMelting = true;
	meltStartedTime = ofGetElapsedTimef();
	meltDuration = timerSecs;
	ofLogNotice("IceCream") << "started melting at [" << ofToString(meltStartedTime, 2) << "] secs for dur: " << meltDuration << "s";
}

void IceCream::addSprinkles(int num){
	
	for (int i=0; i<num; ++i){
		
		// random sprite index
		int idx = int(ofRandom(0, N_SPRINKLE_SPRITES)) % N_SPRINKLE_SPRITES;
		
		// place within a collider x bounds
		ofVec2f p;
		int c = int(ofMap(i, 0, num, 0, colliders.size(), true));
		auto& rect = colliders[c].first;
		p.x = ofRandom(rect.getLeft(), rect.getRight());
		// place within all colliders y bounds
		p.y = ofMap(i, 0, num, colliders[0].first.getTop(), colliders.back().first.getBottom(), true);
		sprinkles.emplace_back(idx, p);
	}
	if (sprinkles.size() > 0) { bHasSprinkles = true; }
}

void IceCream::draw(){
	
	// empty / refilling
	if (!isFilled()){
		coneImg.draw(pos, size.x, size.y);	// empty cone
		ofPushStyle();
		ofSetColor(iceCreamTint);
		if (refillIdx >= 0 && refillIdx < N_REFILL_IMAGES){
			refillAnimation[refillIdx].draw(pos, size.x, size.y);
		}
		ofPopStyle();
	}
	
	// ice cream
	else {
		ofPushStyle();
		ofSetColor(iceCreamTint);
		iceCreamAnimation[lickAnimIdx].draw(pos, size.x, size.y);
		ofPopStyle();
		coneFrontImg.draw(pos, size.x, size.y);
	
		if (isReady()) {
			
			// draw toppings
			if (bHasChocolate){
				//if (chocoLickIdx >= 0 && chocoLickIdx < N_CHOCOLICK_IMAGES)
					chocoLickAnimation[0].draw(pos, size.x, size.y);
			}
			if (bHasSprinkles){
				drawSprinkles();
			}
		
			if (bMelting){
				// draw melt animation
				if (meltIdx >=0 && meltIdx < N_MELT_IMAGES) {
					meltAnimation[meltIdx].draw(pos, size.x, size.y);
				}
			}
		} else {
			if (bHasChocolate) {
				chocoPourAnimation[chocoPourIdx].draw(pos, size.x, size.y);
			}
		}
	}
	
}


void IceCream::drawSprinkles(){
	
	for (auto& sprinkle : sprinkles){
		int i 	= sprinkle.first;
		auto p	= pos + sprinkle.second;	// local -> global
		
		sprinkleSprites[i].draw(p);
	}
}


void IceCream::drawColliders(){	// for debug
	
	ofPushStyle();
	ofSetColor(0,200,100);
	ofNoFill();
	ofDrawRectangle(pos, size.x, size.y);	// whole cone
	ofPushMatrix();
	ofTranslate(pos);		// colliders relative to cone
	ofSetColor(255);
	for (auto& col : colliders){
		ofDrawRectangle(col.first);
	}
	ofPopMatrix();
	ofPopStyle();
}

void IceCream::loadAssets(){
	
	// ice cream cone base
	for(int i =0; i< N_ICECREAM_IMAGES; i++){
		string file = "ice_cream_cone/ice_cream_cone_" + ofToString(i, 2, '0') + ".png";
		iceCreamAnimation[i].load(file);
	}
	
	// melt drip
	for(int i =0; i< N_MELT_IMAGES; i++){
		string file = "ice_cream_melt/ice_cream_melt_" + ofToString(i, 2, '0') + ".png";
		meltAnimation[i].load(file);
	}
	
	// soft serve refill
	for(int i =0; i< N_REFILL_IMAGES; i++){
		string file = "ice_cream_refill/ice_cream_refill_" + ofToString(i, 2, '0') + ".png";
		refillAnimation[i].load(file);
	}
	
	// chocolate
	// pour
	for(int i =0; i< N_CHOCOPOUR_IMAGES; i++){
		string file = "toppings/choco_pour_" + ofToString(i, 2, '0') + ".png";
		chocoPourAnimation[i].load(file);
	}
	// lick
	for (int i = 0; i < N_CHOCOLICK_IMAGES; i++){
		string file = "toppings/choco_lick_" + ofToString(i, 2, '0') + ".png";
		chocoLickAnimation[i].load(file);
	}
	
	// sprinkles
	for (int i = 0; i < N_SPRINKLE_SPRITES; i++){
		sprinkleSprites[i].load("toppings/sprinkle_" + ofToString(i, 2, '0') + ".png");
		sprinkleSprites[i].resize(sprinkleSprites[i].getWidth() * SPRINKLE_SCALE, sprinkleSprites[i].getHeight() * SPRINKLE_SCALE);
	}
	
	// level clear / win anim
//	for(int i =0; i < N_WIN_IMAGES; i++){
//		winAnimation[i].load("special_fx/wink_" + ofToString(i, 2, '0') + ".png");
//	}
	
	// empty cone image
	coneImg.load("cone.png");
	// cone front only
	coneFrontImg.load("cone_front.png");
}



//void IceCream::level1(){
////    moveIncrement = 2;
//    meltRate = 1000;
//    draw();
//}
//void IceCream::level2(){
////    moveIncrement = 5;
//    meltRate = 15;
//    move();
//    draw();
//    drawSprinkles();
//}
//void IceCream::level3(){
//    drawChoco();
//}
//void IceCream::level4(){
//
//    meltRate = 10;
//    move();
//
//    draw();
//    cout<<"lick state: "<<lickState<<endl;
//    if (lickState ==0){
//        chocoLickAnimation[0].draw(pos, size.x, size.y);
//    }
//    if (lickState == 1){
//        chocoLickAnimation[1].draw(pos.x, pos.y + 10, size.x, size.y);
//    }
//    if (lickState == 2){
//        chocoLickAnimation[2].draw(pos.x, pos.y + 30, size.x, size.y);
//    }
////    if (lickState ==10){
////        win();
////    }
//}

//void IceCream::level8(){
//    if (ofGetFrameNum()% 5== 0 && winkIndex < WINKIMAGES -1){
//        winkIndex += 1;
//    }
//    winkAnimation[winkIndex].draw(pos.x, pos.y);
//    if(winkIndex == WINKIMAGES -1){
//        gameLevel += 1;
//    }
//}

//void IceCream::win(){
//    if(ofGetFrameNum() % flowSpeed == 0 && winkIndex < N_WIN_IMAGES - 1){
//        winkIndex = winkIndex + 1;
//    }
//    if(winkIndex == 6){
//    }
////    if(winkIndex == WINKIMAGES - 1){
////        winkIndex = 0;
////    }
//    cout<<"wink index: " << winkIndex<<endl;
//	winAnimation[winkIndex].draw(pos, size.x, size.y);
//    if (winkIndex >= N_WIN_IMAGES -1){
//        resetWholeGame();
//    }
//}

//void IceCream::brainFreeze(){
//    if (gotLick){
//        currentLickFrame = ofGetFrameNum();
//        //cout<<"licked frame: " << savedLickFrame<<endl;
//    }
//    if (currentLickFrame - savedLickFrame < speedLimit){
//        brainFrozen = true;
//    } else {
//        brainFrozen = false;
//    }
//    savedLickFrame = currentLickFrame;
//}



