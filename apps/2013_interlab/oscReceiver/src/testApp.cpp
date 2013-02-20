#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	current_msg_string = 0;
	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";

	ofBackground(30, 30, 130);
    
    serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
	
	// this should be set to whatever com port your serial device is connected to.
	// (ie, COM4 on a pc, /dev/tty.... on linux, /dev/tty... on a mac)
	// arduino users check in arduino app....
	//serial.setup(0, 9600); //open the first device
	//serial.setup("COM4"); // windows example
	//serial.setup("/dev/tty.usbserial-A4001JEC",9600); // mac osx example
	serial.setup("/dev/tty.usbserial-A9007OsB",9600); // yasmin's mac osx 
   // serial.setup("/dev/tty.usbmodem411", 9600);
	//serial.setup("/dev/ttyUSB0", 9600); //linux example
    
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
}

//--------------------------------------------------------------
void testApp::update(){

	// hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msg_strings[i] = "";
		}
	}

	// check for waiting messages
	while(receiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
        for (int i=0; i<messageFormats.size(); ++i) {
            if(m.getAddress() == messageFormats[i].addressName){
                
                //write address
                serial.writeByte(m.getAddress()[0]);
                
                //get and write arguments
                for(int j=0; j<messageFormats[i].argTypes.size(); ++j){
                    int type = messageFormats[i].argTypes[j];
                    
                    switch(type){
                        case TYPESTRING:
                            
                            serial.writeByte(m.getArgAsString(j)[0]);
                            
                            cout << "got " << m.getAddress()[0] << " write param " << j << " string " << m.getArgAsString(j)[0] << endl;
                            break;
                        case TYPEINT:
                            
                            serial.writeByte(m.getArgAsInt32(j));
                            
                            cout << "got " << m.getAddress()[0] << " write param " << j << " int " << m.getArgAsInt32(j) << endl;
                            break;
                        case TYPEFLOAT:
                            
                            serial.writeByte(m.getArgAsFloat(j));
                            
                            cout << "got " << m.getAddress()[0] << " write param " << j << " float " << m.getArgAsFloat(j) << endl;
                            break;
                    }
                }
            }
        }
        
        
        
        
        
		// check for mouse moved message
		if(m.getAddress() == "/mouse/position"){
			// both the arguments are int32's
			mouseX = m.getArgAsInt32(0);
			mouseY = m.getArgAsInt32(1);
		}
		// check for mouse button message
		else if(m.getAddress() == "/mouse/button"){
			// the single argument is a string
			mouseButtonState = m.getArgAsString(0);
		}
		else{
			// unrecognized message: display on the bottom of the screen
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for(int i = 0; i < m.getNumArgs(); i++){
				// get the argument type
				msg_string += m.getArgTypeName(i);
				msg_string += ":";
				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msg_string += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msg_string += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msg_string += m.getArgAsString(i);
				}
				else{
					msg_string += "unknown";
				}
			}
			// add to the list of strings to display
			msg_strings[current_msg_string] = msg_string;
			timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
			current_msg_string = (current_msg_string + 1) % NUM_MSG_STRINGS;
			// clear the next line
			msg_strings[current_msg_string] = "";
		}

	}
}


//--------------------------------------------------------------
void testApp::draw(){

	string buf;
	buf = "listening for osc messages on port" + ofToString(PORT);
	ofDrawBitmapString(buf, 10, 20);

	// draw mouse state
	buf = "mouse: " + ofToString(mouseX, 4) +  " " + ofToString(mouseY, 4);
	ofDrawBitmapString(buf, 430, 20);
	ofDrawBitmapString(mouseButtonState, 580, 20);

	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		ofDrawBitmapString(msg_strings[i], 10, 40 + 15 * i);
	}



}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

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
