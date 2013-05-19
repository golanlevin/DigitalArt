#pragma once

#include "ofMain.h"
#include "ofxTiming.h"

class BufferedVideo : public ofBaseVideo {
protected:
	vector<ofPixels> images;
	int loaded;
	ofTexture texture;
	bool newFrame, playing;
	int currentFrame;
	DelayTimer timer;
	ofDirectory directory;
	void loadNextAvailable() {
		if(loaded < images.size()) {
			ofLoadImage(images[loaded], directory.getPath(loaded));
			loaded++;
		}
	}
	void updateFrame() {
		texture.loadData(images[currentFrame]);
		newFrame = true;
	}
public:
	BufferedVideo()
	:currentFrame(0)
	,playing(true)
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
	void setPlaying(bool playing) {
		this->playing = playing;
	}
	void goToPrevious() {
		currentFrame = (currentFrame - 1 + images.size()) % images.size();
		updateFrame();
	}
	void goToNext() {
		currentFrame = (currentFrame + 1) % images.size();
		updateFrame();
	}
	void update() {
		if(playing) {
			if(timer.tick()) {
				goToNext();
			}
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