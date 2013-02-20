#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofxXmlSettings XML;
    if( XML.loadFile("mySettings.xml") ){
		cout << "mySettings.xml loaded!" << endl;
	}else{
		cout << "unable to load mySettings.xml check data/ folder" << endl;
	}
    
    destinationFolder = XML.getValue("FILES:PICTURE_FOLDER", "images/");
    maxShownLayers = XML.getValue("IMAGE_PROCESSING:MAX_SHOWN_PICTURES",11);
    _wCamera = XML.getValue("IMAGE_PROCESSING:WIDTH",10);
    _hCamera = XML.getValue("IMAGE_PROCESSING:HEIGTH",10);
    
    
    string imgMode = XML.getValue("IMAGE_PROCESSING:IMAGE_MODE","COLOR");
    if (imgMode == "COLOR") {
        imageMode = COLOR;
    }
    else if (imgMode == "BLACKWHITE"){
        imgMode = BLACKWHITE;
    }
    else{
        imgMode = SEPIA;
    }
    
    
    
    
    ofSetWindowShape(_wCamera, _hCamera);
    
	bLearnBakground = true;
	
    
    //allocate the pictures
    
    
    toningPicture.allocate(_wCamera, _hCamera);
    toningPictureColor.allocate(_wCamera, _hCamera);
    
    _bgImg.loadImage("images/bg.jpg");
    
    ofFbo::Settings settings;
    settings.width = _wCamera;
    settings.height = _hCamera;
    settings.internalformat = GL_RGB;
    settings.numSamples = 0;
    settings.useDepth = false;
    settings.useStencil = false;
    fbo.allocate(settings);
    
    //toningPicturePixels.allocate(_wCamera, _hCamera);

    
    //load the images
    ofDirectory dir;
    nLayers = dir.listDir("images/");
    for (int i=0; i<nLayers; ++i) {
        ofImage img;
        img.loadImage(dir.getPath(i));
        layer.push_back(img);
        layerName.push_back(dir.getPath(i));
    }
    
    showGui = false;
    
    
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(100,100,100);
    
   
    
    ofDirectory dir;
    int nLayersNew = dir.listDir(destinationFolder);
    if (nLayers < nLayersNew) {
        for (int i=0; i<nLayersNew; ++i) {
            string path = dir.getPath(i);
            bool isIn = false;
            for (int j=0; j<layer.size(); ++j) {
                if (path == layerName[j]) {
                    isIn = true;
                }
            }
            if (!isIn) {
                ofImage img;
                img.loadImage(dir.getPath(i));
                layer.push_back(img);
                layerName.push_back(dir.getPath(i));
            }  
        }
        nLayers = nLayersNew;
    }

    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    //draw images
    if (imageMode != COLOR) {
        fbo.begin();
    }
    //draw backgound
    ofSetHexColor(0xffffff);
    _bgImg.draw(0, 0,ofGetWindowWidth(),ofGetWindowHeight());
    ofEnableAlphaBlending();
    
    //draw layers
    int start = layer.size()-maxShownLayers;
    if (start<0) start = 0;
    
	for (int i=start; i<layer.size(); ++i) {
        layer[i].draw(0, 0,ofGetWindowWidth(),ofGetWindowHeight());
    }
    
    if (imageMode != COLOR) {
        fbo.end();
        fbo.getTextureReference().readToPixels(toningPicturePixels);
        toningPictureColor.setFromPixels(toningPicturePixels);
        toningPicture = toningPictureColor;
        if (imageMode == SEPIA) {
            ofSetColor(162, 138, 101);
        }
        toningPicture.draw(0, 0,ofGetWindowWidth(),ofGetWindowHeight());
    }
    
    
    //draw gui
    if (showGui)
    {
        string t = "maxShownLayers (n/m): ";
        t+= ofToString(maxShownLayers);
        t+=("\n");
        t+= "imageMode (i): ";
        switch (imageMode) {
            case COLOR:
                t+="COLOR";
                break;
            case BLACKWHITE:
                t+="BLACKWHITE";
                break;    
            case SEPIA:
                t+="SEPIA";
                break; 
        }
        t+=("\n");
        t+= "ImageResolution: ";
        t+= ofToString(_wCamera);
        t+= " * ";
        t+=  ofToString(_hCamera);
        t+=("\n");
        t+= "image directory: ";
        t+= destinationFolder;
        ofSetColor(0, 0, 0);
        ofRect(0, ofGetWindowHeight() - 130, 700, 130);
        ofSetColor(255, 255, 255);
        ofDrawBitmapString(t, 20, ofGetWindowHeight() - 115);
    }
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
	switch (key){
        case 'n':
			maxShownLayers--;
			if (maxShownLayers < 0) maxShownLayers = 0;
			break;
        case 'm':
			maxShownLayers++;
			break;
        case 'g':
            showGui = !showGui;
            break;
        case 'f':
            ofToggleFullscreen();
            break;
        case 'i':
            switch (imageMode) {
                case COLOR:
                    imageMode = BLACKWHITE;
                    break;
                case BLACKWHITE:
                    imageMode = SEPIA;
                    break;    
                case SEPIA:
                    imageMode = COLOR;
                    break; 
            }
            break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
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

