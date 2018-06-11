//
//  Tongue.cpp
//  tongueGame_tyler
//
//  Created by Tyler on 12/9/15.
//
//

#include "Tongue.h"

Tongue::Tongue(){
	
	pos = ofVec2f(ofGetWidth()/2, ofGetHeight()/2);
	prevPos = pos;
	lastHighestYPos = pos.y;
	
	lerp.x = 0.5f;
	lerp.y = 0.3f;
	
}

void Tongue::update(float _posX, float _posY){
	
	prevPos = pos;
	
	pos.x = ofLerp(pos.x,_posX,lerp.x);
	pos.y = ofLerp(pos.y,_posY,lerp.y);
	
	if (pos.y < lastHighestYPos){
		lastHighestYPos = pos.y;
	}
	
	
	//CHECK FOR LICKING
	// licking is any movement from low->high while not moving much left-right
	
//	if (pos.y < prevPos.y){
//		if ((pos.x > prevPos.x - 10) && (pos.x < prevPos.x + 10)){
//			bLicking = true;
//			
//		} else {
//			bLicking = false;
//		}
//		bMovingDown = false;
//		
//	} else {
//		bLicking = false;
//		
//		if (pos.y > prevPos.y + 15){
//			bMovingDown = true;
//		}
//	}
	
}

void Tongue::draw(){
	
	
}
