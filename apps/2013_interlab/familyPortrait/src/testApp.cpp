#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    _wCamera = 640;
    _hCamera = 480;
    
    _vidGrabber.setVerbose(true);
    _vidGrabber.initGrabber(_wCamera,_hCamera);
    
    
	bLearnBakground = true;
	
    
    //allocate the pictures
    
    _camImg.allocate(_wCamera, _hCamera);
    _bgImg.allocate(_wCamera, _hCamera);
    
    BinImage.allocate(_wCamera,_hCamera);
    greenChannel.allocate(_wCamera,_hCamera);
    redChannel.allocate(_wCamera,_hCamera);
    blueChannel.allocate(_wCamera,_hCamera);
    greenChannel_bg.allocate(_wCamera,_hCamera);
    redChannel_bg.allocate(_wCamera,_hCamera);
    blueChannel_bg.allocate(_wCamera,_hCamera);
    diffWorkImage=new unsigned char[_wCamera*_hCamera];
    
    fbo.allocate(_wCamera, _hCamera, GL_RGBA);
    tempPic.allocate(_wCamera, _hCamera, OF_IMAGE_COLOR_ALPHA);
    
    colorDifferenceMethod = 0;
    
    
    //load the images
    ofDirectory dir;
    nLayers = dir.listDir("images/");
    for (int i=0; i<nLayers; ++i) {
        ofImage img;
        img.loadImage(dir.getPath(i));
        layer.push_back(img);
    }
    
    showGui = false;
    
    
    ofxXmlSettings XML;
    if( XML.loadFile("mySettings.xml") ){
		cout << "mySettings.xml loaded!" << endl;
	}else{
		cout << "unable to load mySettings.xml check data/ folder" << endl;
	}
    
    destinationFolder = XML.getValue("FILES:PICTURE_FOLDER", "images/");
    threshold = XML.getValue("IMAGE_PROCESSING:THRESHOLD",80);
    maxShownLayers = XML.getValue("IMAGE_PROCESSING:MAX_SHOWN_PICTURES",10);
    city = XML.getValue("FILES:CITY", "");
    timeZone = XML.getValue("FILES:TIMEZONE", 0);
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(100,100,100);
    
    processCurrentImage();
    
    //update images;
    ofDirectory dir;
    int nLayersNew = dir.listDir(destinationFolder);
    if (nLayers < nLayersNew) {
        for (int i=nLayers; i<nLayersNew; ++i) {
            ofImage img;
            img.loadImage(dir.getPath(i));
            layer.push_back(img);
            nLayers = nLayersNew;
        }
    }
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    //draw images
    
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
    
    //draw temporary figure
    tempPic.draw(0, 0,ofGetWindowWidth(),ofGetWindowHeight());
    
    
    //draw gui
    if (showGui)
    {
        string t = "threshold (+/-): ";
        t+= ofToString(threshold);
        t+=("\n");
        t+= "maxShownLayers (n/m): ";
        t+= ofToString(maxShownLayers);
        t+=("\n");
        t+= "image directory: ";
        t+= destinationFolder;
        t+=("\n");
        t+= "city: ";
        t+= city;
        t+=("\n");
        t+= "timezone: ";
        t+= ofToString(timeZone);
        ofSetColor(0, 0, 0);
        ofRect(0, ofGetWindowHeight() - 85, 700, 85);
        ofSetColor(255, 255, 255);
        ofDrawBitmapString(t, 20, ofGetWindowHeight() - 70);
    }
}

void testApp::savePicture(){
     
    //save picture
    ofxXmlSettings XML;
    if( XML.loadFile("mySettings.xml") ){
		cout << "mySettings.xml loaded!" << endl;
	}else{
		cout << "unable to load mySettings.xml check data/ folder" << endl;
	}
    
	
    
	
    string timestamp = ofGetTimestampString();
    string t2 = timestamp.substr(11,2);
    int ttemp = ofToInt(t2);
    ttemp+=timeZone;
    t2 = ofToString(ttemp);
    timestamp[11] = t2[0];
    timestamp[12] = t2[1];
    
    string file = destinationFolder; 
    file += timestamp;
    file += "_";
    file += city;
    file += ".png";
    
    
    tempPic.saveImage(file);
}

void testApp::updateTemporaryImage(){
    
    unsigned char* pix = tempPic.getPixels();
    unsigned char* pixCam = _camImg.getPixels();
    unsigned char* mask = BinImage.getPixels();
    
    for (int i=0; i<_wCamera*_hCamera; ++i) {
        pix[i*4] =  pixCam[i*3];
        pix[i*4+1] =  pixCam[i*3+1];
        pix[i*4+2] =  pixCam[i*3+2];
        pix[i*4+3] = mask[i];
    }
    tempPic.setFromPixels(pix, _wCamera, _hCamera, OF_IMAGE_COLOR_ALPHA);
}

void testApp::processCurrentImage(){
    //update if camera has new frame
    bool bNewFrame = false;
    _vidGrabber.update();
    bNewFrame = _vidGrabber.isFrameNew();
    
	if (bNewFrame){
        
        //update frame
        _camImg.setFromPixels(_vidGrabber.getPixels(),_wCamera,_hCamera);
        updateTemporaryImage();
        
		if (bLearnBakground == true){
			_bgImg = _camImg;
            _bgImg.convertToGrayscalePlanarImages(redChannel_bg,greenChannel_bg,blueChannel_bg);
			bLearnBakground = false;
		}
        
        
        // mask picture
        //generate binary picture from highest contrast of each color chanel
        //Get Color Chanels
        int totalPixels = _wCamera*_hCamera;
        
        _camImg.convertToGrayscalePlanarImages(redChannel,greenChannel,blueChannel);
        
        
        pixels_r = redChannel.getPixels();
        pixels_g = greenChannel.getPixels();
        pixels_b = blueChannel.getPixels();
        
        pixels_r_bg = redChannel_bg.getPixels();
        pixels_g_bg = greenChannel_bg.getPixels();
        pixels_b_bg = blueChannel_bg.getPixels();
        
        /* Generate Difference Picture from Distance in Color Space and Distance in lightness*/
        
        for (int i = 0; i < totalPixels; i++)
        {
            int dr, db, dg;
            dr = pixels_r[i] - pixels_r_bg[i];
            if (dr<0) dr*=-1;
            dg = pixels_g[i] - pixels_g_bg[i];
            if (dg<0) dg*=-1;
            db = pixels_b[i] - pixels_b_bg[i];
            if (db<0) db*=-1;
            
            if (dr > threshold || dg > threshold || db > threshold) {
                diffWorkImage[i] = 255;
            }
            else
                diffWorkImage[i] = 0;
        }
        
        //improve edges 
        BinImage=diffWorkImage;
        BinImage.erode();
        BinImage.blurGaussian();
        BinImage.dilate_3x3();
        
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
	switch (key){
		case ' ':
			bLearnBakground = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
        case 'n':
			maxShownLayers--;
			if (maxShownLayers < 0) maxShownLayers = 0;
			break;
        case 'm':
			maxShownLayers++;
			break;
        case 'p':
            savePicture();
            break;
        case 'g':
            showGui = !showGui;
            break;
        case 'f':
            ofToggleFullscreen();
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
