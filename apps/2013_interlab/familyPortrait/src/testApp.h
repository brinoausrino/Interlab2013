#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxOpenCv.h"
#include "ofxSimpleSerial.h"

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
    
    
    ofVideoGrabber          _vidGrabber;
    
    
    int                     threshold;
    bool                    bLearnBakground;
    
private:
    
    //variables for loading/saving images
    int                     nLayers;
    int                     maxShownLayers;
    vector<ofImage>         layer;  // the layers
    vector<string>          layerName;
    ofFbo                   fbo;
    ofxCvGrayscaleImage     toningPicture;
    ofxCvColorImage         toningPictureColor;
    ofPixels                toningPicturePixels;
    string                  destinationFolder;
    ofImage                 tempPic;
    string                  city;
    int                     timeZone;
    IMAGE_MODE              imageMode;
    
    
    // variables for image processing
    int                     _wCamera, _hCamera;
    ofxCvColorImage			_camImg; //camera image
    ofxCvColorImage         _bgImg; //background image
    
    //bg_substraction stuff
    ofxCvGrayscaleImage 	greenChannel_bg;
	ofxCvGrayscaleImage 	redChannel_bg;
	ofxCvGrayscaleImage 	blueChannel_bg;
    
    ofxCvGrayscaleImage 	greenChannel;
	ofxCvGrayscaleImage 	redChannel;
	ofxCvGrayscaleImage 	blueChannel;
    ofxCvGrayscaleImage 	BinImage;
    //temporary pointers to colorarrays
    unsigned char			*pixels_r,*pixels_g,*pixels_b,*pixels_r_bg,*pixels_g_bg,*pixels_b_bg; 
    bool                    useFullCollorDifference;
    int                     colorDifferenceMethod;
    unsigned char *			diffWorkImage;
	unsigned char *			diffWorkImage_skin;
    
    
    
    //textinterface
    bool                    showGui;
    
    //serial
    bool                    useSerial;
    bool                    isPressed;
    string                  serialDevice;
    int                     baudRate;
    string                  serialMessage;
    ofxSimpleSerial         _serial;
    float                   readTime;	
    int                     shotTime, shotBegin;
    
    ofSoundPlayer sounds;
};

