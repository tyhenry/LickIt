
//
//  UI.cpp
//  tongueGame
//
//  Created by Miri Park on 12/8/15.
//
//

#include "UI.h"

UI::UI(){
    lowerTeeth.load("UI/lowerTeeth.png");
    upperTeeth.load("UI/upperTeeth.png");
	teeth.load("UI/teeth.png");
	
	// close animation
	for (int i=0; i<N_CLOSE_IMGS; i++){
		string imgNum = ofToString(i, 2, '0');
		closeAnimation[i].load("UI/teeth_close_" + imgNum  + ".png");
	}
	
	closeFrameDelay = 1.f / 8.f;	// 8 fps
}

void UI::update(){
	
	if (!bMouthClosed && closeIndex >= 0){
		float t = ofGetElapsedTimef();
		if (t - closeFrameDelay >= tLastCloseFrame){
			++closeIndex;
			tLastCloseFrame = t;
			if (closeIndex >= N_CLOSE_IMGS){
				closeIndex = N_CLOSE_IMGS - 1;
				bMouthClosed = true;
                ofLogNotice("UI") << "mouth closed";
			}
		}
	}
}

void UI::draw(){
	
	// calc scaling and pos
	ofVec2f wS = ofVec2f(ofGetWidth(), ofGetHeight());
	float scaleFactor = wS.x/upperTeeth.getWidth();
	
	// draw teeth
	if (closeIndex < 0 || closeIndex >= N_CLOSE_IMGS){
		teeth.draw(0,0, wS.x, teeth.getHeight() * scaleFactor);
	}
	else {
		// draw mouth closing
		if (closeIndex >= 0 && closeIndex < N_CLOSE_IMGS){
			auto& img = closeAnimation[closeIndex];
			img.draw(0,0, wS.x,img.getHeight() * scaleFactor);
		}
	}
}
			

/*
 void UI::loseDraw(int index){
 
 if (index >= MOUTHCLOSEIMAGES) {
 //reset
 index = 0;
 }
 
 closeAnimation[index].draw(0,0, ofGetWidth(),ofGetHeight());
 
 }
 */
