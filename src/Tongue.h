//
//  Tongue.h
//  tongueGame_tyler
//
//  Created by Tyler on 12/9/15.
//
//

#pragma once
#include "ofMain.h"
#include "ofxKinect.h"

class Tongue{
public:
    
    Tongue();
    void update(float _posX, float _posY);
    void draw();
	
	void setLerp(ofVec2f lerpPct) {
		lerp = lerpPct;
	}
	
	bool isLicking() {
		return (pos.y < prevPos.y) && (pos.x > prevPos.x - 10) && (pos.x < prevPos.x + 10);
	}
	bool isMovingDown() {
		return (pos.y > prevPos.y + 15.f);	// TODO: make adjustable?
	}
    
    ofVec2f pos;
    ofVec2f prevPos; //last frame

protected:
    
    bool bLicking = false;
    bool bMovingDown = false;
	
	float lastHighestYPos = 0;
	ofVec2f lerp;
    
    
};
