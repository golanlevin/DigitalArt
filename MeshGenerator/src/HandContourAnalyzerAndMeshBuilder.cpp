//  Created by Golan Levin on 5/19/13.

#include "HandContourAnalyzerAndMeshBuilder.h"


//============================================================
HandContourAnalyzerAndMeshBuilder::HandContourAnalyzerAndMeshBuilder (){
	
	/*
	ofPolyline	handContourRaw;
	ofPolyline	handContour1;
	ofPolyline	handContour2;
	
	
	vector <int>	joints;
	ofMesh			handMesh;
	 */
	
	bCalculatedMesh = false;
	contourResampleSpacing = 2.0;
	contourSmoothingSize   = 20.0;
	
	sampleOffset = 8;
	peakAngleCutoff = 14;
	peakNeighborDistance = 32;
	
	crotchAngleCutoff = -40;
	crotchNeighborDistance = 40;
	
	eigenData.orientation = 0;
	eigenData.eigenValue = 0;
	
	
}

//============================================================
void HandContourAnalyzerAndMeshBuilder::setContourFilterValues(float r, float s){
	contourResampleSpacing = r;
	contourSmoothingSize   = s;
}


//============================================================
void HandContourAnalyzerAndMeshBuilder::process (ofPolyline inputContour, cv::Point2f inputCentroid){
	if (inputContour.size() > 0){
		
		// Clear the main output Mesh, and copy in the new information (contour and centroid). 
		handMesh.clear();
		handContourRaw = inputContour;
		handCentroid.set(inputCentroid.x, inputCentroid.y);
	
		// handContourFiltered is handContourRaw resampled, then smoothed, then resampled.
		handContourResampled= handContourRaw.getResampledBySpacing(2.0);
		handContourNice		= handContourResampled.getSmoothed(5.0);
		handContour1        = handContourResampled.getSmoothed(contourSmoothingSize);
		handContourFiltered = handContour1.getResampledBySpacing(contourResampleSpacing);
		
		// compute handRadius;
		handRadius = computeHandRadius (handContourFiltered);
		
		
		
		//--------------------------------------
		// FIND FINGER TIPS
		// Cache the local curvatures; find peaks. 
		handContourFilteredCurvatures = buildCurvatureAnalysis (handContourFiltered, sampleOffset);
		handContourFilteredTipIndices = findPeaks (handContourFilteredCurvatures, peakAngleCutoff, peakNeighborDistance);
		
		// Store the indices of fingertips on the filtered contour.
		// Only select those > some min distance from centroid.
		// Note that peaks are already sorted in descending order of quality. 
		fingerTipPointsFiltered.clear();
		int maxNPeaksToConsider = 10;
		float minFingerTipDistanceFromCentroid = handRadius * 0.5;
		
		for (int i = 0; i < handContourFilteredTipIndices.size(); i++) {
			int indexOfPossibleFingerTip = handContourFilteredTipIndices[i];
			ofVec2f aPossibleFingerTip = handContourFiltered [ indexOfPossibleFingerTip ];
			float distanceFromCentroid = aPossibleFingerTip.distance(handCentroid);
			if ((distanceFromCentroid > minFingerTipDistanceFromCentroid) &&
				(fingerTipPointsFiltered.size() < maxNPeaksToConsider)){
				fingerTipPointsFiltered.push_back (aPossibleFingerTip);
			}
		}

		
		// Find the points on handContourNice which are closest
		// to the likely fingerTips in handContourFiltered.
		fingerTipPointsTmp.clear();
		fingerTipIndicesTmp.clear();
		for (int i=0; i<fingerTipPointsFiltered.size(); i++){
			
			bool bStashInterpolatedPoints = false;
			if (bStashInterpolatedPoints){
				// was nice, but is no longer used. 
				ofPoint closestOriginal = handContourNice.getClosestPoint( fingerTipPointsFiltered[i] );
				fingerTipPointsTmp.push_back (closestOriginal);
				
			} else {
				// we store original, not interpolated points, because we need the indices.
				int indexOfClosest = 0;
				float minDistance = 99999;
				ofVec2f winningFilteredPoint = fingerTipPointsFiltered[i];
				
				for (int j=0; j<handContourNice.size(); j++){
					ofVec2f pointOnNiceContour = handContourNice[j];
					float ijDist = ofDist(winningFilteredPoint.x, winningFilteredPoint.y,
										  pointOnNiceContour.x, pointOnNiceContour.y);
					if (ijDist < minDistance){
						minDistance = ijDist;
						indexOfClosest = j;
					}
				}
				fingerTipPointsTmp.push_back ( handContourNice[indexOfClosest] );
				fingerTipIndicesTmp.push_back( indexOfClosest);
			}
		}
		
		
		// Sort the fingerTipIndicesTmp by the angle around the centroid.
		vector< pair<float, int> > fingertipAngleIndexPairs;
		for (int i=0; i<fingerTipIndicesTmp.size(); i++){
			int indexInHandContourNice = fingerTipIndicesTmp[i];
			
			ofVec2f a = handContourNice [indexInHandContourNice];
			ofVec2f b = handCentroid;
			ofVec2f c = ofVec2f(100, 0);
			a -= b;
			float angle = a.angle(c);
			angle = (angle < 0)? angle + 360: angle;
			// push pairs of (angle, index):
			fingertipAngleIndexPairs.push_back (pair<float, int> (angle, indexInHandContourNice));
		}
		ofSort (fingertipAngleIndexPairs);
		 
		// reshuffle the fingerTipIndicesTmp based on that sorting. 
		fingerTipIndicesTmp.clear();
		for (int i=0; i<fingertipAngleIndexPairs.size(); i++){
			int indexOfSortedFingertip = fingertipAngleIndexPairs[i].second;
			fingerTipIndicesTmp.push_back (indexOfSortedFingertip);
		}
		
				
		
		//--------------------------------------
		// FIND FINGER CROTCHES
		handContourCurvatures = buildCurvatureAnalysis (handContourNice, sampleOffset);
		handContourPossibleCrotchIndices = findCrotches (handContourCurvatures, crotchAngleCutoff, crotchNeighborDistance);
		
		fingerCrotchPointsTmp.clear();
		handContourCrotchIndicesTmp.clear();
		int maxNCrotchesToConsider = 4;
		float maxCrotchDistanceFromCentroid = handRadius * 0.666;
		
		// Filter the possible crotches by quantity, distance from the centroid
		for (int i = 0; i < handContourPossibleCrotchIndices.size(); i++) {
			int indexOfPossibleCrotch = handContourPossibleCrotchIndices[i];
			ofVec2f aPossibleCrotch = handContourNice [ indexOfPossibleCrotch ];
			float distanceFromCentroid = aPossibleCrotch.distance(handCentroid);
			if ((distanceFromCentroid < maxCrotchDistanceFromCentroid) &&
				(fingerCrotchPointsTmp.size() < maxNCrotchesToConsider)){
				 fingerCrotchPointsTmp.push_back       (aPossibleCrotch);
				 handContourCrotchIndicesTmp.push_back (indexOfPossibleCrotch);
			}
		}
		
		
		//--------------------------------------
		// IDENTIFY WHICH IS THE THUMB CROTCH.
		// SORT THE CROTCHES BY THEIR POSITION ALONG A LINE
	
		
		// http://faculty.cs.niu.edu/~hutchins/csci230/best-fit.htm
		// find the line that approximates the crotches.
		// Formula Y = Slope * X + YInt
		float SumX = 0;  // sum of all the X values
		float SumY = 0;  //sum of all the Y values
		float SumX2 = 0; //sum of the squares of the X values
		float SumXY = 0; //sum of the products X*Y for all the points
		int Count = maxNCrotchesToConsider;
		for (int i=0; i<Count; i++){
			ofVec2f pointi = fingerCrotchPointsTmp[i];
			SumX  += pointi.x;
			SumY  += pointi.y;
			SumX2 += (pointi.x * pointi.x);
			SumXY += (pointi.x * pointi.y);
		}
		float XMean = SumX / (float) Count;
		float YMean = SumY / (float) Count;
		float Slope = (SumXY - SumX * YMean) / (SumX2 - SumX * XMean);
		float YInt  = YMean - Slope * XMean;
		crotchLineSlope     = Slope;
		crotchLineIntercept = YInt;
		
		//--------
		// Compute the coordinates of the line which fits the four crotches
		float x1 = -99999;
		float x2 =  99999;
		for (int i=0; i<fingerCrotchPointsTmp.size(); i++){
			float ftx = fingerCrotchPointsTmp[i].x;
			if (ftx > x1){ x1 = ftx; }
			if (ftx < x2){ x2 = ftx; }
		}
		float y1 = crotchLineSlope * x1 + crotchLineIntercept;
		float y2 = crotchLineSlope * x2 + crotchLineIntercept;
		float dx = x2 - x1;
		float dy = y2 - y1;
		float LineMag2 = dx*dx + dy*dy;
		
		//--------
		// Using the code to find the nearest point on a line,
		// find the parametrization u for each point on that line.
		// http://paulbourke.net/geometry/pointlineplane/
		// Then we will sort them by this parameter u.
		vector< pair<float, int> > crotchParametrizationIndexPairs;
		for (int i=0; i<fingerCrotchPointsTmp.size(); i++){
			float x3  = fingerCrotchPointsTmp[i].x;
			float y3  = fingerCrotchPointsTmp[i].y;
			float  u  = ((x3-x1)*(x2-x1) + (y3-y1)*(y2-y1)) / LineMag2;
			// push pairs of (parametrizations, index): 
			int indexInHandContourNice = handContourCrotchIndicesTmp[i];
			crotchParametrizationIndexPairs.push_back (pair<float, int> (u, indexInHandContourNice));
		}
		ofSort (crotchParametrizationIndexPairs);
		
		
		fingerCrotchPoints.clear();
		handContourCrotchIndicesSorted.clear();
		for (int i=0; i<crotchParametrizationIndexPairs.size(); i++){
			int indexOfSortedCrotch = crotchParametrizationIndexPairs[i].second;
			ofVec2f aSortedCrotch = handContourNice [ indexOfSortedCrotch ];
			handContourCrotchIndicesSorted.push_back (indexOfSortedCrotch);
			fingerCrotchPoints.push_back (aSortedCrotch);
		}
		
		// score the crotches by their distance to the centroid, and to each other.
		// The highest score is most likely the thumb.
		float scores[maxNCrotchesToConsider];
		for (int i=0; i<maxNCrotchesToConsider; i++){ scores[i] = 1.0; }
		for (int i=0; i<maxNCrotchesToConsider; i++){
			ofVec2f pointi = fingerCrotchPoints[i];
			float icDist = ofDist(pointi.x, pointi.y, handCentroid.x, handCentroid.y);
			scores[i] *= icDist / handRadius;
			for (int j=0; j<maxNCrotchesToConsider; j++){
				if (j != i){
					ofVec2f pointj = fingerCrotchPoints[j];
					float ijDist = ofDist(pointi.x, pointi.y,  pointj.x, pointj.y);
					scores[i] *= ijDist / handRadius;
				}
			}
		}
		// in fingerCrotchPoints, which one (of the four, 0,1,2,3, is the thumb?
		int indexOfThumbCrotch = 0;
		float maxScore = 0;
		for (int i=0; i< maxNCrotchesToConsider; i++){
			if (scores[i] > maxScore){
				maxScore = scores[i];
				indexOfThumbCrotch = i;
			}
		}
		
		
		
		// totally temp!!:
		fingerTipPoints.clear();
		for (int i=0; i<fingerTipIndicesTmp.size(); i++){
			int indexOfSortedFingertip = fingerTipIndicesTmp[i]; //fingertipAngleIndexPairs[i].second;
			ofVec2f aSortedFingertip = handContourNice [ indexOfSortedFingertip ];
			fingerTipPoints.push_back (aSortedFingertip);
		}
		
		

		
		
		
		//--------------------------------------
		// FIND THE POINTER & PINKY SIDES -- BY FINDING THE ORIENTATION OF THE HAND
		float handOrientation = getOrientation (handContourNice.getVertices(), handCentroid);
		
		float dq = handRadius * 0.10;
		float qx = handCentroid.x + dq * sinf(handOrientation);
		float qy = handCentroid.y + dq * cosf(handOrientation);
		
		float sp1x = qx - handRadius * cosf(handOrientation);
		float sp1y = qy + handRadius * sinf(handOrientation);
		float sp2x = qx + handRadius * cosf(handOrientation);
		float sp2y = qy - handRadius * sinf(handOrientation);
		sideLineP1.set(sp1x, sp1y);
		sideLineP2.set(sp2x, sp2y);
		

	
		// Search for the pinky side.
		if (indexOfThumbCrotch == 3){ // right hand
			int contourIndexOfPinkyTip   = fingerTipIndicesTmp[0];
			int contourIndexOfPinkysideWrist = fingerTipIndicesTmp[ fingerTipIndicesTmp.size() - 1];
			
			if (contourIndexOfPinkysideWrist < contourIndexOfPinkyTip){
				
			
			}
			// search for the point on the contour
			// between contourIndexOfPinkyTip and contourIndexOfPinkysideWrist
			// which is closest to the sideLine. 
		
		} else {
			// handle the left hand another day.
		
		}
		
		
		
		
		//--------------------------------------
		// ASSEMBLE HANDMARKS
		// Assumes indexOfThumbCrotch == 3 ! >> RIGHT HANDS ONLY FOR NOW
		// Assume nFingerTipIndices >= 7!
		
		int nFingerTipIndices = fingerTipIndicesTmp.size();
		int contourIndexOfPinkyTip   = fingerTipIndicesTmp[0];
		int contourIndexOfRingTip    = fingerTipIndicesTmp[1];
		int contourIndexOfMiddleTip  = fingerTipIndicesTmp[2];
		int contourIndexOfPointerTip = fingerTipIndicesTmp[3];
		int contourIndexOfThumbTip   = fingerTipIndicesTmp[4];
		
		int contourIndexOfPRCrotch	 = handContourCrotchIndicesSorted[0];
		int contourIndexOfRMCrotch	 = handContourCrotchIndicesSorted[1];
		int contourIndexOfMICrotch	 = handContourCrotchIndicesSorted[2];
		int contourIndexOfITCrotch	 = handContourCrotchIndicesSorted[3];
		
		int contourIndexOfPinkysideWrist = fingerTipIndicesTmp[nFingerTipIndices - 1];
		int contourIndexOfThumbsideWrist = fingerTipIndicesTmp[nFingerTipIndices - 2];
		
		if (indexOfThumbCrotch != 3){
			; // then we have a left hand; deal with it later. // reverse everything
		} 
	
		Handmarks[HANDMARK_PINKY_TIP].index			= contourIndexOfPinkyTip;
		Handmarks[HANDMARK_PINKY_TIP].point			= handContourNice [contourIndexOfPinkyTip];
		Handmarks[HANDMARK_PINKY_TIP].type			= HANDMARK_PINKY_TIP;
		
		Handmarks[HANDMARK_PR_CROTCH].index			= contourIndexOfPRCrotch;
		Handmarks[HANDMARK_PR_CROTCH].point			= handContourNice [contourIndexOfPRCrotch];
		Handmarks[HANDMARK_PR_CROTCH].type			= HANDMARK_PR_CROTCH;
		
		Handmarks[HANDMARK_RING_TIP].index			= contourIndexOfRingTip;
		Handmarks[HANDMARK_RING_TIP].point			= handContourNice [contourIndexOfRingTip];
		Handmarks[HANDMARK_RING_TIP].type			= HANDMARK_RING_TIP;
		
		Handmarks[HANDMARK_RM_CROTCH].index			= contourIndexOfRMCrotch;
		Handmarks[HANDMARK_RM_CROTCH].point			= handContourNice [contourIndexOfRMCrotch];
		Handmarks[HANDMARK_RM_CROTCH].type			= HANDMARK_RM_CROTCH;
		
		Handmarks[HANDMARK_MIDDLE_TIP].index		= contourIndexOfMiddleTip;
		Handmarks[HANDMARK_MIDDLE_TIP].point		= handContourNice [contourIndexOfMiddleTip];
		Handmarks[HANDMARK_MIDDLE_TIP].type			= HANDMARK_MIDDLE_TIP;
		
		Handmarks[HANDMARK_MI_CROTCH].index			= contourIndexOfMICrotch;
		Handmarks[HANDMARK_MI_CROTCH].point			= handContourNice [contourIndexOfMICrotch];
		Handmarks[HANDMARK_MI_CROTCH].type			= HANDMARK_MI_CROTCH;
		
		Handmarks[HANDMARK_POINTER_TIP].index		= contourIndexOfPointerTip;
		Handmarks[HANDMARK_POINTER_TIP].point		= handContourNice [contourIndexOfPointerTip];
		Handmarks[HANDMARK_POINTER_TIP].type		= HANDMARK_POINTER_TIP;
		
		Handmarks[HANDMARK_POINTER_SIDE].type		= HANDMARK_INVALID;
		
		Handmarks[HANDMARK_IT_CROTCH].index			= contourIndexOfITCrotch;
		Handmarks[HANDMARK_IT_CROTCH].point			= handContourNice [contourIndexOfITCrotch];
		Handmarks[HANDMARK_IT_CROTCH].type			= HANDMARK_IT_CROTCH;
		
		Handmarks[HANDMARK_THUMB_TIP].index			= contourIndexOfThumbTip;
		Handmarks[HANDMARK_THUMB_TIP].point			= handContourNice [contourIndexOfThumbTip];
		Handmarks[HANDMARK_THUMB_TIP].type			= HANDMARK_THUMB_TIP;
		
		Handmarks[HANDMARK_THUMB_BASE].type			= HANDMARK_INVALID;
		
		Handmarks[HANDMARK_THUMBSIDE_WRIST].index	= contourIndexOfThumbsideWrist;
		Handmarks[HANDMARK_THUMBSIDE_WRIST].point	= handContourNice [contourIndexOfThumbsideWrist];
		Handmarks[HANDMARK_THUMBSIDE_WRIST].type	= HANDMARK_THUMBSIDE_WRIST;
		
		Handmarks[HANDMARK_PINKYSIDE_WRIST].index	= contourIndexOfPinkysideWrist;
		Handmarks[HANDMARK_PINKYSIDE_WRIST].point	= handContourNice [contourIndexOfPinkysideWrist];
		Handmarks[HANDMARK_PINKYSIDE_WRIST].type	= HANDMARK_PINKYSIDE_WRIST;
		
		Handmarks[HANDMARK_PINKY_SIDE].type			= HANDMARK_INVALID;
		
	
		
		
		
		
		//--------------------------------------
		// bCalculatedMesh = true;
	}
}



