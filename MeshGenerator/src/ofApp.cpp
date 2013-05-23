#include "ofApp.h"
#include "SharedUtils.h"

void ofApp::setup() {
	sharedSetup();
	setupGui();
	video.load("recording");
	
	thresholdValue			= 26;
	prevThresholdValue		= 0;
	blurKernelSize			= 4;
	blurredStrengthWeight	= 0.07;
	handyFloat01			= 0;
	minLaplaceEdgeStrength  = 184;
	

	
	bHandyBool = false;
	bDoAdaptiveThresholding = true;
	bDoLaplacianEdgeDetect  = true;
	
	active = true;
	intermediate = false;
	playing = true;
	mask.loadImage("mask.png");
	lineFormationGradientImage.loadImage("lineFormationGradient.png");

	allocate (thresholdConstMat, 768, 1024, CV_8UC1);
	allocate (adaptiveThreshImg, 768, 1024, CV_8UC1);
	allocate (goodEdgesImg,      768, 1024, CV_8UC1);
	allocate (tempGrayscaleMat1, 768, 1024, CV_8UC1);
	allocate (tempGrayscaleMat2, 768, 1024, CV_8UC1);
	
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
	//gui->addSlider("Sample offset", 1, 40, &(HCAAMB.sampleOffset));
	//gui->addSlider("Peak angle cutoff", -150, 0, &(HCAAMB.peakAngleCutoff));
	//gui->addSlider("Peak neighbor distance", 0, 100, &(HCAAMB.peakNeighborDistance));
	gui->addSlider("MinLaplaceEdgeStrength", 1, 255, &minLaplaceEdgeStrength);
	
	
	
	gui->addSpacer();
	
	float			sampleOffset;
	float			peakAngleCutoff;
	float			peakNeighborDistance;
	
	gui->addLabelToggle("Active", &active);
	gui->addLabelToggle("Intermediate", &intermediate);
	gui->addLabelToggle("Play", &playing);
	gui->addLabelToggle("Adaptive", &bDoAdaptiveThresholding);
	gui->addLabelToggle("LaplacianEdges", &bDoLaplacianEdgeDetect);
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
		
		
		
		
	
		//-----------------------------------------------------------
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
			cv::subtract (gray, adaptiveThreshImg, tempGrayscaleMat1);
			int thresholdMode = cv::THRESH_BINARY; // cv::THRESH_BINARY_INV
			cv::threshold (tempGrayscaleMat1, thresholded, 1, 255, thresholdMode);
		
		} else {
			// If we are not adaptive thresholding, just use a regular threshold. 
			threshold(gray, thresholded, thresholdValue);
		}
		
		
		//-----------------------------------------------------------
		// Compute the Laplacian edges of the gray image, and threshold them.
		if (bDoLaplacianEdgeDetect){
			
			int kSize = 7;
			double delta = 128;
			double sensitivity = 0.01;
			int edgeThreshold = (int) minLaplaceEdgeStrength;
			Laplacian (gray, edgeDetected, -1, kSize, sensitivity, delta, BORDER_DEFAULT );
			cv::threshold (edgeDetected, tempGrayscaleMat1, edgeThreshold, 255, cv::THRESH_BINARY);
			
			// Extract contours of the thresholded blobs (= "edges").
			// Exclusively render these edges into a new (temporary) image, tempGrayscaleMat2
			edgeContourFinder.setMinArea(128);
			edgeContourFinder.findContours(tempGrayscaleMat1);
			tempGrayscaleMat2.setTo( (unsigned char) (255));
			//Not using: goodEdgesImg = Mat::zeros(imgH, imgW, CV_8UC1);
			int nGoodEdges = edgeContourFinder.getContours().size();
			cv::Mat dstMat = toCv(tempGrayscaleMat2);
			for (int i=0; i<nGoodEdges; i++){
				vector <cv::Point> aGoodEdgeContour = edgeContourFinder.getContour(i);
				const cv::Point* ppt[1] = { &(aGoodEdgeContour[0]) };
				int npt[] = { aGoodEdgeContour.size() };
				fillPoly(dstMat, ppt, npt, 1, Scalar(0));//Scalar(255));
			}
			
			// Erode tempGrayscaleMat2 into goodEdgesImg
			cv::Mat structuringElt = Mat();
			cv::erode  (tempGrayscaleMat2, goodEdgesImg, structuringElt);
			
			// Mask ('and') the good edge blobs against the thresholded image. 
			cv::bitwise_and(thresholded, goodEdgesImg, thresholded); 

		}
		
		
		//-----------------------------------------------------------
		// Extract the contour(s) of the binarized image, and FIND THE CONTOURS
		contourFinder.setMinAreaNorm( minAllowableContourAreaAsAPercentOfImageSize );
		contourFinder.setMaxAreaNorm( maxAllowableContourAreaAsAPercentOfImageSize );
		contourFinder.findContours(thresholded);
		
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
void ofApp::draw() {
	if(active) {
		
		ofPushStyle();
		
		ofSetColor(64);
		drawMat(thresholded, 0, 0);
		
		//ofSetColor(255,0,0);
		//contourFinder.draw();
		
		if (bValidHandContourExists){
			HCAAMB.drawAnalytics();
			//if (bValidHandContourExists){ HCAAMB.drawMousePoint (mouseX); }
		}
		ofPopStyle();
		
	} else {
		
		ofPushStyle();
		// drawMat(thresholded, 0,0);
		// drawMat(blurred, 0,0);
		// drawMat(goodEdgesImg, 0,0);
		
		ofSetColor(255);
		video.draw(0, 0);
		
		if (bValidHandContourExists){
			HCAAMB.drawAnalytics();
		}
		
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
		drawMat(goodEdgesImg,		imgW * xItem, 0); xItem++;
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