//
//  HandContourAnalyzerAndMeshBuilder.h
//  ofApp
//
//  Created by Golan Levin on 5/19/13.
//
//

#pragma once

#include "ofMain.h"
#include "ofxCv.h"


struct eigenMultiPartData {
	float orientation; 
	float eigenValue;
};

enum HandType {
	HAND_ERROR = -1,
	HAND_NONE  =  0,
	HAND_RIGHT =  1,
	HAND_LEFT  =  2
};

#define N_HANDMARKS 16
#define DESIRED_N_CONTOUR_POINTS 1250

enum HandmarkType {
	HANDMARK_INVALID			= -1,
	HANDMARK_PINKY_TIP			= 0,
	HANDMARK_PR_CROTCH			= 1,
	HANDMARK_RING_TIP			= 2,
	HANDMARK_RM_CROTCH			= 3,
	HANDMARK_MIDDLE_TIP			= 4,
	HANDMARK_MI_CROTCH			= 5,
	HANDMARK_POINTER_TIP		= 6,
	HANDMARK_POINTER_SIDE		= 7,
	HANDMARK_IT_CROTCH			= 8,
	HANDMARK_THUMB_TIP			= 9,
	HANDMARK_THUMB_KNUCKLE		= 10,
	HANDMARK_THUMB_BASE			= 11,
	HANDMARK_THUMBSIDE_WRIST	= 12,
	HANDMARK_PINKYSIDE_WRIST	= 13,
	HANDMARK_PALM_BASE			= 14,
	HANDMARK_PINKY_SIDE			= 15
};

struct Handmark { // analogous to landmark
	ofVec2f			point;			// the actual location
	ofVec2f			pointPrev;		// its previous location
	int				index;			// the index in the handContourNice
	HandmarkType	type;			// the name (or type) of the point
	float			confidence;		// a score for the Handmark's stability, 0...1
};




class HandContourAnalyzerAndMeshBuilder {
public:
	
	HandContourAnalyzerAndMeshBuilder();
	
	void process (ofPolyline inputContour, cv::Point2f inputCentroid);
	void draw ();
	void draw (bool bDrawWireframe, bool bDrawJoints);
	void drawAnalytics();
	
	ofMesh		&getMesh();
	vector<int> &getJoints();
	bool		isCalculated();
	HandType	getHandType(); 
	
	vector<float> buildCurvatureAnalysis (ofPolyline& polyline, int offset);
	vector<int>   findPeaks (vector<float>& values, float cutoff, int peakArea);
	vector<int>   findCrotches (vector<float>& values, float cutoff, int crotchArea);
	float computeHandRadius (ofPolyline aPolyline);
	
	
	ofVec2f 	handCentroid;
	ofPolyline	handContourRaw;
	ofPolyline	handContourResampled;
	ofPolyline	handContourNice;
	ofPolyline	handContourFiltered;
	ofPolyline	handContour1;
	float		handRadius; 
	

	// parameters, knobs:
	float			contourResampleSpacing;
	float			contourSmoothingSize;
	float			sampleOffset;
	float			peakAngleCutoff;
	float			peakNeighborDistance;
	float			crotchAngleCutoff;
	float			crotchNeighborDistance;
	
	
	vector<float>	handContourFilteredCurvatures;
	vector<int>		handContourFilteredTipIndices;
	vector<int>		handContourFingerTipIndicesSorted;
	
	vector<ofVec2f> fingerTipPointsFiltered;
	vector<ofVec2f> fingerTipPointsTmp; // on handContourNice
	vector<ofVec2f> fingerTipPoints; // on handContourNice
	vector<int>		fingerTipContourIndices; // on handContourNice
	
	vector<float>	handContourNiceCurvatures; // from "nice" (accurate) contour
	vector<int>		handContourPossibleCrotchIndices;
	vector<int>		handContourCrotchIndicesTmp;
	vector<int>		handContourCrotchIndicesSorted;
	
	vector<ofVec2f> fingerCrotchPointsTmp;
	vector<ofVec2f> fingerCrotchPoints; // on handContourNice
	
	float crotchLineSlope;
	float crotchLineIntercept;
	
	ofVec2f sideLineP1;
	ofVec2f sideLineP2;
	
	
	void drawMousePoint (float mx);
	
	Handmark		Handmarks[N_HANDMARKS];
	
	eigenMultiPartData	eigenData;
	float getOrientation (vector<ofPoint> pts, ofVec2f COM);
	void  calcEigenvector (float matrix_00, float matrix_01,
						   float matrix_10, float matrix_11 );

private:
	vector <int>	joints;
	ofMesh			handMesh;
	bool			bCalculatedMesh;
	
	
	void		prepareContours (ofPolyline inputContour, cv::Point2f inputCentroid);
	void		locateFingerTips();
	void		locateFingerCrotches();
	void		identifyThumbCrotchAndHandType();
	void		computeHandOrientationAndSideLine();
	void		locatePinkySide();
	void		locatePointerSide();
	
	int			locateContourFeature(int startFeatureIndex,
									 int endFeatureIndex,
									 float startFeatureIndexOffsetPercent,
									 float endFeatureIndexInsetPercent,
									 bool bConcave);
	void		locateThumbBase();
	void		locatePalmBase();
	void		locateThumbKnuckle();
	void		assembleHandmarks();
	
	float		distanceFromPointToLine (ofVec2f linePt1, ofVec2f linePt2,  ofVec2f aPoint);
	
	int			maxNCrotchesToConsider;
	int			contourIndexOfPinkySide;
	int			contourIndexOfPointerSide;
	int			contourIndexOfThumbBase;
	int			contourIndexOfPalmBase;
	int			contourIndexOfThumbKnuckle; 
	
	HandType	currentHandType;

	
	void drawMeshWireframe();
	void drawJoints();
	
};