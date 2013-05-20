#include "ofApp.h"
#include "SharedUtils.h"

void ofApp::setup() {
	sharedSetup();
	setupGui();
	video.load("recording");
	thresholdValue = 30;
	active = true;
	intermediate = false;
	playing = true;
	mask.loadImage("mask.png");

	//allocate (T& img, int width, int height, int cvType)
	//imitate(dst, src);
	//copy (thresholded, thresholdedCleaned);
	
	minAllowableContourAreaAsAPercentOfImageSize = 0.05;
	maxAllowableContourAreaAsAPercentOfImageSize = 0.12; // approx 100000.0 / (768*1024);
	
	bHandyBool = false;
	bDoMorphologicalCleanup = true;
}

void ofApp::setupGui() {
	gui = new ofxUICanvas();
	gui->addLabel("Mesh Generation");
	gui->addSpacer();
	gui->addFPS();
	gui->addSlider("Threshold", 0.0, 255.0, &thresholdValue);
	gui->addSlider("MinContourArea%", 0.0, 0.10, &minAllowableContourAreaAsAPercentOfImageSize);
	gui->addSlider("MaxContourArea%", 0.0, 0.25, &maxAllowableContourAreaAsAPercentOfImageSize);
	gui->addSlider("ContourSmoothing", 0.0, 50.0,&(HCAAMB.contourSmoothingSize));
	gui->addSlider("ContourResampling", 1.0, 20.0, &(HCAAMB.contourResampleSpacing));
	
	gui->addSpacer();
	
	gui->addSlider("Sample offset", 1, 40, &(HCAAMB.sampleOffset));
	gui->addSlider("Peak angle cutoff", 0, 60, &(HCAAMB.peakAngleCutoff));
	gui->addSlider("Peak neighbor distance", 0, 100, &(HCAAMB.peakNeighborDistance));
	
	gui->addSpacer();
	
	float			sampleOffset;
	float			peakAngleCutoff;
	float			peakNeighborDistance;
	
	gui->addLabelToggle("Active", &active);
	gui->addLabelToggle("Intermediate", &intermediate);
	gui->addLabelToggle("Play", &playing);
	gui->addLabelToggle("Morphological", &bDoMorphologicalCleanup);
	gui->autoSizeToFitWidgets();
}



//==============================================================
void ofApp::update() {
	video.update();
	
	
	if(video.isFrameNew() || !playing) {
		
		
		// Fetch the (color) video, and convert to (properly weighted) grayscale.
		Mat videoMat = toCv(video);
		convertColor(videoMat, gray, CV_RGB2GRAY);
		imgH = videoMat.rows;
		imgW = videoMat.cols;
		
		// Threshold the result, and mask against relevant-area quad.
		threshold(gray, thresholded, thresholdValue);
		bitwise_and(mask, thresholded, thresholded);
		
		// Morphological cleanup filtering here. Uses thresholded; puts results in thresholdedCleaned
		doMorphologicalCleanupOnThresholdedVideo();
		
		// Extract the contour(s) of the binarized image, and FIND THE CONTOURS
		contourFinder.setMinAreaNorm( minAllowableContourAreaAsAPercentOfImageSize );
		contourFinder.setMaxAreaNorm( maxAllowableContourAreaAsAPercentOfImageSize );
		contourFinder.findContours(thresholdedCleaned);
		
		// Find the index ID of the largest contour, which is most likely the hand.
		int indexOfHandContour = NO_VALID_HAND;
		float largestContourArea = 0; 
		vector <ofPolyline> polylineContours = contourFinder.getPolylines();
		int nPolylineContours = polylineContours.size();
		for (int i=0; i<nPolylineContours; i++){
			// 
			float contourArea = contourFinder.getContourArea(i);
			if (contourArea > largestContourArea){
				contourArea = largestContourArea; 
				indexOfHandContour = i; 
			}
		}
		bValidHandContourExists = false;
		if (indexOfHandContour != NO_VALID_HAND){
			bValidHandContourExists = true;
			handContourPolyline = contourFinder.getPolyline(indexOfHandContour);
			handContourCentroid = contourFinder.getCentroid(indexOfHandContour);
			HCAAMB.process(handContourPolyline, handContourCentroid);
		}
		
		 
	}
	video.setPlaying(playing);
}

//==============================================================
void ofApp::doMorphologicalCleanupOnThresholdedVideo(){
	
	if (bDoMorphologicalCleanup){
		
		bool bUseRoundMorphologicalStructuringElement = false;
		int elementSize = 5;
		cv::Mat structuringElement = Mat();
		if (bUseRoundMorphologicalStructuringElement){
			structuringElement = cv::getStructuringElement(cv::MORPH_ELLIPSE,
														   cv::Size(2*elementSize + 1, 2*elementSize + 1),
														   cv::Point(elementSize, elementSize) );
		}
		
		// Opening: 
		// cv::erode  (thresholded, thresholdedCleaned, structuringElement);
		// cv::dilate (thresholded, thresholdedCleaned, structuringElement);
		
		cv::medianBlur(thresholded, thresholdedCleaned, elementSize);
	
	} else {
		copy (thresholded, thresholdedCleaned);
		
	}
}



void ofApp::draw() {
	if(active) {
		
		ofPushStyle();
		
		ofSetColor(64);
		drawMat(thresholdedCleaned, 0, 0);
		
		//ofSetColor(255,0,0);
		//contourFinder.draw();
		
		if (bValidHandContourExists){
			//ofSetColor(0,255,0);
			//ofSetLineWidth(2);
			//handContourPolyline.draw();
			HCAAMB.drawAnalytics();
		}
		ofPopStyle();
		
	} else {
		
		ofPushStyle();
		ofSetColor(255); 
		video.draw(0, 0);
		ofPopStyle();
	}
	if(intermediate) {
		
		ofPushMatrix();
		float miniScale = 0.2;
		ofTranslate(0, ofGetHeight() - (miniScale * imgH));
		ofScale(miniScale, miniScale);
		
		int xItem = 0;  
		drawMat(gray,        imgW * xItem, 0); xItem++;
		drawMat(thresholded, imgW * xItem, 0); xItem++;
		ofPopMatrix();
	}		
}

void ofApp::keyPressed(int key) {
	if(key == ' ') {
		playing = !playing;
	}
	if(key == OF_KEY_LEFT) {
		video.goToPrevious();
	}
	if(key == OF_KEY_RIGHT) {
		video.goToNext();
	}
	if (key == 'b'){
		bHandyBool = !bHandyBool; 
	}
}