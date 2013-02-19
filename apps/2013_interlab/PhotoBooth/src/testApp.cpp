#include "testApp.h"

using namespace ofxCv;

void testApp::setup() {
#ifdef TARGET_OSX
	ofSetDataPathRoot("../../../data/");
#endif
	ofSetVerticalSync(true);
	cloneReady = false;
	/*if(_FULLSCREEN)
		cam.initGrabber(ofGetScreenWidth(),ofGetScreenHeight());
	else*/
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
	
	sounds.loadSound("sounds/camera_snap.wav");
	//sounds.setMultiPlay(true);

	//TODO: get directory settings
	/*ofxXmlSettings XML;
    if( XML.loadFile("mySettings.xml") ){
		cout << "mySettings.xml loaded!" << endl;
	}else{
		cout << "unable to load mySettings.xml check data/ folder" << endl;
	}
    
    destinationFolder = XML.getValue("FILES:PICTURE_FOLDER", "images/");
    city = XML.getValue("FILES:CITY", "");
    timeZone = XML.getValue("FILES:TIMEZONE", 0);*/
	
	
	//TOOD: connect to drop box folderss
	
	//directory of faces to map onto live image
	faces.allowExt("jpg");
	faces.allowExt("png");
	faces.listDir("faces");
	
	//diretory of images saved (clean user image)
	captured.listDir("capture");
	
	//directory of mapped-face that selected to print/save
	printed.listDir("saved");
	
	wait = 0;
	currentFace = 0;
	imageSequence = 0;
	videoCaptured = imageCaptured = false;
	
	
	printf("# of files %d",faces.numFiles());
	
	//TODO: change to latest face from Dresden, right click/left click toggles through faces
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
		cam.draw(0, 0);
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

void testApp::keyPressed(int key){
	
	printf("faces.size() = %d. current face %d",faces.size(),currentFace);
	switch(key){
	/*case OF_KEY_UP:
		currentFace++;
		break;*/
	case OF_KEY_DOWN:
		currentFace--;
		break;
	case OF_KEY_RIGHT:
			if(camTracker.getFound())
			{
				string imagePath = "saved/image" + ofToString(printed.numFiles()) + ".jpg";
				ofSaveScreen(imagePath);
				videoCaptured = true;
			}
			break;
	case OF_KEY_LEFT:
			if(camTracker.getFound())
			{
				string capturePath = "capture/image" + ofToString(captured.numFiles()) + ".jpg";
				sounds.play();
				ofSaveImage(cam.getPixelsRef(), capturePath); //, ofImageQualityType qualityLevel=OF_IMAGE_QUALITY_BEST)
				imageCaptured = true;
				
			}
			break;
	}

	
	//currentFace = ofClamp(currentFace,0,faces.size()-1);
	if(faces.size()!=0){
		
		if(currentFace == -1 ) currentFace = faces.size()-1;
		else if(currentFace == faces.size()) currentFace =0;
		
		loadFace(faces.getPath(currentFace));
	}
}
