#include "ofApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 768, 1024, OF_WINDOW);
	ofRunApp(new ofApp());
}