//============================================================
float HandContourAnalyzerAndMeshBuilder::computeHandRadius (ofPolyline aPolyline){
	float maxRadiusSquared = 0;
	for (int i=0; i< aPolyline.size(); i++){
		float dx = handCentroid.x - aPolyline[i].x;
		float dy = handCentroid.y - aPolyline[i].y;
		float dh2 = dx*dx + dy*dy;
		if (dh2 > maxRadiusSquared){
			maxRadiusSquared = dh2; 
		}
	}
	return sqrt(maxRadiusSquared); 
}

//============================================================
void HandContourAnalyzerAndMeshBuilder::drawAnalytics(){
	ofPushStyle();
	ofSetLineWidth(1.0);
	
	ofSetColor(0,150,0);
	handContourNice.draw();
	
	ofFill();
	ofSetColor(50,200,0);
	for (int i=0; i<fingerTipPoints.size(); i++){
		float ftx = fingerTipPoints[i].x;
		float fty = fingerTipPoints[i].y;
		ofEllipse(ftx, fty, 9,9);
		ofDrawBitmapString( ofToString(i), ftx+8, fty+10); 
	}
	ofSetColor(0,150,255);
	for (int i=0; i<fingerCrotchPoints.size(); i++){
		float ftx = fingerCrotchPoints[i].x;
		float fty = fingerCrotchPoints[i].y;
		ofEllipse(ftx, fty, 9,9);
		ofDrawBitmapString( ofToString(i), ftx+8, fty+10); 
	}
	
	// ----------------
	// DRAW HANDMARKS!
	ofNoFill();
	ofSetColor(255,0,255);
	ofBeginShape();
	for (int i=0; i<N_HANDMARKS; i++){
		if (Handmarks[i].type != HANDMARK_INVALID){
			float hx = Handmarks[i].point.x;
			float hy = Handmarks[i].point.y;
			// ofEllipse(hx, hy, 30,30);
			ofVertex(hx,hy); 
		}
	}
	ofEndShape(OF_CLOSE);
	
	
	
	ofSetColor(0,100,200);
	float minx =  99999;
	float maxx = -99999;
	for (int i=0; i<fingerCrotchPointsTmp.size(); i++){
		float ftx = fingerCrotchPointsTmp[i].x;
		if (ftx < minx){ minx = ftx; }
		if (ftx > maxx){ maxx = ftx; }
	}
	float y0 = crotchLineSlope * minx + crotchLineIntercept;
	float y1 = crotchLineSlope * maxx + crotchLineIntercept;
	ofLine(minx,y0, maxx, y1); 
    ofLine(sideLineP1, sideLineP2);
	
	
	
	
	
	ofNoFill();
	ofSetColor(255,100,0);
	handContourFiltered.draw();

	
	float cx = handCentroid.x;
	float cy = handCentroid.y;
	ofNoFill();
	ofEllipse(cx, cy, 10,10);
	ofEllipse(cx, cy, handRadius*2, handRadius*2);
	
	ofSetColor(255,255,0);
	float orientation = eigenData.orientation;
	float ox = 100 * sinf(orientation);
	float oy = 100 * cosf(orientation);
	ofLine (cx,cy, cx+ox, cy+oy); 
	/*
	ofFill();
	for (int i=0; i<fingerTipPointsFiltered.size(); i++){
		float ftx = fingerTipPointsFiltered[i].x;
		float fty = fingerTipPointsFiltered[i].y;
		ofEllipse(ftx, fty, 9,9);
	}
	*/
	
	
	ofPopStyle();
	
}



