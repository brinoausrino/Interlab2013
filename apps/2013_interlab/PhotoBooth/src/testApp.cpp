#include "testApp.h"

using namespace ofxCv;

bool DEBUG = false;

void testApp::setup() {
#ifdef TARGET_OSX
	ofSetDataPathRoot("../../../data/");
#endif
	ofSetVerticalSync(true);
	cloneReady = false;
	cam.initGrabber(ofGetWindowWidth(), ofGetWindowHeight());
	int grabW = cam.width;
	int grabH = cam.height;
	printf("asked for 320 by 240 - actual size is %i by %i ", grabW, grabH);
	//cam.initGrabber(640, 480);
	clone.setup(cam.getWidth(), cam.getHeight());
	cam.setVerbose(true);
	ofFbo::Settings settings;
	settings.width = cam.getWidth();
	settings.height = cam.getHeight();
	maskFbo.allocate(settings);
	srcFbo.allocate(settings);
	camTracker.setup();
	srcTracker.setup();
	srcTracker.setIterations(25);
	srcTracker.setAttempts(4);
	
	currentState = WELCOME;
	
	countTimeout = false;
	
	_time = ofGetElapsedTimef();
	
	//camera capture sound, for fun
	sounds.loadSound("sounds/camera_snap.wav");
	arabic.loadImage("fonts/arabic2.png");
	
	//get city and directory settings
	ofxXmlSettings XML;
    if( XML.loadFile("mySettings.xml") ){
		cout << "mySettings.xml loaded!" << endl;
	}else{
		cout << "unable to load mySettings.xml check data/ folder" << endl;
	}
    
    destinationFolder = XML.getValue("FILES:PICTURE_FOLDER", "") + "Interlab/PhotoBooth/";
    city = ofToLower( XML.getValue("FILES:CITY", "") );
	string debugSetting = ofToLower( XML.getValue("FILES:DEBUG", "FALSE") );
	
	DEBUG = (debugSetting == "true" )? TRUE : FALSE;
	
    useSerial = false;
    string useSerialTemp = XML.getValue("FILES:USE_SERIAL","false");
    if (useSerialTemp == "true") {
        useSerial = true;
    }
    serialMessage = XML.getValue("FILES:SERIAL_MESSAGE","0");
    serialDevice = XML.getValue("FILES:SERIAL_DEVICE","com0");
    baudRate = XML.getValue("FILES:BAUD_RATE",9600);
	
	timeZone = XML.getValue("FILES:TIMEZONE", 0);
	printerName = XML.getValue("IMAGE_PROCESSING:PRINTER", "");
	
	//DEBUG
	printf("PRINTER is %s", printerName.c_str());	
	printf("Current City: %s", city.c_str());
	
	
	englishInstructions.loadFont("fonts/helvetica_med.ttf", 30, true, true);
	englishInstructions.setLineHeight(65.0f);
	englishInstructions.setLetterSpacing(1.035);
	
	arabicInstructions.loadFont("fonts/FrutigerLTArabic-65Bold.ttf", 30, true, true);
	arabicInstructions.setLineHeight(34.0f);
	arabicInstructions.setLetterSpacing(1.035);
	
#ifdef TARGET_OSX
	//change data path root to Dropbox
	ofSetDataPathRoot(destinationFolder );
#endif	
	
	faces.allowExt("jpg");
	faces.allowExt("png");
	printed.allowExt("jpg");
	printed.allowExt("png");
	captured.allowExt("png");
	captured.allowExt("png");
	
	if(city == "cairo")
	{
		//FACES directory in Cairo is Faces_Dresden
		faces.listDir("Faces_Dresden");
		
		//Diretory of images saved (clean user image) from Cairo for Dresden
		captured.listDir("Faces_Cairo"); 
	}
	else //DRESDEN
	{
		//FACES directory in Cairo is Faces_Dresden
		faces.listDir("Faces_Cairo"); 
		
		//Diretory of images saved (clean user image) from Dresden to Cairo
		captured.listDir("Faces_Dresden"); 			
	}
	
	//directory of mapped-face that selected to print/save
	printed.listDir("PrintedPhotos");
	
	
	wait = 0;
	currentFace = 0;
	imageSequence = 0;
	videoCaptured = imageCaptured = false;
	
	//debug
	printf("# of files %d",faces.numFiles());
	
	//serial setup
    if (useSerial) {
        isPressed = false;
        _serial.setup(serialDevice, baudRate);
        _serial.startContinuesRead();
        ofAddListener(_serial.NEW_MESSAGE,this,&testApp::onNewMessage);
        _serial.sendRequest();
    }
	
	//load latest face from other city
	if(faces.size()!=0){
		currentFace = faces.numFiles() - 1;
		loadFace(faces.getPath(currentFace));
	}
}

