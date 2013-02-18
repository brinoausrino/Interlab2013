#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxSimpleSerial.h"

#define HOST "localhost"
#define PORT 12345



//--------------------------------------------------------
class testApp : public ofBaseApp {

	public:

		void setup();
		void update();
		void draw();

		void onNewMessage(string & message);

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
		ofxOscSender sender;
		ofxSimpleSerial serial;

		
		struct MessageFormat {
			string addressName;
			vector<int> argTypes;
		};

		vector<MessageFormat> messageFormats;

		//static const int TypeString = 0;
		//static const int TypeInt = 1;
		//static const int TypeFloat = 2;
    
        enum types1 {TYPESTRING = 0, TYPEINT = 1, TYPEFLOAT = 2};
    
        
};

