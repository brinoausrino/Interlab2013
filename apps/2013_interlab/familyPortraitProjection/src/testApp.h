#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxOpenCv.h"

enum IMAGE_MODE { COLOR, BLACKWHITE, SEPIA };


class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);	
	
    void savePicture();
    void updateTemporaryImage();
    void processCurrentImage();
    void onNewMessage(string & message);
    
    
    int                     threshold;
    bool                    bLearnBakground;
    
private:
    
    //variables for loading/saving images
    int                     nLayers;
    int                     maxShownLayers;
    vector<ofImage>         layer;  // the layers
    ofFbo                   fbo;
    ofxCvGrayscaleImage     toningPicture;
    ofxCvColorImage         toningPictureColor;
    ofPixels                toningPicturePixels;
    string                  destinationFolder;
    IMAGE_MODE              imageMode;
    
    
    // variables for image processing
    int                     _wCamera, _hCamera;
    ofImage                 _bgImg; //background image
    
    
    
    //textinterface
    bool                    showGui;
    	
};

