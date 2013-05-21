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
	void		setContourFilterValues(float r, float s);
	
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
	
	vector<ofVec2f> fingerTipPointsFiltered;
	vector<ofVec2f> fingerTipPoints; // on handContourNice
	
	vector<float>	handContourCurvatures;
	vector<int>		handContourPossibleCrotchIndices;
	vector<int>		handContourCrotchIndicesTmp;
	vector<int>		handContourCrotchIndicesSorted;
	
	vector<ofVec2f> fingerCrotchPointsTmp;
	vector<ofVec2f> fingerCrotchPoints; // on handContourNice
	
	float crotchLineSlope;
	float crotchLineIntercept;
	
	
	void drawMousePoint (float mx);
	
	
	
	eigenMultiPartData	eigenData;
	float getOrientation (vector<ofPoint> pts, ofVec2f COM);
	void  calcEigenvector (float matrix_00, float matrix_01,
						   float matrix_10, float matrix_11 );

private:
	vector <int>	joints;
	ofMesh			handMesh;
	bool			bCalculatedMesh;
	
	
	
	
	
	void drawMeshWireframe();
	void drawJoints();
	
};