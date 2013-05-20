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
	
	
}

//============================================================
void HandContourAnalyzerAndMeshBuilder::setContourFilterValues(float r, float s){
	contourResampleSpacing = r;
	contourSmoothingSize   = s;
}


//============================================================
void HandContourAnalyzerAndMeshBuilder::process (ofPolyline inputContour, cv::Point2f inputCentroid){
	if (inputContour.size() > 0){
		
		handMesh.clear();
		handContourRaw = inputContour;
		handCentroid.set(inputCentroid.x, inputCentroid.y);
	
		// handContourFiltered is handContourRaw resampled, then smoothed, then resampled.
		handContourResampled= handContourRaw.getResampledBySpacing(2.0);
		handContour1        = handContourResampled.getSmoothed(contourSmoothingSize);
		handContourFiltered = handContour1.getResampledBySpacing(contourResampleSpacing);
		
		// compute handRadius;
		handRadius = computeHandRadius (handContourFiltered);
		float minFingerTipDistanceFromCentroid = handRadius * 0.5; 
		
		// Cache the local curvatures; find peaks. 
		handContourFilteredCurvatures = buildCurvatureAnalysis (handContourFiltered, sampleOffset);
		handContourFilteredTipIndices = findPeaks (handContourFilteredCurvatures, peakAngleCutoff, peakNeighborDistance);
		
		// Store the indices of fingertips on the filtered contour.
		// Only select those > some min distance from centroid.
		// Note that peaks are already sorted in descending order of quality. 
		fingerTipPointsFiltered.clear();
		int maxNPeaksToConsider = 10;
		for (int i = 0; i < handContourFilteredTipIndices.size(); i++) {
			int indexOfPossibleFingerTip = handContourFilteredTipIndices[i];
			ofVec2f aPossibleFingerTip = handContourFiltered [ indexOfPossibleFingerTip ];
			float distanceFromCentroid = aPossibleFingerTip.distance(handCentroid);
			if ((distanceFromCentroid > minFingerTipDistanceFromCentroid) &&
				(fingerTipPointsFiltered.size() < maxNPeaksToConsider)){
				fingerTipPointsFiltered.push_back (aPossibleFingerTip);
			}
		}

		// Find the points on handContourResampled which are closest
		// to the likely fingerTips in handContourFiltered.
		fingerTipPoints.clear();
		for (int i=0; i<fingerTipPointsFiltered.size(); i++){
			
			ofPoint closestOriginal = handContourResampled.getClosestPoint( fingerTipPointsFiltered[i] );
			fingerTipPoints.push_back (closestOriginal);
			
			/*
			float closestDistance2 = 99999;
			float indexOnHandContourResampledOfClosest = 0;
			
			for (int j=0; j<handContourResampled.size(); j++){
				float jx = handContourResampled[j].x;
				float jy = handContourResampled[j].y;
				
				float dist2 = ofDistSquared(ix,iy, jx,jy);
				if (dist2 < closestDistance2){
					closestDistance2 = dist2;
					indexOnHandContourResampledOfClosest = j;
				}
			}
			 */
			
			/*
			int nNeighborSpanToSearch = 12;
			int nRes = handContourResampled.size(); 
			int mid   = indexOnHandContourResampledOfClosest; 
			float longestDistance2FromCentroid = 0;
			int   indexOfNeighborMostDistalFromCentroid = 0;
			for (int j=0; j<nNeighborSpanToSearch; j++){
				int searchIndex = (mid + j - (nNeighborSpanToSearch/2) + nRes)%nRes;
				float jx = handContourResampled[searchIndex].x;
				float jy = handContourResampled[searchIndex].x;
				float jh = ofDistSquared(jx,jy, handCentroid.x,handCentroid.y);
				if (jh > longestDistance2FromCentroid){
					longestDistance2FromCentroid = jh;
					indexOfNeighborMostDistalFromCentroid = searchIndex;
				}
			}
			 */
		
			
			//ofVec2f aFingerTip = handContourResampled [ indexOfNeighborMostDistalFromCentroid ];
			//fingerTipPoints.push_back (aFingerTip);
			
			//ofVec2f aFingerTip = handContourResampled [ indexOnHandContourResampledOfClosest ];
			//fingerTipPoints.push_back (aFingerTip);
		}
		
		
		
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
	
	ofSetColor(50,200,0);
	handContourResampled.draw();
	ofFill();
	for (int i=0; i<fingerTipPoints.size(); i++){
		float ftx = fingerTipPoints[i].x;
		float fty = fingerTipPoints[i].y;
		ofEllipse(ftx, fty, 9,9);
	}
	
	
	
	
	// handContour3 is resampled, then smoothed, then resampled
	ofNoFill();
	ofSetColor(255,100,0);
	handContourFiltered.draw();

	
	float cx = handCentroid.x;
	float cy = handCentroid.y;
	ofNoFill();
	ofEllipse(cx, cy, 10,10);
	ofEllipse(cx, cy, handRadius*2, handRadius*2);
	
	ofFill();
	for (int i=0; i<fingerTipPointsFiltered.size(); i++){
		float ftx = fingerTipPointsFiltered[i].x;
		float fty = fingerTipPointsFiltered[i].y;
		ofEllipse(ftx, fty, 9,9);
	}
	
	
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
		bool hasNeighbor = false;
		for(int j = 0; j < indices.size(); j++) {
			if(abs(curIndex - indices[j]) < peakArea || abs((curIndex + n) - indices[j]) < peakArea) {
				hasNeighbor = true;
				break;
			}
		}
		if(!hasNeighbor) {
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
ofMesh& HandContourAnalyzerAndMeshBuilder::getMesh(){
	return handMesh;
}

//============================================================
vector<int>& HandContourAnalyzerAndMeshBuilder::getJoints(){
	return joints;
}
	
