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
	float computeHandRadius (ofPolyline aPolyline);
	
	
	ofVec2f 	handCentroid;
	ofPolyline	handContourRaw;
	ofPolyline	handContourResampled;
	ofPolyline	handContourFiltered;
	ofPolyline	handContour1;
	float		handRadius; 
	

	// parameters, knobs:
	float			contourResampleSpacing;
	float			contourSmoothingSize;
	float			sampleOffset;
	float			peakAngleCutoff;
	float			peakNeighborDistance;
	
	
	vector<float>	handContourFilteredCurvatures;
	vector<int>		handContourFilteredTipIndices;
	vector<ofVec2f> fingerTipPointsFiltered;
	vector<ofVec2f> fingerTipPoints; // on handContourResampled
	


private:
	vector <int>	joints;
	ofMesh			handMesh;
	bool			bCalculatedMesh;
	
	
	
	void drawMeshWireframe();
	void drawJoints();
	
};