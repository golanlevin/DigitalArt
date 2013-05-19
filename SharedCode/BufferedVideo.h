#pragma once

#include "ofMain.h"
#include "ofxTiming.h"

class BufferedVideo : public ofBaseVideo {
protected:
	vector<ofPixels> images;
	int loaded;
	ofTexture texture;
	bool newFrame;
	int currentFrame;
	DelayTimer timer;
	ofDirectory directory;
	void loadNextAvailable() {
		if(loaded < images.size()) {
			ofLoadImage(images[loaded], directory.getPath(loaded));
			loaded++;
		}
	}
public:
	BufferedVideo()
	:currentFrame(0)
	,newFrame(false)
	,loaded(0) {
		timer.setFramerate(30);
	}
	void setFrameRate(float frameRate) {
		timer.setFramerate(frameRate);
	}
	void load(string directoryName) {
		directory.listDir(directoryName);
		images.resize(directory.size());
		loadNextAvailable();
		texture.allocate(images[0]);
	}
	void close() {
	}
	unsigned char* getPixels() {
		return images[currentFrame].getPixels();
	}
	ofPixels& getPixelsRef() {
		return images[currentFrame];
	}
	ofPixels& getFrame(int i) {
		return images[i];
	}
	void update() {
		if(timer.tick()) {
			texture.loadData(images[currentFrame]);
			newFrame = true;
			currentFrame = (currentFrame + 1) % images.size();
		}
		loadNextAvailable();
	}
	bool isFrameNew() {
		bool cur = newFrame;
		newFrame = false;
		return cur;
	}
	void draw(float x, float y) {
		texture.draw(x, y);
	}
	void draw(float x, float y, float w, float h) {
		texture.draw(x, y, w, h);
	}
};