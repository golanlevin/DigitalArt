#include "ofApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 512*2, 384*2, OF_WINDOW);
	ofRunApp(new ofApp());
}
