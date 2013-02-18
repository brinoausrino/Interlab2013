#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(40, 100, 40);

	// open an outgoing connection to HOST:PORT
	sender.setup(HOST, PORT);

	//serial reader
	//serial.setup("COM8", 9600);
    //serial.startContinuesRead();
    //ofAddListener(serial.NEW_MESSAGE,this,&testApp::onNewMessage);
	//serial.sendRequest();

	
	vector<int> argTypes1;
	argTypes1.push_back(testApp::TypeInt);
	struct MessageFormat mf1 = {"/test/eins", argTypes1};
	messageFormats.push_back(mf1);

	vector<int> argTypes2;
	argTypes2.push_back(testApp::TypeString);
	struct MessageFormat mf2 = {"/test/zwei", argTypes2};
	messageFormats.push_back(mf2);

	vector<int> argTypes3;
	argTypes3.push_back(testApp::TypeInt);
	argTypes3.push_back(testApp::TypeString);
	struct MessageFormat mf3 = {"/test/drei", argTypes3};
	messageFormats.push_back(mf3);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	// display instructions
	string buf;
	buf = "sending osc messages to" + string(HOST) + ofToString(PORT);
	ofDrawBitmapString(buf, 10, 20);
	ofDrawBitmapString("move the mouse to send osc message [/mouse/position <x> <y>]", 10, 50);
	ofDrawBitmapString("click to send osc message [/mouse/button <button> <\"up\"|\"down\">]", 10, 65);
	ofDrawBitmapString("press A to send osc message [/test 1 3.5 hello <time>]", 10, 80);
}

//--------------------------------------------------------------
void testApp::onNewMessage(string & message)
{
   
	for (int i=0; i<messageFormats.size(); ++i) {
		unsigned found = message.find(messageFormats[i].addressName);
		if (found!=std::string::npos){

			//get address name
			string addressName = messageFormats[i].addressName;
			//split off address name
			message.erase(0, addressName.length());
			//get arguments
			vector<string> args = ofSplitString(message, "|");

			ofxOscMessage m;
			m.setAddress(addressName);

			
			for(int j=0; j<messageFormats[i].argTypes.size(); ++j){
				

				int type = messageFormats[i].argTypes[j];
				switch(type){
					case testApp::TypeString:
						m.addStringArg(ofToString(args[j]));
					case testApp::TypeInt:
						m.addIntArg(ofToInt(args[j]));
					case testApp::TypeFloat:
						m.addFloatArg(ofToFloat(args[j]));
				}
			}
			sender.sendMessage(m);
		}
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'a' || key == 'A'){
		ofxOscMessage m;
		m.setAddress("/test");
		m.addIntArg(1);
		m.addFloatArg(3.5f);
		m.addStringArg("hello");
		m.addFloatArg(ofGetElapsedTimef());
		sender.sendMessage(m);
	}
	if(key == 's' || key == 'S'){
		string message = "/test/eins9";
		onNewMessage(message);	
	}
	if(key == 'd' || key == 'D'){
		string message = "/test/zweihallo";
		onNewMessage(message);	
	}
	if(key == 'f' || key == 'F'){
		string message = "/test/drei12|zwölf";
		onNewMessage(message);	
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
	ofxOscMessage m;
	m.setAddress("/mouse/position");
	m.addIntArg(x);
	m.addIntArg(y);
	sender.sendMessage(m);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	ofxOscMessage m;
	m.setAddress("/mouse/button");
	m.addStringArg("down");
	sender.sendMessage(m);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	ofxOscMessage m;
	m.setAddress("/mouse/button");
	m.addStringArg("up");
	sender.sendMessage(m);

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}