//============================================================
// From Kyle McDonald, Digital Interaction
vector<float> HandContourAnalyzerAndMeshBuilder::buildCurvatureAnalysis (ofPolyline& polyline, int offset) {
	int n = polyline.size();
	if(offset > n) {
		offset = n;
	}
	vector<float> curvature(n);
	for(int i = 0; i < n; i++) {
		int left = i - offset;
		if(left < 0) {
			left += n;
		}
		int right = i + offset;
		if(right >= n) {
			right -= n;
		}
		ofVec2f a = polyline[left], b = polyline[i], c = polyline[right];
		a -= b;
		c -= b;
		float angle = a.angle(c);
		curvature[i] = -(angle > 0 ? angle - 180 : angle + 180);
	}
	return curvature;
}

//============================================================
// From Kyle McDonald, Digital Interaction
vector<int> HandContourAnalyzerAndMeshBuilder::findPeaks (vector<float>& values, float cutoff, int peakArea) {
	vector< pair<float, int> > peaks;
	int n = values.size();
	for(int i = 1; i < n - 1; i++) {
		if(values[i] > cutoff) {
			peaks.push_back(pair<float, int>(-values[i], i));
		}
	}
	ofSort(peaks);
	vector<int> indices;
	for(int i = 0; i < peaks.size(); i++) {
		int curIndex = peaks[i].second;
		
		bool bHasNeighbor = false;
		for (int j = 0; j < indices.size(); j++) {
			int small = MIN(indices[j], curIndex);
			int big   = MAX(indices[j], curIndex);
			int diff  = big - small;
			if ((diff < peakArea) || (diff > n - peakArea)) {
				bHasNeighbor = true;
				break;
			}
		}
		
		if(!bHasNeighbor) {
			indices.push_back(curIndex);
		}

	}
	return indices;
}




