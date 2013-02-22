#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

// listen on port 12345
#define PORT 12345
#define NUM_MSG_STRINGS 20

#define SOUNDHOST "192.168.0.107"
#define SOUNDPORT 12345

#define BIRDCAGEHOST "192.168.0.110"
#define BIRDCAGEPORT 12345

class testApp : public ofBaseApp {
	public:

		void setup();
		void update();
		void draw();
    
        void notifySound();
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofTrueTypeFont font;
		ofxOscReceiver receiver;

		int current_msg_string;
		string msg_strings[NUM_MSG_STRINGS];
		float timers[NUM_MSG_STRINGS];

		int mouseX, mouseY;
		string mouseButtonState;
    
        ofSerial	serial;
    
        ofxOscSender soundSender;
        string soundAddress;
    
        ofxOscSender birdcageSender;
        
    
        struct MessageFormat {
            string addressName;
            vector<int> argTypes;
        };
        vector<MessageFormat> messageFormats;
        enum types {TYPESTRING = 0, TYPEINT = 1, TYPEFLOAT = 2};
};