void testApp::update() {
	cam.update();
	

	//printf("time is %f \n", ofGetElapsedTimef() - _time);
	//printf("time float is %f\n", ofGetElapsedTimef());
	
	if(wait > 100)
	{
		wait = 0;
	}
	else wait++;
	
	if(countTimeout) 
	{
		int currTime = ofGetElapsedTimef() - _time;
		//printf("currTime is %d\n", currTime);
		if(currTime > 20)
			timeout();
	}
	
	if(cam.isFrameNew()) {
		camTracker.update(toCv(cam));
		
		cloneReady = camTracker.getFound();
		if(cloneReady) {
			ofMesh camMesh = camTracker.getImageMesh();
			camMesh.clearTexCoords();
			camMesh.addTexCoords(srcPoints);
			
			maskFbo.begin();
			ofClear(0, 255);
			camMesh.draw();
			maskFbo.end();
			
			srcFbo.begin();
			ofClear(0, 255);
			src.bind();
			camMesh.draw();
			src.unbind();
			srcFbo.end();
			
			clone.setStrength(16);
			clone.update(srcFbo.getTextureReference(), cam.getTextureReference(), maskFbo.getTextureReference());
		}
	}
}

void testApp::draw() {
	ofSetColor(255);
	
	if(src.getWidth() > 0 && cloneReady) {
		clone.draw(0, 0);
	} else {
		cam.draw(0, 0); //, ofGetWindowWidth(),ofGetWindowHeight());
	}
	
	string debugMessage;
	if(!camTracker.getFound()) {
		//if(DEBUG) drawHighlightString("camera face not found", 10, 10);
		debugMessage = "Can't detect face for face-mapping";
		
		if( currentState == IN_USE ) 
			resetTimeout(); //currentState = WELCOME;
	}
	if(src.getWidth() == 0) {
		//if(DEBUG) drawHighlightString("drag an image here", 10, 30);
		//debugMessage = "Can't detect face for face-mapping";
	} else if(!srcTracker.getFound()) {
		//if(DEBUG) drawHighlightString("image face not found", 10, 30);
		debugMessage = "Can't find source image to load";
	}
	
	if(imageCaptured) {
		//if(wait==100) imageCaptured = false;
		//if(DEBUG) drawHighlightString("Took a picture of you to send", 10, 60);
		debugMessage = "Took a stealth picture of you to send to ";
		debugMessage += (city == "cairo")? "Dresden" : "Cairo";
	}
	
	if(videoCaptured) {
		//if(wait==100) videoCaptured = false;
		//if(DEBUG) drawHighlightString("Took a picture of your NEW face", 10, 60);
		debugMessage = "Yay! Printed your photo...";
	}
	
	//printf("Current sate is %d", currentState);
	
	if(currentState == WELCOME)
	{
		if(wait==100 && camTracker.getFound()) { 
			saveNewFace();
			currentState = SAVE_FACE; 
		}
		showWelcomeScreen();
	}
	if(currentState == SAVE_FACE)
	{
		if(wait == 50)
			currentState = IN_USE;
		else 
			showWelcomeScreen();
	}
	if(currentState == GOODBYE)
	{
		showGoodbyeScreen();
		resetTimeout();
	}
	
	//draw gui
    if (DEBUG)
    {
        string t = "Current State: ";
        t+= getState();
        t+=("\n");
		t+= "Debug Message: ";
		t+= debugMessage;
		t+=("\n");
        t+= "seconds: ";
		t+= ofToString(ofGetElapsedTimef() - _time);
		t+=("\n");
        if (useSerial) {
            t+= "useSerial: true";
        }
        else  
            t+= "useSerial: false";
        t+=("\n");
        t+= "serial Device: ";
        t+= serialDevice;
        t+=("\n");
        t+= "baudRate: ";
        t+= ofToString(baudRate);
        t+=("\n");
        t+= "serialMessage: ";
        t+= serialMessage;
        ofSetColor(0, 0, 0);
        ofRect(0, ofGetWindowHeight() - 160, 700, 160);
        ofSetColor(255, 255, 255);
        ofDrawBitmapString(t, 20, ofGetWindowHeight() - 145);
    }
	
	
}

