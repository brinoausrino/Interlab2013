#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(40, 100, 40);

	// open an outgoing connection to HOST:PORT
	sender.setup(HOST, PORT);
    soundSender.setup(SOUNDHOST, SOUNDPORT);
	localSender.setup(LOCALHOST, LOCALPORT);

	//serial reader
	serial.setup("COM4", 9600);
    //serial.setup("/dev/tty.usbserial-A600K2BH", 9600);
    serial.startContinuesRead();
    ofAddListener(serial.NEW_MESSAGE,this,&testApp::onNewMessage);
	serial.sendRequest();

	
    vector<int> argTypes1;
	argTypes1.push_back(TYPEINT);
	struct MessageFormat mf1 = {"g", argTypes1};
	messageFormats.push_back(mf1);
    
    vector<int> argTypes2;
	argTypes2.push_back(TYPEINT);
	struct MessageFormat mf2 = {"h", argTypes2};
	messageFormats.push_back(mf2);
    
    vector<int> argTypes3;
	argTypes3.push_back(TYPEINT);
	struct MessageFormat mf3 = {"i", argTypes3};
	messageFormats.push_back(mf3);
    
    vector<int> argTypes4;
	argTypes4.push_back(TYPEINT);
	struct MessageFormat mf4 = {"j", argTypes4};
	messageFormats.push_back(mf4);

	vector<int> argTypes5;
	argTypes5.push_back(TYPEINT);
	struct MessageFormat mf5 = {"k", argTypes5};
	messageFormats.push_back(mf5);
    
    vector<int> argTypes6;
	argTypes6.push_back(TYPEINT);
	struct MessageFormat mf6 = {"l", argTypes6};
	messageFormats.push_back(mf6);

	vector<int> argTypes7;
	argTypes7.push_back(TYPEINT);
	struct MessageFormat mf7 = {"c", argTypes7};
	messageFormats.push_back(mf7);
    
    vector<int> argTypes8;
	argTypes8.push_back(TYPEINT);
	struct MessageFormat mf8 = {"e", argTypes8};
	messageFormats.push_back(mf8);

	vector<int> argTypes9;
	argTypes9.push_back(TYPEINT);
	struct MessageFormat mf9 = {"d", argTypes9};
	messageFormats.push_back(mf9);
    
    soundAddress = "/message/send";
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
    cout << message << endl;
    
    for (int i=0; i<messageFormats.size(); ++i) {
        
        //split message e.g. "a|1|2"
        vector<string> messageParts = ofSplitString(message, "|");
        
        if(messageParts.size()>1 && messageParts[0] == messageFormats[i].addressName){
            
            cout << "ok" << endl;
        
			//get address name
			string addressName = messageFormats[i].addressName;
            messageParts.erase(messageParts.begin());
			
            //create osc message
			ofxOscMessage m;
			m.setAddress(addressName);

			//get arguments
			for(int j=0; j<messageFormats[i].argTypes.size(); ++j){
				int type = messageFormats[i].argTypes[j];
				switch(type){
					case TYPESTRING:
						m.addStringArg(ofToString(messageParts[j]));
                        break;
					case TYPEINT:
						m.addIntArg(ofToInt(messageParts[j]));
                        break;
					case TYPEFLOAT:
						m.addFloatArg(ofToFloat(messageParts[j]));
                        break;
				}
			}
            
			sender.sendMessage(m);
            
            testApp::notifySound();

			//also send to local receiver
			string locaAddress;
			if(addressName == "g"){
				locaAddress = "a";
			}
			else if(addressName == "h"){
				locaAddress = "b";
			}
			else if(addressName == "i"){
				locaAddress = "c";
			}
			else if(addressName == "j"){
				locaAddress = "e";
			}
			else if(addressName == "l"){
				locaAddress = "d";
			}
			m.setAddress(locaAddress);
			localSender.sendMessage(m);
			
		}
    }
}

//--------------------------------------------------------------
void testApp::notifySound(){
	
    ofxOscMessage sm;
    sm.setAddress(soundAddress);
    sm.addIntArg(1);
    soundSender.sendMessage(sm);
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
	if(key == 'g' || key == 'G'){
		string message = "g|1";
		onNewMessage(message);
	}
	if(key == 'h' || key == 'H'){
		string message = "h|0";
		onNewMessage(message);
	}
	if(key == 'i' || key == 'I'){
		string message = "i|1";
		onNewMessage(message);
	}
    if(key == 'j' || key == 'J'){
		string message = "j|1";
		onNewMessage(message);
	}
    if(key == 'w' || key == 'W'){
		string message = "c|1";
		onNewMessage(message);
	}
    if(key == 's' || key == 'S'){
		string message = "c|0";
		onNewMessage(message);
	}
    if(key == 'e' || key == 'E'){
		string message = "d|1";
		onNewMessage(message);
	}
    if(key == 'd' || key == 'D'){
		string message = "d|0";
		onNewMessage(message);
	}
	if(key == 'r' || key == 'R'){
		string message = "l|1";
		onNewMessage(message);
	}
    if(key == 'f' || key == 'F'){
		string message = "l|0";
		onNewMessage(message);
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
	//ofxOscMessage m;
	//m.setAddress("/mouse/position");
	//m.addIntArg(x);
	//m.addIntArg(y);
	//sender.sendMessage(m);
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

