#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

	current_msg_string = 0;

	ofBackground(30, 30, 130);
    
    
    
    ofSoundPlayer sound1;
    sound1.loadSound("sounds/sound1.mp3");
    sound1.setVolume(0.75f);
    sounds.push_back(sound1);
    
    ofSoundPlayer sound2;
    sound2.loadSound("sounds/sound2.mp3");
    sound2.setVolume(0.75f);
    sounds.push_back(sound2);
    
    ofSoundPlayer sound3;
    sound3.loadSound("sounds/sound3.mp3");
    sound3.setVolume(0.75f);
    sounds.push_back(sound3);
    
    ofSoundPlayer sound4;
    sound4.loadSound("sounds/sound4.mp3");
    sound4.setVolume(0.75f);
    sounds.push_back(sound4);
    
    current_sound = 0;
    is_silent = false;
    sounds[current_sound].play();
}

//--------------------------------------------------------------
void testApp::update(){
    
    
    //play sounds
    if(!sounds[current_sound].getIsPlaying()){
        
        
        if(current_sound + 1 < sounds.size()){
            current_sound++;
        }
        else{
            current_sound = 0;
        }
        
        
        sounds[current_sound].play();
    }
    
    
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
        
        
		
		if(
           m.getAddress() == "b" ||
           m.getAddress() == "/b"
        ){
			if(!is_silent){
                
                sounds[current_sound].setVolume(0.0f);
                
                cout << "SILENCE!!!!!!!" << endl;
            }
            else{
                sounds[current_sound].setVolume(0.75f);
                
                cout << "PARTY!!!!!!!" << endl;
            }
            is_silent = !is_silent;
            
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
