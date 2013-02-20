#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	/*if(_FULLSCREEN)
		ofSetupOpenGL(&window, 1680, 1050, OF_FULLSCREEN);
	else*/
		ofSetupOpenGL(&window, 640, 480, OF_WINDOW);
	
	ofRunApp(new testApp());
}