string testApp::getState()
{
	switch (currentState) {
		case WELCOME:
			return "Welcome";
		case SAVE_FACE:
			return "Saved Face Capture";
		case IN_USE:
			return "Using Photobooth";
		case GOODBYE:
			return "Goodbye. Waiting for Reset";
	}
}

void testApp::loadFace(string face){
	src.loadImage(face);
	if(src.getWidth() > 0) {
		srcTracker.update(toCv(src));
		srcPoints = srcTracker.getImagePoints();
	}
}

void testApp::dragEvent(ofDragInfo dragInfo) {
	loadFace(dragInfo.files[0]);
}

void testApp::timeout()
{
	countTimeout = false;
	currentState = WELCOME; 
}

void testApp::resetTimeout()
{
	if(countTimeout) return;
	_time = ofGetElapsedTimef();
	countTimeout = true;
}

string testApp::getTimeStamp()
{
	string timestamp = ofGetTimestampString();
    string t2 = timestamp.substr(11,2);
//	printf("timesteamp %s : t2 %s\n", timestamp.c_str(), t2.c_str());
//    int ttemp = ofToInt(t2);
//    ttemp+=timeZone;
//    t2 = ofToString(ttemp);
//    timestamp[11] = t2[0];
//    timestamp[12] = t2[1];
	
	return timestamp;
}

//save & print to shared directory of mapped-faces
//TODO: a5 printing (half the size)
void testApp::printPicture()
{
	if( currentState != IN_USE ) return;
	
	string imagePath = printed.getAbsolutePath();
	imagePath += "/";
	imagePath += getTimeStamp();
	imagePath += "_";
    imagePath += city;
	//imagePath += ofToString(printed.numFiles()); 
	imagePath += ".jpg";
	
	printf("PRINT PICTURE! %s", imagePath.c_str());
	
	//play sound
	sounds.play();
	//save picture
	ofSaveScreen(imagePath);
	videoCaptured = true;
	wait = 0;
	
	//TODO TEST
	//print picture
	string command = "lp -d " + printerName + " " + ofFilePath::getAbsolutePath(imagePath);
	printf(command.c_str());
	
	system(command.c_str());
	
	currentState = GOODBYE;
}

void testApp::saveNewFace()
{
	if(camTracker.getFound())
	{		
		//string capturePath = captured.getAbsolutePath() + ofToString(captured.numFiles()) + ".jpg";
		
		string capturePath = captured.getAbsolutePath();
		capturePath += "/";
		capturePath += getTimeStamp();
		//capturePath += ofToString(captured.numFiles()); 
		capturePath += ".jpg";
		
		printf("SAVE FACE for other city! %s", capturePath.c_str());	
		
		ofSaveImage(cam.getPixelsRef(), capturePath); //, ofImageQualityType qualityLevel=OF_IMAGE_QUALITY_BEST)
		imageCaptured = true;
		wait = 0;
		
	}
}

//TODO: Change this to a random shuffle
void testApp::shuffleFace()
{
	if( currentState != IN_USE ) return;
	
	currentFace--;
	
	//currentFace = ofClamp(currentFace,0,faces.size()-1);
	if(faces.size()!=0){
		
		if(currentFace == -1 ) currentFace = faces.size()-1;
		else if(currentFace == faces.size()) currentFace =0;
		
		printf("shuffle face %d/%d\n", currentFace, faces.size());
		loadFace(faces.getPath(currentFace));
	}
}