//============================================================
vector<int> HandContourAnalyzerAndMeshBuilder::findCrotches (vector<float>& values, float cutoff, int crotchArea) {
    // cutoff ~= -40.0; 
 
	vector< pair<float, int> > crotches;
	int n = values.size();
	for (int i=1; i < (n-1); i++) {
		if(values[i] < cutoff) {
			crotches.push_back(pair<float, int>(values[i], i));
		}
	}
	ofSort(crotches);
	
	vector<int> indices;
	for(int i = 0; i < crotches.size(); i++) {
		int curIndex = crotches[i].second;
		
		bool bHasNeighbor = false;
		for (int j = 0; j < indices.size(); j++) {
			int small = MIN(indices[j], curIndex);
			int big   = MAX(indices[j], curIndex);
			int diff  = big - small;
			if ((diff < crotchArea) || (diff > n - crotchArea)) {
				bHasNeighbor = true;
				break;
			}
		}
		 
		if(!bHasNeighbor) {
			indices.push_back(curIndex);
		} 
	}
	
	return indices;
}






//============================================================
void HandContourAnalyzerAndMeshBuilder::draw (){
	draw (true, true);
}
void HandContourAnalyzerAndMeshBuilder::draw (bool bDrawWireframe, bool bDrawJoints){
	if (bDrawWireframe){
		drawMeshWireframe();
	}
	if (bDrawJoints){
		drawJoints();
	}
}





