#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "Clone.h"
#include "ofxFaceTracker.h"
#include "ofxFaceTrackerThreaded.h"
#include "ofxXmlSettings.h"
#include "ofSoundPlayer.h"

enum BOOTH_SATE {
	WELCOME = 0,
	SAVE_FACE,	
	IN_USE,
	GOODBYE
};
 
class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void dragEvent(ofDragInfo dragInfo);
	void loadFace(string face);
	
	void showWelcomeScreen();
	void showGoodbyeScreen();
	
	void keyPressed(int key);
	void mousePressed(int x, int y, int button);
	
	void shuffleFace();
	void printPicture();
	void saveNewFace();
	void errorMessage();
	string getTimeStamp();
	
	void resetTimeout();
	void timeout();

	ofxFaceTrackerThreaded camTracker;
	ofVideoGrabber cam;
	
	ofxFaceTracker srcTracker;
	ofImage src;
	vector<ofVec2f> srcPoints;
	
	bool cloneReady;
	Clone clone;
	ofFbo srcFbo, maskFbo;

	ofDirectory faces; //directory of faces to map onto live image
					   //for Cairo it's Faces_Dresden
					   //for Dresden it's Faces_Cairo
	ofDirectory captured; //diretory of images saved (clean user image)
						  //for Cairo it's Faces_Cairo	
						  //for Dresden it's Faces_Dresden
	ofDirectory printed; //directory of mapped-face that selected to print/save (common folder for both)
	
	int currentFace;
	
	int imageSequence;
	bool videoCaptured;
	bool imageCaptured;
	int wait;
	
	bool countTimeout;
	
	float _time;
	
	BOOTH_SATE currentState;
	
	ofTrueTypeFont	englishInstructions;
	ofTrueTypeFont	arabicInstructions;

	
	string  destinationFolder;
	string city;
	string printerName;
    int timeZone;
	
	ofSoundPlayer sounds;
};
