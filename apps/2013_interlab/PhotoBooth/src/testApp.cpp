#include "testApp.h"

using namespace ofxCv;

void testApp::setup() {
#ifdef TARGET_OSX
	ofSetDataPathRoot("../../../data/");
#endif
	ofSetVerticalSync(true);
	cloneReady = false;
	cam.initGrabber(640, 480);
	clone.setup(cam.getWidth(), cam.getHeight());
	ofFbo::Settings settings;
	settings.width = cam.getWidth();
	settings.height = cam.getHeight();
	maskFbo.allocate(settings);
	srcFbo.allocate(settings);
	camTracker.setup();
	srcTracker.setup();
	srcTracker.setIterations(25);
	srcTracker.setAttempts(4);
	
	//camera capture sound, for fun
	sounds.loadSound("sounds/camera_snap.wav");

	//get city and directory settings
	ofxXmlSettings XML;
    if( XML.loadFile("mySettings.xml") ){
		cout << "mySettings.xml loaded!" << endl;
	}else{
		cout << "unable to load mySettings.xml check data/ folder" << endl;
	}
    
    destinationFolder = XML.getValue("FILES:PICTURE_FOLDER", "") + "Interlab/PhotoBooth/";
    city = ofToLower( XML.getValue("FILES:CITY", "") );
   /* timeZone = XML.getValue("FILES:TIMEZONE", 0);*/
	printerName = XML.getValue("IMAGE_PROCESSING:PRINTER", "");

	//DEBUG
	printf("PRINTER is %s", printerName.c_str());	
	printf("Current City: %s", city.c_str());
	
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
	
	//load latest face from other city
	if(faces.size()!=0){
		currentFace = faces.numFiles() - 1;
		loadFace(faces.getPath(currentFace));
	}
}

void testApp::update() {
	cam.update();
	
	if(wait > 100)
	{
		wait = 0;
	}
	else wait++;
	
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
		cam.draw(0, 0, ofGetWindowWidth(),ofGetWindowHeight());
	}
	
	if(!camTracker.getFound()) {
		drawHighlightString("camera face not found", 10, 10);
	}
	if(src.getWidth() == 0) {
		drawHighlightString("drag an image here", 10, 30);
	} else if(!srcTracker.getFound()) {
		drawHighlightString("image face not found", 10, 30);
	}
	
	if(imageCaptured) {
		if(wait==100) imageCaptured = false;
		drawHighlightString("Took a picture of you to send", 10, 60);
	}
	
	if(videoCaptured) {
		if(wait==100) videoCaptured = false;
		drawHighlightString("Took a picture of your NEW face", 10, 60);
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

string testApp::getTimeStamp()
{
	string timestamp = ofGetTimestampString();
    string t2 = timestamp.substr(11,2);
    int ttemp = ofToInt(t2);
    ttemp+=timeZone;
    t2 = ofToString(ttemp);
    timestamp[11] = t2[0];
    timestamp[12] = t2[1];
	
	return timestamp;
}

//save & print to shared directory of mapped-faces
void testApp::printPicture()
{
	string imagePath = printed.getAbsolutePath();
	imagePath += "/";
	imagePath += getTimeStamp();
	imagePath += "_";
    imagePath += city;
	//imagePath += ofToString(printed.numFiles()); 
	imagePath += ".jpg";
	
	printf("PRINT PICTURE! %s", imagePath.c_str());
	
	//save picture
	ofSaveScreen(imagePath);
	videoCaptured = true;
	
	//TODO TEST
	//print picture
	string command = "lp -d " + printerName + " " + ofFilePath::getAbsolutePath(imagePath);
	printf(command.c_str());
	
	system(command.c_str());
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
		
		//play sound
		sounds.play();
		ofSaveImage(cam.getPixelsRef(), capturePath); //, ofImageQualityType qualityLevel=OF_IMAGE_QUALITY_BEST)
		imageCaptured = true;
		
	}
}

//TODO: Change this to a random shuffle
void testApp::shuffleFace()
{
	currentFace--;
	
	//currentFace = ofClamp(currentFace,0,faces.size()-1);
	if(faces.size()!=0){
		
		if(currentFace == -1 ) currentFace = faces.size()-1;
		else if(currentFace == faces.size()) currentFace =0;
		
		loadFace(faces.getPath(currentFace));
	}
}

void testApp::errorMessage()
{
	
}

void testApp::keyPressed(int key){
	
	switch(key){
		case OF_KEY_DOWN:
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
	}
	
}

void testApp::mousePressed(int x, int y, int button){
	
	printf("button pressed: %d", button);
	
	//0: Mouse Button 1, shuffle pictures
	//1: Mouse Button 2, print picture
	switch(button){
		case 0: 
			break;
		case 1:
			break;
	}
    
}