//============================================================
void HandContourAnalyzerAndMeshBuilder::drawMeshWireframe(){
	ofPushStyle();
	
	ofPopStyle();
}
//============================================================
void HandContourAnalyzerAndMeshBuilder::drawJoints(){
	ofPushStyle();
	
	ofPopStyle();
}

//============================================================
void HandContourAnalyzerAndMeshBuilder::drawMousePoint (float mx){
	int nCurvatures = handContourCurvatures.size();
	if (nCurvatures > 0){
		int whichCurvatureIndex = ((int)(mx))%nCurvatures;
		ofVec2f aPoint = handContourResampled [ whichCurvatureIndex ];
		
		ofNoFill();
		ofSetColor(0,160,255);
		ofEllipse(aPoint.x, aPoint.y, 15,15);
		ofDrawBitmapString( ofToString( whichCurvatureIndex ), aPoint.x+20, aPoint.y-10);
	}
}





//============================================================
float HandContourAnalyzerAndMeshBuilder::getOrientation (vector<ofPoint> pts, ofVec2f COM) {
	
	float orientation  = 0.0;
	float orientedness = 0.0;
	
		
	int nPoints = pts.size();
	if (nPoints > 2) {
		
		//arguments: an array of pixels, the array's width & height, and the location of the center of mass (com).
		//this function calculates the elements of a point set's tensor matrix,
		//calls the function calcEigenvector() to get the best eigenvector of this matrix
		//and returns this eigenVector as a pair of doubles
		
		//first we look at all the pixels, determine which ones contribute mass (the black ones),
		// and accumulate the sums for the tensor matrix
		float dX, dY;
		float XXsum, YYsum, XYsum;
		
		XXsum = 0;
		YYsum = 0;
		XYsum = 0;
		
		for (int j=0; j<nPoints; j++) {
			ofVec2f pt = pts[j];
			dX = pt.x - COM.x;
			dY = pt.y - COM.y;
			XXsum += dX * dX;
			YYsum += dY * dY;
			XYsum += dX * dY;
		}
		
		// here's the tensor matrix.
		// watch out for memory leaks.
		// float matrix2x2[][] = new float[2][2];
		float matrix2x2_00 =  YYsum;
		float matrix2x2_01 = -XYsum;
		float matrix2x2_10 = -XYsum;
		float matrix2x2_11 =  XXsum;
		
		// get the orientation of the bounding box
		calcEigenvector ( matrix2x2_00, matrix2x2_01, matrix2x2_10, matrix2x2_11 );
		orientation  = eigenData.orientation; //response[0];
		orientedness = eigenData.eigenValue;  //response[1];
	}
	
	// orientedness is calculated but not returned.
	return orientation;
}

