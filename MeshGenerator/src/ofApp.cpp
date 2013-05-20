#include "ofApp.h"
#include "SharedUtils.h"

void ofApp::setup() {
	sharedSetup();
	setupGui();
	video.load("recording");
	
	thresholdValue			= 26;
	prevThresholdValue		= 0;
	blurKernelSize			= 4;
	blurredStrengthWeight	= 0.08;
	
	bHandyBool = false;
	bDoAdaptiveThresholding = true;
	bDoMorphologicalCleanup = true;
	bDoMorphologicalOpening = false; 
	
	active = true;
	intermediate = false;
	playing = true;
	mask.loadImage("mask.png");

	allocate (thresholdConstMat, 768, 1024, CV_8UC1);
	allocate (adaptiveThreshImg, 768, 1024, CV_8UC1);
	
	//imitate(dst, src);
	//copy (thresholded, thresholdedCleaned);
	
	minAllowableContourAreaAsAPercentOfImageSize = 0.05;
	maxAllowableContourAreaAsAPercentOfImageSize = 0.12; // approx 100000.0 / (768*1024);
	
	
}

void ofApp::setupGui() {
	gui = new ofxUICanvas();
	gui->addLabel("Mesh Generation");
	gui->addSpacer();
	gui->addFPS();
	gui->addSlider("Threshold", 0.0, 255.0, &thresholdValue);
	gui->addSlider("BlurKernelSize", 1, 63, &blurKernelSize);
	gui->addSlider("BlurredStrengthFactor", 0.0, 1.5, &blurredStrengthWeight);
	gui->addSpacer();
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
	gui->addLabelToggle("Morph-Opening", &bDoMorphologicalOpening);
	gui->addLabelToggle("Adaptive", &bDoAdaptiveThresholding);
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
		
		// Mask against relevant-area quad, and then threshold. 
		bitwise_and(mask, gray, gray);
		
		
		if (bDoAdaptiveThresholding){
			
			// Copy the gray image to a very small version
			cv::Size blurredSmallSize = cv::Size(imgW/4, imgH/4);
			resize(gray, graySmall, blurredSmallSize, 0,0, INTER_NEAREST );
			 
			// Blur the heck out of the very small version
			int kernelSize = ((int)(blurKernelSize)*2 + 1);
			float thresholdBlurSigma = kernelSize/2.0;
			cv::Size blurSize = cv::Size(kernelSize, kernelSize);
			GaussianBlur (graySmall, blurredSmall, blurSize, thresholdBlurSigma);
			
			// Upscale the small blurry version into a large blurred version
			cv::Size blurredSize = cv::Size(imgW,imgH);
			resize(blurredSmall, blurred, blurredSize, 0,0, INTER_LINEAR );
			
			// Fill the thresholdConstMat with the threshold value (if it has changed). 
			if (thresholdValue != prevThresholdValue){
				thresholdConstMat.setTo( (unsigned char) ((int)thresholdValue));
			}
			prevThresholdValue = thresholdValue;
			
			// Create the adaptiveThreshImg by adding a weighted blurred + constant image.
			cv::scaleAdd (blurred, blurredStrengthWeight, thresholdConstMat, adaptiveThreshImg);
			
			// Do the actual (adaptive thresholding. 
			cv::subtract (gray, adaptiveThreshImg, tempGrayscaleMat);
			int thresholdMode = cv::THRESH_BINARY; // cv::THRESH_BINARY_INV
			cv::threshold (tempGrayscaleMat, thresholded, 1, 255, thresholdMode);
		
		} else {
			// If we are not adaptive thresholding, just use a regular threshold. 
			threshold(gray, thresholded, thresholdValue);
		}
		
		
		// Morphological cleanup filtering here.
		// Uses thresholded image; puts results in thresholdedCleaned
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
		
		
		int elementSize = 5;
		
		if (bDoMorphologicalOpening){
			bool bUseRoundMorphologicalStructuringElement = true;
			cv::Mat structuringElement = Mat();
			if (bUseRoundMorphologicalStructuringElement){
				structuringElement = cv::getStructuringElement(cv::MORPH_ELLIPSE,
															   cv::Size(2*elementSize + 1, 2*elementSize + 1),
															   cv::Point(elementSize, elementSize) );
			}
	 
			cv::erode  (thresholded, thresholdedCleaned, structuringElement);
			//cv::dilate (thresholded, thresholdedCleaned, structuringElement);
		}
		
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
		
		// drawMat(thresholdedCleaned, 0,0);
		// drawMat(blurred, 0,0);
		video.draw(0, 0);
		ofPopStyle();
	}
	if(intermediate) {
		
		ofPushMatrix();
		float miniScale = 0.2;
		ofTranslate(0, ofGetHeight() - (miniScale * imgH));
		ofScale(miniScale, miniScale);
		
		int xItem = 0;  
		drawMat(gray,				imgW * xItem, 0); xItem++;
		drawMat(thresholded,		imgW * xItem, 0); xItem++;
		drawMat(adaptiveThreshImg,	imgW * xItem, 0); xItem++;
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