void testApp::errorMessage()
{
	
}

void testApp::keyPressed(int key){
	
	switch(key){
		case 'g':
			shuffleFace();
			break;
		case 's' :
			saveNewFace();
			break;
		case 'p':
			printPicture();
			break;
		case 'f':
			//not fully working yet! some bugs
            ofToggleFullscreen();
            break;
		case 'w':
			currentState = WELCOME;
			break;
		case 'i':
			currentState = IN_USE;
			break;
		case 'x':
			currentState = GOODBYE;
			break;
		case 'd':
			DEBUG = !DEBUG;
			break;
	}
	
}

void testApp::showWelcomeScreen()
{
	if( currentState == WELCOME)
		ofSetColor(255,0,0,180); //127);
	else ofSetColor(0,255,0,180); //127);
	
	ofRect(10,10,ofGetWindowWidth()-20, ofGetWindowHeight()-20);
	// alpha is usually turned off - for speed puposes.  let's turn it on!
	ofEnableAlphaBlending();
	
	/*string introEng = (currentState == WELCOME) ? "WELCOME TO THE PHOTOBOOTH":"GREAT!"; 
	string introEng2 = (currentState == WELCOME)? "PLEASE SIT AND FACE THE CAMERA": "LETS GET STARTED!"; 
	string introEng3 = "WHILE WE SCAN YOUR FACE";
	
	ofSetColor(225);
	
	if( currentState == WELCOME)
	{
		englishInstructions.drawString(introEng, (ofGetWindowWidth()- englishInstructions.stringWidth(introEng))/2, 200);
		englishInstructions.drawString(introEng2, (ofGetWindowWidth()- englishInstructions.stringWidth(introEng2))/2, 400);
		englishInstructions.drawString(introEng3, (ofGetWindowWidth()- englishInstructions.stringWidth(introEng3))/2, 500);
	}
	else {
		englishInstructions.drawString(introEng, (ofGetWindowWidth()- englishInstructions.stringWidth(introEng))/2, 300);
		englishInstructions.drawString(introEng2, (ofGetWindowWidth()- englishInstructions.stringWidth(introEng2))/2, 400);
	}*/


	
	if(!camTracker.getFound())
	{
		return;
	}
	
	//load latest face from other city
	if(faces.size()!=0){
		currentFace = faces.numFiles() - 1;
		loadFace(faces.getPath(currentFace));
	}
}

void testApp::showGoodbyeScreen()
{
	ofSetColor(0,0,0,255); //127);
	ofRect(0,0,ofGetWindowWidth(), ofGetWindowHeight());	
	ofEnableAlphaBlending();
	
	string goodbye1 = "THANK YOU!"; 
	string goodbye2 = "YOUR PHOTO IS PRINTING!"; 
	string goodbye3 = "GRAB IT OUTSIDE THE BOOTH!"; 
	
	
	ofSetColor(225);
	
	englishInstructions.drawString(goodbye1, (ofGetWindowWidth()- englishInstructions.stringWidth(goodbye1))/2, 100);
	englishInstructions.drawString(goodbye2, (ofGetWindowWidth()- englishInstructions.stringWidth(goodbye2))/2, 200);
	englishInstructions.drawString(goodbye3, (ofGetWindowWidth()- englishInstructions.stringWidth(goodbye3))/2, 300);
	
	if(city == "cairo") arabic.draw(0, 0);
}


void testApp::mousePressed(int x, int y, int button){
	
	printf("button pressed: %d", button);
	
	//0: Mouse Button 1, shuffle pictures
	//1: Mouse Button 2, print picture
	switch(button){
		case 0: 
			shuffleFace();
			break;
		case 1:
			printPicture();
			break;
	}
}

void testApp::onNewMessage(string & message)
{
	if(DEBUG) printf("serial message is %s\n", message.c_str());
	
    if (message == serialMessage && !isPressed) {
        isPressed = true;
    }
    else if(message != serialMessage && isPressed)
    {
        isPressed = false;
    }
}
