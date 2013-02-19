#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "Clone.h"
#include "ofxFaceTracker.h"
#include "ofxFaceTrackerThreaded.h"
#include "ofxXmlSettings.h"
#include "ofSoundPlayer.h"
 
class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void dragEvent(ofDragInfo dragInfo);
	void loadFace(string face);
	
	void keyPressed(int key);

	ofxFaceTrackerThreaded camTracker;
	ofVideoGrabber cam;
	
	ofxFaceTracker srcTracker;
	ofImage src;
	vector<ofVec2f> srcPoints;
	
	bool cloneReady;
	Clone clone;
	ofFbo srcFbo, maskFbo;

	ofDirectory faces; //directory of faces to map onto live image
	ofDirectory captured; //diretory of images saved (clean user image)
	ofDirectory printed; //directory of mapped-face that selected to print/save
	
	int currentFace;
	
	int imageSequence;
	bool videoCaptured;
	bool imageCaptured;
	int wait;
	
	string  destinationFolder;
	string city;
    int timeZone;
	
	ofSoundPlayer sounds;
};
