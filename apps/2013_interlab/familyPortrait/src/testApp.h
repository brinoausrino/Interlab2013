#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxOpenCv.h"


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
        

        ofVideoGrabber 		_vidGrabber;


        ofxCvColorImage			colorImg;

        ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayBg;
		ofxCvGrayscaleImage 	grayDiff;

        ofxCvContourFinder 	contourFinder;

		int 				threshold;
		bool				bLearnBakground;
    
    private:
    
    //variables for loading/saving images
    int nLayers;
    
     vector<ofImage> layer;  // the layers
    ofFbo fbo;
    
    
    
    // variables for image processing
    int _wCamera, _hCamera;
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
    
    unsigned char			*pixels_r,*pixels_g,*pixels_b,*pixels_r_bg,*pixels_g_bg,*pixels_b_bg; //temporary pointers to colorarrays
    double					a,b,d,v1,v2,v3; //used für color difference methods
    bool    useFullCollorDifference;
    int colorDifferenceMethod;
    unsigned char *			diffWorkImage;
	unsigned char *			diffWorkImage_skin;
    
    int c_threshold;
    
    
    ofImage tempPic;
   
    
    //textinterface
    bool showGui;
    

};

