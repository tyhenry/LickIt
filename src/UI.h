//
//  UI.h
//  tongueGame
//
//  Created by Miri Park on 12/8/15.
//
//

#include "ofMain.h"

#define N_CLOSE_IMGS 16

class UI {
public:
    UI();
    void draw();
    void update();
	void beginClose(float fps = 8.f) {
		closeFrameDelay = 1.f / ofClamp(fps, 0.1f, 60.f);
		tLastCloseFrame = ofGetElapsedTimef();
		closeIndex = 0;
	}
	void setMouthOpen(bool open) {
		if (open){
			closeIndex = -1;
		} else {
			closeIndex = N_CLOSE_IMGS - 1;
		}
		bMouthClosed = !open;
	}
    
    ofImage lowerTeeth;
    ofImage upperTeeth;
	
	ofImage closeAnimation[N_CLOSE_IMGS];
	int closeIndex = -1;
	float tLastCloseFrame;
	float closeFrameDelay;
	bool bMouthClosed = false;
};
