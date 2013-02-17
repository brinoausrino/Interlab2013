#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    _wCamera = 640;
    _hCamera = 480;

        _vidGrabber.setVerbose(true);
        _vidGrabber.initGrabber(_wCamera,_hCamera);


	bLearnBakground = true;
	threshold = 20;
    c_threshold = 1;
    
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
    
    useFullCollorDifference = true;
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
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(100,100,100);

    processCurrentImage();
    
    //update images;
    ofDirectory dir;
    int nLayersNew = dir.listDir("images/");
    if (nLayers < nLayersNew) {
        for (int i=nLayers; i<nLayersNew; ++i) {
            ofImage img;
            img.loadImage(dir.getPath(i));
            layer.push_back(img);
        }
    }
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetHexColor(0xffffff);
    ofEnableAlphaBlending();
	for (int i=0; i<layer.size(); ++i) {
        layer[i].draw(0, 0,ofGetWindowWidth(),ofGetWindowHeight());
    }
    tempPic.draw(0, 0,ofGetWindowWidth(),ofGetWindowHeight());
   
    if (showGui)
    {
        string t = "threshold: ";
        t+= ofToString(threshold);
        ofSetColor(255, 255, 255);
        ofDrawBitmapString(t, 20, 500);
    }
}

void testApp::savePicture(){
    
    updateTemporaryImage();
    
    
    
    //save picture
    ofxXmlSettings XML;
    if( XML.loadFile("mySettings.xml") ){
		cout << "mySettings.xml loaded!" << endl;
	}else{
		cout << "unable to load mySettings.xml check data/ folder" << endl;
	}
    
	string city = XML.getValue("SETTINGS:CITY", "");
    int timeZone = XML.getValue("SETTINGS:TIMEZONE", 0);
    
	
    string timestamp = ofGetTimestampString();
    string t2 = timestamp.substr(11,2);
    int ttemp = ofToInt(t2);
    ttemp+=timeZone;
    t2 = ofToString(ttemp);
    timestamp[11] = t2[0];
    timestamp[12] = t2[1];
    
    string file = "images/"; 
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
    bool bNewFrame = false;
    
    _vidGrabber.update();
    bNewFrame = _vidGrabber.isFrameNew();
    
	if (bNewFrame){
        _camImg.setFromPixels(_vidGrabber.getPixels(),_wCamera,_hCamera);
        updateTemporaryImage();
        
		if (bLearnBakground == true){
			_bgImg = _camImg;
            _bgImg.convertToGrayscalePlanarImages(redChannel_bg,greenChannel_bg,blueChannel_bg);
			bLearnBakground = false;
		}
        
        
        // mask picture
        //generate binary picture from highest contrast of each color chanel
        if(useFullCollorDifference)
        {
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
                
                if (dr > threshold && dg > threshold && db > threshold) {
                    diffWorkImage[i] = 255;
                }
                else
                    diffWorkImage[i] = 0;
            }
            
            
            /*
             for (int i = 0; i < totalPixels; i++)
             {
             a=(pixels_r[i]*pixels_r_bg[i])+(pixels_g[i]*pixels_g_bg[i])+(pixels_b[i]*pixels_b_bg[i]);
             b=((pixels_r_bg[i]*pixels_r_bg[i])+(pixels_g_bg[i]*pixels_g_bg[i])+(pixels_b_bg[i]*pixels_b_bg[i]));
             d=a/b;
             v1=(pixels_r[i]-d*pixels_r_bg[i]);
             v1=v1*v1;
             v2=(pixels_g[i]-d*pixels_g_bg[i]);
             v2=v2*v2;
             v3=(pixels_b[i]-d*pixels_b_bg[i]);
             v3=v3*v3;
             
             diffWorkImage[i]=0;
             
             //treshold for ligthness and color
             if((abs((d)-1))>(double)threshold/100&&sqrt(v1+v2+v3)>c_threshold)
             {
             diffWorkImage[i]=255;
             }
             }*/
            BinImage=diffWorkImage;
            BinImage.erode();
            BinImage.blurGaussian();
            BinImage.dilate_3x3();
            
            
        }
        
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