//============================================================
void HandContourAnalyzerAndMeshBuilder::calcEigenvector (float matrix_00, float matrix_01,
														 float matrix_10, float matrix_11 ) {
	
	//this function takes a 2x2 matrix, and returns a pair of angles which are the eigenvectors
	float A = matrix_00;
	float B = matrix_01;
	float C = matrix_10;
	float D = matrix_11;
	
	// float multiPartData[] = new float[2]; // watch out for memory leaks.
	
	//because we assume a 2x2 matrix,
	//we can solve explicitly for the eigenValues using the Quadratic formula.
	//the eigenvalues are the roots of the equation  det( lambda * I  - T) = 0
	float a, b, c, root1, root2;
	a = 1.0;
	b = (0.0 - A) - D;
	c = (A * D) - (B * C);
	float Q = (b * b) - (4.0 * a * c);
	if (Q >= 0) {
		root1 = ((0.0 - b) + sqrt ( Q)) / (2.0 * a);
		root2 = ((0.0 - b) - sqrt ( Q)) / (2.0 * a);
		
		//assume x1 and x2 are the elements of the eigenvector.  Then, because Ax1 + Bx2 = lambda * x1,
		//we know that x2 = x1 * (lambda - A) / B.
		float factor2 = ( min (root1, root2) - A) / B;
		
		//we arbitrarily set x1 = 1.0 and compute the magnitude of the eigenVector with respect to this assumption
		float magnitude2 = sqrtf (1.0 + factor2*factor2);
		
		//we now find the exact components of the eigenVector by scaling by 1/magnitude
		if ((magnitude2 == 0)  || isnan(magnitude2) || isinf(magnitude2) ){
			eigenData.orientation = 0;
			eigenData.eigenValue  = 0; 
		}
		else {
			float orientedBoxOrientation = atan2f ( (1.0 / magnitude2), (factor2 / magnitude2));
			float orientedBoxEigenvalue  = logf (1.0+root2); // orientedness
			eigenData.orientation = orientedBoxOrientation;
			eigenData.eigenValue  = orientedBoxEigenvalue;
		}
	}
	else {
		eigenData.orientation = 0;
		eigenData.eigenValue  = 0;
	}
	
	// return eigenData;
}





//============================================================
ofMesh& HandContourAnalyzerAndMeshBuilder::getMesh(){
	return handMesh;
}

//============================================================
vector<int>& HandContourAnalyzerAndMeshBuilder::getJoints(){
	return joints;
}
	
