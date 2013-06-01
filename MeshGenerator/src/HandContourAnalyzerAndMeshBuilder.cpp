//  Created by Golan Levin on 5/19/13.

#include "HandContourAnalyzerAndMeshBuilder.h"


//============================================================
HandContourAnalyzerAndMeshBuilder::HandContourAnalyzerAndMeshBuilder (){
	
	/*
	vector <int>	joints;
	ofMesh			handMesh;
	 */
	
	currentHandType			= HAND_NONE;
	bCalculatedMesh			= false;
	contourResampleSpacing	= 2.0;
	contourSmoothingSize	= 20.0;
	
	sampleOffset			= 8;
	peakAngleCutoff			= 14;
	peakNeighborDistance	= 32;
	
	crotchAngleCutoff		= -40;
	crotchNeighborDistance	= 40;
	maxNCrotchesToConsider	= 4;
	
	tooMuchMotionThresholdInStDevs = 1.00;
	indexInterpolationAlpha  = 0.40;

	
	contourIndexOfPalmBase  = 0;
	contourIndexOfThumbKnuckle = 0;
	
	
	// drawing order, in case you're curious.
	fingerTipIndices[0] = HANDMARK_THUMB_TIP;
	fingerTipIndices[1] = HANDMARK_PINKY_TIP;
	fingerTipIndices[2] = HANDMARK_RING_TIP;
	fingerTipIndices[3] = HANDMARK_MIDDLE_TIP;
	fingerTipIndices[4] = HANDMARK_POINTER_TIP;
	
	
}

//============================================================
void HandContourAnalyzerAndMeshBuilder::informThereIsNoHandPresent(){
	
	currentHandType = HAND_NONE;
	currentHandExistsFrameCount = 0;
	
	for (int i=0; i<N_HANDMARKS; i++){
		Handmarks[i].pointHistory.clear();
		Handmarks[i].pointAvg.set     (0,0);
		Handmarks[i].pointStDv.set    (0,0);
	}
	
}

//============================================================
void HandContourAnalyzerAndMeshBuilder::process (ofPolyline inputContour, cv::Point2f inputCentroid){
	
	computeHandmarkStatistics();
	if (inputContour.size() > 0){
		
		/* 
		ofPolyline bogus; 
		float angle = 1.0; //ofGetElapsedTimef()/10.0;
		float newX, newY;
		for (int i=0; i<inputContour.size(); i++) {
			ofPoint pt = inputContour[i];
			// translate the point so that its center is on the origin
			pt.x = pt.x - inputCentroid.x;
			pt.y = pt.y - inputCentroid.y;
			// rotate the point
			newX = (pt.x * cos(angle)) - (pt.y * sin(angle));
			newY = (pt.x * sin(angle)) + (pt.y * cos(angle));
			// translate the point back
			pt.x = newX + inputCentroid.x;
			pt.y = newY + inputCentroid.y;
			bogus.addVertex(pt.x, pt.y);
		}
		*/
		
		// Update hand information. 
		currentHandType = HAND_RIGHT; // replace later
		currentHandExistsFrameCount++;
		
		// Compute smoothed and resampled contours, the globals:
		// handContourResampled, handContourNice, handContourFiltered
		prepareContours (inputContour, inputCentroid);
		
		// Compute (sorted) fingertips into fingerTipPoints and fingerTipContourIndices.
		// Also computes handContourFilteredCurvatures.
		locateFingerTips();
		
		// Locate (unsorted) finger valleys, into fingerCrotchPointsTmp & handContourCrotchIndicesTmp
		locateFingerCrotches();
	
		// Reckon which crotch is the thumb's, and thus assign currentHandType
		identifyThumbCrotchAndHandType(); 
		if (currentHandType > HAND_NONE){
			
			// Obtain the "side line" which transects the hand, to find pointer & pinky side corners. 
			computeHandOrientationAndSideLine();
			
			// Locate the pinky & pointer sides, store in contourIndexOfPinkySide & contourIndexOfPointerSide
			locatePinkySide();
			locatePointerSide();
			locateThumbBase();
			locatePalmBase();
			locateThumbKnuckle();
			
			assembleHandmarksPreliminary();
			refineFingertips();
			refineHandmarksBasedOnMotionStDev();
			buildMesh();
			
		}
		
		//--------------------------------------
		// bCalculatedMesh = true;
		
		
	} else {
		// There is no hand contour. 
		currentHandType	= HAND_NONE;
		currentHandExistsFrameCount = 0; 
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
	bool bDrawFilteredContour			= false;
	bool bDrawHandmarksOutline			= false;
	bool bDrawHandmarks					= true;
	bool bDrawHandMarkTrails			= false; 
	bool bDrawFirstFingerTipEstimates	= false;
	bool bDrawCrotchPoints				= false;
	

	ofSetLineWidth(1.0);
	ofSetColor(0,255,0);
	handContourNice.draw();
	
	
	if (bDrawFirstFingerTipEstimates){
		ofFill();
		ofSetColor(50,200,0);
		for (int i=0; i<fingerTipPoints.size(); i++){
			float ftx = fingerTipPoints[i].x;
			float fty = fingerTipPoints[i].y;
			ofEllipse(ftx, fty, 9,9);
			ofDrawBitmapString( ofToString(i), ftx+8, fty+10); 
		}
	}
	if (bDrawCrotchPoints){
		ofFill();
		ofSetColor(0,150,255);
		for (int i=0; i<fingerCrotchPoints.size(); i++){
			float ftx = fingerCrotchPoints[i].x;
			float fty = fingerCrotchPoints[i].y;
			ofEllipse(ftx, fty, 9,9);
			ofDrawBitmapString( ofToString(i), ftx+8, fty+10); 
		}
	}
	
	// ----------------
	// DRAW HANDMARKS!
	if (bDrawHandmarksOutline){
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
	}
	
	if (bDrawHandMarkTrails){
		for (int i=0; i<N_HANDMARKS; i++){
			ofSetColor(100,100,100, 60);
			ofEnableAlphaBlending();
			ofNoFill();
			ofBeginShape();
			for (int j=0; j<Handmarks[i].pointHistory.size(); j++){
				float px = Handmarks[i].pointHistory[j].x;
				float py = Handmarks[i].pointHistory[j].y;
				ofVertex( px,py );
			}
			ofEndShape();
			ofDisableAlphaBlending();
		}
	}
		
	if (bDrawHandmarks){
		ofEnableAlphaBlending();
		for (int i=0; i<N_HANDMARKS; i++){
			ofSetColor(0,255,0, 200);
			ofFill();
			if (Handmarks[i].type != HANDMARK_INVALID){
				float hx = Handmarks[i].point.x;
				float hy = Handmarks[i].point.y;
				ofEllipse(hx, hy, 6,6);
			}
		}
		ofDisableAlphaBlending();
		
		// draw Standard Deviation circles.
		ofNoFill();
		ofEnableAlphaBlending();
		for (int i=0; i<N_HANDMARKS; i++){
			if (Handmarks[i].type != HANDMARK_INVALID){
				
				int np = Handmarks[i].pointHistory.size();
				if (np > 2){
					
					float px = Handmarks[i].point.x;
					float py = Handmarks[i].point.y;
					
					float rx = Handmarks[i].pointStDv.x;
					float ry = Handmarks[i].pointStDv.y;
					float rh = sqrtf(rx*rx + ry*ry);
					
					float prevx = Handmarks[i].pointHistory[np-1].x;
					float prevy = Handmarks[i].pointHistory[np-1].y;
					float dx = px - prevx;
					float dy = py - prevy;
					float dh = sqrtf(dx*dx + dy*dy);
					
					if (dh > (tooMuchMotionThresholdInStDevs * rh)){
						ofSetLineWidth(3);
						ofSetColor(255,10,0);
					} else {
						ofSetLineWidth(1);
						ofSetColor(50,100,50, 180);
					}
					
					ofEllipse(px, py, 10+rh,10+rh);
				}
			}
		}
		ofDisableAlphaBlending();
		ofSetLineWidth(1);
	}
	
	
	
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
	
	
	

	if (bDrawFilteredContour){
		ofNoFill();
		ofSetColor(255,100,0);
		handContourFiltered.draw();
	}

	
	float cx = handCentroid.x;
	float cy = handCentroid.y;
	ofNoFill();
	ofEllipse(cx, cy, 10,10);
	ofEllipse(cx, cy, handRadius*2, handRadius*2);
	
	ofSetColor(180,180,0);
	float ox = 100 * sinf(handOrientation);
	float oy = 100 * cosf(handOrientation);
	ofLine (cx,cy, cx+ox, cy+oy);
	
	
	for (int f=0; f<5; f++){
		Handmark H = Handmarks[fingerTipIndices[f]];
		float fx = H.point.x;
		float fy = H.point.y;
		float fox = 50 * sinf(fingerOrientations[f]);
		float foy = 50 * cosf(fingerOrientations[f]);
		ofLine (fx-fox,fy-foy, fx+fox, fy+foy);
	}
	
	
	//============================================================================================================
	float deltaX = -150;
	float deltaY = -450;
	ofPushMatrix();
	//ofTranslate( deltaX, deltaY);
	//ofScale(1.5, 1.5);
	

	handMesh.clear();
	handMesh.setupIndicesAuto();
	handMesh.setMode( OF_PRIMITIVE_TRIANGLES );
	
	// Add triangles for each finger to handMesh:
	int nContourPoints = handContourNice.size();
	for (int f=0; f<5; f++){
	
		int f0 = (fingerTipIndices[f] - 1 + N_HANDMARKS) % N_HANDMARKS;
		int f1 = (fingerTipIndices[f]     + N_HANDMARKS) % N_HANDMARKS;
		int f2 = (fingerTipIndices[f] + 1 + N_HANDMARKS) % N_HANDMARKS;
		
		int contourIndex0 = Handmarks[f0].index;
		int contourIndex1 = Handmarks[f1].index;
		int contourIndex2 = Handmarks[f2].index;
		
		// Collect the first side of the finger, from contourIndex0 up to contourIndex1
		ofPolyline poly01;
		poly01.clear();
		int contourIndex1a = contourIndex1;
		if (contourIndex1a < contourIndex0) {
			contourIndex1a += nContourPoints;
		}
		for (int i=contourIndex0; i<=contourIndex1a; i++){
			int indexSafe = (i+nContourPoints)%nContourPoints; 
			ofVec2f pointi = handContourNice[indexSafe];
			poly01.addVertex(pointi.x, pointi.y);
		}
		// Collect the reverse side of the finger, from contourIndex2 down to contourIndex1
		ofPolyline poly21;
		poly21.clear();
		int contourIndex1b = contourIndex1;
		if (contourIndex1b > contourIndex2) {
			contourIndex2 += nContourPoints;
		}
		for (int i=contourIndex2; i>=contourIndex1b; i--){
			int indexSafe = (i+nContourPoints)%nContourPoints;
			ofVec2f pointi = handContourNice[indexSafe];
			poly21.addVertex(pointi.x, pointi.y);
		}
		 
		
		//-----------------------
		// Resample the finger's two sides. 
		ofPolyline poly01RS;
		poly01RS.clear();
		poly01RS = poly01.getResampledByCount(N_FINGER_LENGTH_SAMPLES);
		int nPoly01RS = poly01RS.size();
		if (nPoly01RS == N_FINGER_LENGTH_SAMPLES){
			// weird bug. getResampledByCount does not return a consistent number of points.
			ofVec2f pointi = handContourNice[contourIndex1%nContourPoints];
			poly01RS.addVertex(pointi.x, pointi.y);
		}
		ofPolyline poly21RS;
		poly21RS.clear();
		poly21RS = poly21.getResampledByCount(N_FINGER_LENGTH_SAMPLES);
		int nPoly21RS = poly21RS.size();
		if (nPoly21RS == N_FINGER_LENGTH_SAMPLES){
			// weird bug. getResampledByCount does not return a consistent number of points.
			ofVec2f pointi = handContourNice[contourIndex1%nContourPoints];
			poly21RS.addVertex(pointi.x, pointi.y);
		}
		 
		//-----------------------
		// Render the two finger sides.
		bool bRenderFingerEdgePolylines = true;
		if (bRenderFingerEdgePolylines){
			ofNoFill();
			ofSetColor(0,200,100);
			poly01RS.draw();
			ofSetColor(200,100,0);
			poly21RS.draw();
		}
		
		bool bDrawContourTexPoints = false;
		if (bDrawContourTexPoints){
			ofSetColor(0,200,250);
			vector<ofPoint> poly01RSpts = poly01RS.getVertices();
			int nPoly01RSpts = poly01RSpts.size();
			for (int i=0; i<nPoly01RSpts; i++){
				float ex = poly01RSpts[i].x;
				float ey = poly01RSpts[i].y;
				ofEllipse (ex, ey, 6, 6);
			}
			ofSetColor(250,200,0);
			vector<ofPoint> poly21RSpts = poly21RS.getVertices();
			int nPoly21RSpts = poly21RSpts.size();
			for (int i=0; i<nPoly21RSpts; i++){
				float ex = poly21RSpts[i].x;
				float ey = poly21RSpts[i].y;
				ofEllipse (ex, ey, 9, 9);
			}
		}
		
		//---------------
		// ADD VERTICES TO THE MESH. 
		vector<ofPoint> poly01RSpts = poly01RS.getVertices();
		vector<ofPoint> poly21RSpts = poly21RS.getVertices();
		int nPoly01RSpts = poly01RSpts.size();
		int nPoly21RSpts = poly21RSpts.size();
		if ((nPoly01RSpts == nPoly21RSpts) &&
			(nPoly01RSpts == (N_FINGER_LENGTH_SAMPLES+1))){
	
			for (int i=0; i<=N_FINGER_LENGTH_SAMPLES; i++){
				float px01 = poly01RSpts[i].x;
				float py01 = poly01RSpts[i].y;
				float px21 = poly21RSpts[i].x;
				float py21 = poly21RSpts[i].y;
				
				if (i == N_FINGER_LENGTH_SAMPLES){
					// Special case for the tip.
					// N.B., px01 and px21 are the same point
					
					float qx01 = (poly01RSpts[N_FINGER_LENGTH_SAMPLES-1].x + px01)/2.0;
					float qy01 = (poly01RSpts[N_FINGER_LENGTH_SAMPLES-1].y + py01)/2.0;
					ofPoint q01 = ofPoint(qx01, qy01);
				
					float qx21 = (poly21RSpts[N_FINGER_LENGTH_SAMPLES-1].x + px21)/2.0;
					float qy21 = (poly21RSpts[N_FINGER_LENGTH_SAMPLES-1].y + py21)/2.0;
					ofPoint q21 = ofPoint(qx21, qy21);
					
					ofPoint QA = handContourNice.getClosestPoint(q01);
					ofPoint QB = handContourNice.getClosestPoint(q21);
					
					ofEllipse(QA.x, QA.y,  2, 2);
					ofEllipse(px01, py01,  2, 2);
					ofEllipse(QB.x, QB.y,  2, 2);
					
					handMesh.addVertex(ofPoint(QA.x, QA.y,0)); 
					handMesh.addVertex(ofPoint(px01, py01,0)); 
					handMesh.addVertex(ofPoint(QB.x, QB.y,0)); 
					
					
				} else {
					// The usual case, up the sides of the finger.
					if (f < 2){
						for (int j=0; j<N_FINGER_WIDTH_SAMPLES; j++){
							float ex = ofMap(j, 0,N_FINGER_WIDTH_SAMPLES-1, px01,px21);
							float ey = ofMap(j, 0,N_FINGER_WIDTH_SAMPLES-1, py01,py21);
							ofEllipse(ex,ey, 2, 2);
							handMesh.addVertex(ofPoint(ex,ey,0)); 
						}
					} else {
						// skip the 0th point for fingers 2,3,4,
						// because these are shared vertices with points we've already laid.
						for (int j=0; j<N_FINGER_WIDTH_SAMPLES; j++){
							float ex = ofMap(j, 0,N_FINGER_WIDTH_SAMPLES-1, px01,px21);
							float ey = ofMap(j, 0,N_FINGER_WIDTH_SAMPLES-1, py01,py21);
							ofEllipse(ex,ey, 2, 2);
							if ((j==0) && (i==0)){
								;
							} else {
								handMesh.addVertex(ofPoint(ex,ey,0));
							}
						}
						
					}
				}
			}
			
			//---------------
			// ADD TRIANGLES TO THE MESH. 
			// add the triangles on the interior of the finger.
			int vertexIndex = f * (N_FINGER_WIDTH_SAMPLES * N_FINGER_LENGTH_SAMPLES + 3);
			int NW = N_FINGER_WIDTH_SAMPLES;
			
			if (f >= 2){
				// Because we skip the 0th point for fingers 2,3,4
				// (Because the start point for a finger is coincident with the end point
				// for the previous finger)...
			    vertexIndex -= (f-1);
				
				for (int i=0; i<(N_FINGER_LENGTH_SAMPLES-1); i++){
					int row = vertexIndex + i*N_FINGER_WIDTH_SAMPLES;
					for (int j=0; j<(N_FINGER_WIDTH_SAMPLES-1); j++){
						
						if ((i==0) && (j==0)){
							int screwCase = row - (11*5 + 3 - 5);
							handMesh.addTriangle(screwCase, row+j+1,  row+j+NW);
							handMesh.addTriangle(row+j+1, row+j+1+NW, row+j+NW);
						} else {
							handMesh.addTriangle(row+j  , row+j+1,    row+j+NW);
							handMesh.addTriangle(row+j+1, row+j+1+NW, row+j+NW);
						}
					}
				}
				// add triangles at fingertip.
				int row = vertexIndex + (N_FINGER_LENGTH_SAMPLES-1)*N_FINGER_WIDTH_SAMPLES;
				handMesh.addTriangle(row+0, row+1, row+NW);
				handMesh.addTriangle(row+1, row+2, row+NW);
				handMesh.addTriangle(row+2, row+6, row+NW);
				handMesh.addTriangle(row+2, row+7, row+1+NW);
				handMesh.addTriangle(row+2, row+3, row+2+NW);
				handMesh.addTriangle(row+3, row+4, row+2+NW);
				
				
			} else {
				// The simple cases, thumb and pinky.
				//
				for (int i=0; i<(N_FINGER_LENGTH_SAMPLES-1); i++){
					int row = vertexIndex + i*N_FINGER_WIDTH_SAMPLES;
					for (int j=0; j<(N_FINGER_WIDTH_SAMPLES-1); j++){
						handMesh.addTriangle(row+j  , row+j+1,    row+j+NW);
						handMesh.addTriangle(row+j+1, row+j+1+NW, row+j+NW);
					}
				}
				// add triangles at fingertip. 
				int row = vertexIndex + (N_FINGER_LENGTH_SAMPLES-1)*N_FINGER_WIDTH_SAMPLES;
				handMesh.addTriangle(row+0, row+1, row+NW);
				handMesh.addTriangle(row+1, row+2, row+NW);
				handMesh.addTriangle(row+2, row+6, row+NW);
				handMesh.addTriangle(row+2, row+7, row+1+NW);
				handMesh.addTriangle(row+2, row+3, row+2+NW);
				handMesh.addTriangle(row+3, row+4, row+2+NW);
			}
		}
	}
	
	//----------------------------------------------------
	// Add vertices for the knuckles.
	// TRIAGE: START USING SPECIFIC INDICES (rather than computed indices) to build the rest of the mesh. 
	// 60 = N_FINGER_LENGTH_SAMPLES*N_FINGER_WIDTH_SAMPLES + 3, then +2 more into the finger, etc.
	//
	int fingerBaseMiddleIndices[] = {60, 117, 174, 231};
	for (int i=0; i<4; i++){
		int fingerBaseMiddle0 = fingerBaseMiddleIndices[i];
		int fingerBaseMiddle1 = fingerBaseMiddle0 + N_FINGER_WIDTH_SAMPLES;
		ofVec3f P0 = handMesh.getVertex(fingerBaseMiddle0);
		ofVec3f P1 = handMesh.getVertex(fingerBaseMiddle1);
		ofVec3f Pk = P0 - 1.25*(P1 - P0);
		
		// move it slightly toward the hand centroid
		ofVec3f Pc = ofVec3f(handCentroid.x, handCentroid.y, 0);
		float amountToMoveKnucklesTowardCentroid = 0.25;
		Pk = Pk - amountToMoveKnucklesTowardCentroid*(Pk - Pc);
		ofEllipse(Pk.x,Pk.y, 2, 2);
		handMesh.addVertex(Pk);
		
		int nv = handMesh.getNumVertices();
		for (int j=0; j<4; j++){
			if (i==0){
				int fi0 = fingerBaseMiddle0 + j - 2;
				int fi1 = fingerBaseMiddle0 + j - 1;
				handMesh.addTriangle(nv-1, fi1, fi0);
			} else {
				int fi0 = fingerBaseMiddle0 + j - 2;
				if (j == 0) { fi0 = fingerBaseMiddleIndices[i-1] + 2; }
				int fi1 = fingerBaseMiddle0 + j - 1;
				handMesh.addTriangle(nv-1, fi1, fi0);
			}
		}
	}
	
	//----------------------------------------------------
	// Add vertices for the wrist.
	
	int wristIndex0 = Handmarks[HANDMARK_THUMB_BASE      ].index;
	int wristIndex1 = Handmarks[HANDMARK_THUMBSIDE_WRIST ].index;
	int wristIndex2 = Handmarks[HANDMARK_PINKYSIDE_WRIST ].index;
	int wristIndex3 = Handmarks[HANDMARK_PALM_BASE       ].index;
	
	ofVec2f wristP0 = handContourNice[wristIndex0];
	ofVec2f wristP1 = handContourNice[wristIndex1];
	ofVec2f wristP2 = handContourNice[wristIndex2];
	ofVec2f wristP3 = handContourNice[wristIndex3];
	
	vector<ofPoint> wSide01;
	vector<ofPoint> wSide32;
	vector<ofPoint> wSide12;
	
	for (int i=0; i<=4; i++){
		ofVec2f whcn = ((4.0 - (float)i)*wristP0 + ((float)i)*wristP1)/4.0;
		ofPoint wristP_hcn = handContourNice.getClosestPoint(whcn);
		wSide01.push_back (wristP_hcn);
	}
	for (int i=0; i<=4; i++){
		ofVec2f whcn = ((4.0 - (float)i)*wristP3 + ((float)i)*wristP2)/4.0;
		ofPoint wristP_hcn = handContourNice.getClosestPoint(whcn);
		wSide32.push_back (wristP_hcn);
	}
	for (int i=0; i<=4; i++){
		ofVec2f whcn = ((4.0 - (float)i)*wristP1 + ((float)i)*wristP2)/4.0;
		ofPoint wristP_hcn = handContourNice.getClosestPoint(whcn);
		wSide12.push_back (wristP_hcn);
	}
	
	int nvBeforeWrist = handMesh.getNumVertices();
	for (int wy=0; wy<=4; wy++){
		ofVec2f pL = wSide01[wy];
		ofVec2f pR = wSide32[wy];
		if (wy < 4){
			for (int wx=4; wx>=0; wx--){
				ofVec2f wp = ((4.0 - (float)wx)*pL + ((float)wx)*pR)/4.0;
				ofEllipse(wp.x,wp.y, 2, 2);
				handMesh.addVertex(ofPoint(wp.x,wp.y,0));
			}
		} else {
			for (int wx=4; wx>=0; wx--){
				ofVec2f wp = wSide12[wx];
				ofEllipse(wp.x,wp.y, 2, 2);
				handMesh.addVertex(ofPoint(wp.x,wp.y,0));
			}
		}
	}
	
	int nvb = nvBeforeWrist;
	for (int wy=0; wy<4; wy++){
		for (int wx=0; wx<4; wx++){
			handMesh.addTriangle(nvb+wx,   nvb+wx+5, nvb+wx+1);
			handMesh.addTriangle(nvb+wx+1, nvb+wx+5, nvb+wx+6);
		}
		nvb += 5;
	}
	
	
	//----------------------------------------------------
	// Add vertices for the thumb base.
	
	// 0,1,2,3,4, 295
	
	// Get interpolated values on the outside contour: 
	int thumbBaseIndex0 = Handmarks[HANDMARK_THUMB_KNUCKLE ].index;
	int thumbBaseIndex1 = Handmarks[HANDMARK_THUMB_BASE    ].index;
	
	ofVec2f thumbBaseHcn0 = handContourNice[thumbBaseIndex0];
	ofVec2f thumbBaseHcn1 = handContourNice[thumbBaseIndex1];
	
	// create a polyline copy of the handContourNice sub-section between the 2 indices
	ofPolyline thumbCurve;
	if (thumbBaseIndex0 < thumbBaseIndex1){
		for (int i=thumbBaseIndex0; i<=thumbBaseIndex1; i++){
			ofPoint tpoint = handContourNice[i];
			thumbCurve.addVertex(tpoint);
		}
	} else {
		// don't want to deal with an unlikely situation
		thumbCurve = handContourNice;
	}
	
	vector<ofPoint> thumbBaseSide;
	for (int i=0; i<=4; i++){
		ofVec2f hcn = ((4.0 - (float)i)*thumbBaseHcn0 + ((float)i)*thumbBaseHcn1)/4.0;
		ofPoint Phcn = thumbCurve.getClosestPoint(hcn);
		thumbBaseSide.push_back (Phcn);
		//ofEllipse(Phcn.x,Phcn.y, 10, 10);
		// n.b., we will not use points i=0 & i=4 because they are already stored as handMesh vertices 4 & 295
	}
	
	vector<ofPoint> thumbBaseHypotenuse;
	ofVec2f thumbBaseP0 = ofVec2f(handMesh.getVertex(0).x, handMesh.getVertex(0).y);
	for (int i=0; i<=4; i++){
		ofVec2f hcn = ((4.0 - (float)i)*thumbBaseP0 + ((float)i)*thumbBaseHcn1)/4.0;
		thumbBaseHypotenuse.push_back (hcn);
		//ofEllipse(hcn.x,hcn.y, 10, 10);
		// n.b., we will not use points i=0 & i=4 because they are already stored as handMesh vertices 0 & 295
	}
	int nvBeforeThumbBase = handMesh.getNumVertices();
	for (int y=1; y<4; y++){ // skipping y=0 and y=4 because those vertices already exist in handMesh.
		int topx = 4-y;
		for (int x=0; x<=topx; x++){
			ofPoint T0 = thumbBaseHypotenuse[y];
			ofPoint T1 = thumbBaseSide[y];
			ofPoint Tinterp = ((topx-x)*T0 + (x)*T1) / (float)topx;
			handMesh.addVertex(ofPoint(Tinterp.x,Tinterp.y,0));
			ofEllipse(Tinterp.x,Tinterp.y, 2, 2);
		}
	}
	
	// clockwise triangles
	int ti = nvBeforeThumbBase;
	for (int y=0; y<1; y++){ // y == 0 case
		int topx = 4-y;
		for (int x=0; x<topx; x++){
			if (x == 0){
				handMesh.addTriangle(x, ti+x, x+1);
			} else {
				handMesh.addTriangle(x, ti+(x-1), ti+x);
				handMesh.addTriangle(x, ti+x, x+1);
			}
		}
	}
	
	int thumbBaseVertexIndex = 295;
	for (int y=1; y<4; y++){
		int topx = 4-y;
		for (int x=0; x<topx; x++){
			if (x == 0){
				int tj = ti+x+topx+1;
				if (y == 3){ tj = thumbBaseVertexIndex; }
				handMesh.addTriangle(ti+x,   tj, ti+x+1);
			} else {
				handMesh.addTriangle(ti+x,   ti+x+topx+1,     ti+x+1);
				handMesh.addTriangle(ti+x,   ti+x+topx  ,     ti+x+topx+1 );
			}	
		}
		ti += topx+1;
	}
	 
	
	//----------------------------------------------------
	// Add vertices for the thumb webbing.
	
	// Get interpolated values on the outside contour:
	int thumbWebIndex0 = Handmarks[HANDMARK_POINTER_SIDE ].index;
	int thumbWebIndex1 = Handmarks[HANDMARK_IT_CROTCH    ].index;

	ofVec2f thumbWebHcn0 = handContourNice[thumbWebIndex0];
	ofVec2f thumbWebHcn1 = handContourNice[thumbWebIndex1];
	
	// create a polyline copy of the handContourNice sub-section between the 2 indices
	ofPolyline thumbWebCurve;
	if (thumbWebIndex0 < thumbWebIndex1){
		for (int i=thumbWebIndex0; i<=thumbWebIndex1; i++){
			ofPoint tpoint = handContourNice[i];
			thumbWebCurve.addVertex(tpoint);
		}
	} else {
		// don't want to deal with an unlikely situation
		thumbWebCurve = handContourNice;
	}
	
	// vector of interpolated points along top edge
	vector<ofPoint> thumbWebSide1;
	for (int i=0; i<=4; i++){
		ofVec2f hcn = ((4.0 - (float)i)*thumbWebHcn0 + ((float)i)*thumbWebHcn1)/4.0;
		ofPoint Phcn = thumbWebCurve.getClosestPoint(hcn);
		thumbWebSide1.push_back (Phcn);
		// ofEllipse(Phcn.x,Phcn.y, 10, 10);
		// n.b., we will not use points i=0 & i=4, becasue they are handMesh vertices 0 and 233 
	}

	vector<ofPoint> thumbWebHypotenuse;
	ofVec2f thumbWebP0 = ofVec2f(handMesh.getVertex(233).x, handMesh.getVertex(233).y);
	ofVec2f thumbWebP1 = ofVec2f(handMesh.getVertex(thumbBaseVertexIndex).x, handMesh.getVertex(thumbBaseVertexIndex).y);
	int nWebHypSamps = 8;
	for (int i=0; i<=nWebHypSamps; i++){
		ofVec2f hcn = ((nWebHypSamps - (float)i)*thumbWebP0 + ((float)i)*thumbWebP1)/(float)nWebHypSamps;
		thumbWebHypotenuse.push_back (hcn);
		// ofEllipse(hcn.x,hcn.y, 10, 10);
		// n.b., we will not use points i=0 & i=4 because they are already stored as handMesh vertices 233 & 295
	}
	
	int nvBeforeThumbWeb = handMesh.getNumVertices();
	for (int y=1; y<4; y++){ // skipping y=0 and y=4 because those vertices already exist in handMesh.
		int topx = 4-y;
		for (int x=0; x<=topx; x++){
			ofPoint T0 = thumbWebHypotenuse[8-(y*2)];
			ofPoint T1 = thumbWebSide1[4-y];
			ofPoint Tinterp = ((topx-x)*T0 + (x)*T1) / (float)topx;
			handMesh.addVertex(ofPoint(Tinterp.x,Tinterp.y,0));
			ofEllipse(Tinterp.x,Tinterp.y, 2, 2);
		}
	}
	for (int i=7; i>=1; i-=2){
		handMesh.addVertex( thumbWebHypotenuse[i] );
		ofEllipse(thumbWebHypotenuse[i].x,thumbWebHypotenuse[i].y, 2, 2);
	}
	
	
	int thumbWebSideIndices[] = {0, 316, 320, 323, 295};
	
	int wi = nvBeforeThumbWeb;
	// handMesh.addTriangle(thumbWebSideIndices[4-0], thumbWebSideIndices[4-1], wi+0); // replace with 2 triangles
	handMesh.addTriangle(thumbWebSideIndices[4-0],    thumbWebSideIndices[4-1], wi+9);
	handMesh.addTriangle(thumbWebSideIndices[4-1],    wi+0, wi+9);
	handMesh.addTriangle(thumbWebSideIndices[4-1], thumbWebSideIndices[4-2], wi+0);
	handMesh.addTriangle(thumbWebSideIndices[4-2], wi+1, wi+0);
	handMesh.addTriangle(thumbWebSideIndices[4-2], thumbWebSideIndices[4-3], wi+1);
	handMesh.addTriangle(thumbWebSideIndices[4-3], wi+2, wi+1);
	handMesh.addTriangle(thumbWebSideIndices[4-3], thumbWebSideIndices[4-4], wi+2);
	handMesh.addTriangle(thumbWebSideIndices[4-4], wi+3, wi+2);
	
	// handMesh.addTriangle(wi+0, wi+1, wi+4); // replace with 2 triangles, as follows:
	handMesh.addTriangle(wi+0, wi+1, wi+10);
	handMesh.addTriangle(wi+1, wi+4, wi+10);
	handMesh.addTriangle(wi+1, wi+2, wi+4);
	handMesh.addTriangle(wi+2, wi+5, wi+4);
	handMesh.addTriangle(wi+2, wi+3, wi+5);
	handMesh.addTriangle(wi+3, wi+6, wi+5);
	
	// handMesh.addTriangle(wi+4, wi+5, wi+7); // replace with 2 triangles, as follows:
	handMesh.addTriangle(wi+4, wi+5, wi+11);
	handMesh.addTriangle(wi+5, wi+7, wi+11);
	handMesh.addTriangle(wi+5, wi+6, wi+7);
	handMesh.addTriangle(wi+6, wi+8, wi+7);
	
	// handMesh.addTriangle(wi+7, wi+8, 233 ); // replace with 2 triangles, as follows:
	handMesh.addTriangle(wi+7, wi+8, wi+12 );
	handMesh.addTriangle(wi+8, 233,  wi+12 );
	
	
	//----------------------------------------------------
	// Mesh the palm.
	
	// Get interpolated values on the outside contour:
	int palmContourIndex0 = Handmarks[HANDMARK_PALM_BASE     ].index;
	int palmContourIndex1 = Handmarks[HANDMARK_PINKY_SIDE    ].index;
	
	ofPolyline palmSideContour;
	ofPolyline palmSideContourResampled;
	bool bGotPalmSideContour = true;
	if (palmContourIndex0 < palmContourIndex1){
		for (int i=palmContourIndex0; i<=palmContourIndex1; i++){
			ofPoint cpt = handContourNice[i]; 
			palmSideContour.addVertex( cpt ); 
		}
		int nDesiredResampledPoints = 9;
		palmSideContourResampled = palmSideContour.getResampledByCount(nDesiredResampledPoints-1);
		
		int nPalmSideContourPoints = palmSideContourResampled.size();
		if (nPalmSideContourPoints != nDesiredResampledPoints){ // that weird resample bug again.
			ofPoint cpt = handContourNice[palmContourIndex1];
			palmSideContourResampled.addVertex( cpt.x, cpt.y );
		}
		
	} else {
		// hopefully this is really unlikely.
		bGotPalmSideContour = false; 
		printf("Problem meshing palm side.");
	}
	
	if (bGotPalmSideContour){
		
		int nPalmSideResampledContourPoints = palmSideContourResampled.size();
		for (int i=1; i<(nPalmSideResampledContourPoints-1); i++){
			ofPoint cpt = palmSideContourResampled[i];
			handMesh.addVertex (cpt); 
			ofEllipse(cpt.x, cpt.y, 2,2);
		}
		
		handMesh.addTriangle (344, 287, 58);
		handMesh.addTriangle (287, 288, 62);
		handMesh.addTriangle (288, 289, 119);
		handMesh.addTriangle (289, 290, 176);
		handMesh.addTriangle (290, 337, 233);
		
		int wristPointMeshIndex = 293;
		int wristPointMeshIndices[]    = {292, 293,293,293,293, 294};
		int knuckleMeshIndices[]       = {344, 287,288,289,290, 337, 337};
		int thumbSidePalmMeshIndices[] = {295, 334, 325, 335, 329, 336, 332, 337, 233};
		
		for (int k=0; k<6; k++){
			float wx = handMesh.getVertex(wristPointMeshIndices[k]).x; 
			float wy = handMesh.getVertex(wristPointMeshIndices[k]).y; 
			float kx = handMesh.getVertex(knuckleMeshIndices[k]).x;
			float ky = handMesh.getVertex(knuckleMeshIndices[k]).y;
			for (int i=1; i<7; i++){
				float frac = (float)i/7.0;
				float px = (1-frac)*wx + frac*kx;
				float py = (1-frac)*wy + frac*ky;
				handMesh.addVertex( ofVec3f (px,py, 0));
				ofEllipse(px, py, 2,2);
			}
		}
		
		int starti = 338;
		for (int j=0; j<=6; j++){
			
			int dn = 6;
			if (j==0){
				dn = 7;
			}
			
			if (j == 6){
				
				for (int i=0; i<=5; i++){
					int a = starti + i;
					int b = thumbSidePalmMeshIndices[i+1];
					int c = starti + i + 1;
					int d = thumbSidePalmMeshIndices[i+2];
					
					if (i==5){
						c = 337;
						handMesh.addTriangle (a, b, c);
					} else {
						handMesh.addTriangle (a, b, c);
						handMesh.addTriangle (c, b, d);
					}
					
				}
				
			} else {
				if ((j>=1) && (j < 5)){
					int a = starti;
					int b = wristPointMeshIndex;
					int c = starti + dn;
					handMesh.addTriangle (a, b, c);
				}
				
				for (int i=0; i<=5; i++){
					int a = starti + i;
					int b = starti + i + dn;
					int c = starti + i + 1;
					int d = starti + i + dn+1;

					if (i==5){
						if (j > 0){
							c = knuckleMeshIndices[j-1];
							d = knuckleMeshIndices[j  ];
							handMesh.addTriangle (a, b, c);
							handMesh.addTriangle (c, b, d);
						}
					} else {
						handMesh.addTriangle (a, b, c);
						handMesh.addTriangle (c, b, d);
					}
				}
			}
			
			starti += dn;
		}
		
		handMesh.addTriangle (344, 343, 350);
		handMesh.addTriangle (291, 292, 338);
		handMesh.addTriangle (292, 345, 338);
		handMesh.addTriangle (292, 293, 345);
		handMesh.addTriangle (293, 294, 369);
		handMesh.addTriangle (294, 375, 369);
		handMesh.addTriangle (294, 295, 375);
		handMesh.addTriangle (295, 334, 375);
		
		
		
		
	}
	
	
	//----------------------------------------------------
	ofEnableAlphaBlending();
	ofSetColor(255,100,100, 70);
	handMesh.draw();
	ofSetColor(255,120,120, 50);
	handMesh.drawWireframe();
	ofDisableAlphaBlending();
	
	bCalculatedMesh = true;
	
	
	

	
	
	
	ofPopMatrix();
	

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
	
	
	
	int nMeshVertices = handMesh.getNumVertices();
	int whichVertex = ((int)(mx))%nMeshVertices;
	ofVec2f aPoint = handMesh.getVertex(whichVertex); 
	 
	ofNoFill();
	ofSetColor(0,210,255);
	ofEllipse(aPoint.x, aPoint.y, 15,15);
	ofDrawBitmapString( ofToString( whichVertex ), aPoint.x+20, aPoint.y-10);

	
	
	/*
	int nCurvatures = handContourNiceCurvatures.size();
	if (nCurvatures > 0){
		int whichCurvatureIndex = ((int)(mx))%nCurvatures;
		ofVec2f aPoint = handContourResampled [ whichCurvatureIndex ];
		
		ofNoFill();
		ofSetColor(0,160,255);
		ofEllipse(aPoint.x, aPoint.y, 15,15);
		ofDrawBitmapString( ofToString( whichCurvatureIndex ), aPoint.x+20, aPoint.y-10);
	}
	 */
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
		float eigenDataOrientation = calcEigenvector ( matrix2x2_00, matrix2x2_01, matrix2x2_10, matrix2x2_11 );
		orientation  = eigenDataOrientation;
	}
	
	// orientedness is calculated but not returned.
	return orientation;
}

//============================================================
float HandContourAnalyzerAndMeshBuilder::calcEigenvector (float matrix_00, float matrix_01,
														 float matrix_10, float matrix_11 ) {
	
	//this function takes a 2x2 matrix, and returns a pair of angles which are the eigenvectors
	float A = matrix_00;
	float B = matrix_01;
	float C = matrix_10;
	float D = matrix_11;
	
	float eigenDataOrientation;
	float eigenDataEigenValue;
	
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
			eigenDataOrientation = 0;
			eigenDataEigenValue  = 0;
		}
		else {
			float orientedBoxOrientation = atan2f ( (1.0 / magnitude2), (factor2 / magnitude2));
			float orientedBoxEigenvalue  = logf (1.0+root2); // orientedness
			eigenDataOrientation = orientedBoxOrientation;
			eigenDataEigenValue  = orientedBoxEigenvalue;
		}
	}
	else {
		eigenDataOrientation = 0;
		eigenDataEigenValue  = 0;
	}
	
	return eigenDataOrientation;
}


//============================================================
void HandContourAnalyzerAndMeshBuilder::prepareContours (ofPolyline inputContour, cv::Point2f inputCentroid){
	
	// Clear the main output Mesh, and copy in the new information (contour and centroid).
	handMesh.clear();
	handContourRaw = inputContour;
	handCentroid.set(inputCentroid.x, inputCentroid.y);
	
	// handContourFiltered is handContourRaw resampled, then smoothed, then resampled.
	handContourResampled	= handContourRaw.getResampledByCount(DESIRED_N_CONTOUR_POINTS);//Spacing(2.0); //
	handContourNice			= handContourResampled.getSmoothed(5.0);
	handContour1			= handContourResampled.getSmoothed(contourSmoothingSize);
	handContourFiltered		= handContour1.getResampledByCount(DESIRED_N_CONTOUR_POINTS);
	
	// compute handRadius;
	handRadius = computeHandRadius (handContourFiltered);	
}


//============================================================
void HandContourAnalyzerAndMeshBuilder::locateFingerTips(){
	
	
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
	fingerTipContourIndices.clear();
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
			fingerTipContourIndices.push_back( indexOfClosest);
		}
	}
	
	
	// Sort the fingerTipContourIndices by their angles around the centroid.
	vector< pair<float, int> > fingertipAngleIndexPairs;
	for (int i=0; i<fingerTipContourIndices.size(); i++){
		int indexInHandContourNice = fingerTipContourIndices[i];
		
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
	
	// reshuffle the fingerTipContourIndices based on that sorting.
	fingerTipContourIndices.clear();
	for (int i=0; i<fingertipAngleIndexPairs.size(); i++){
		int indexOfSortedFingertip = fingertipAngleIndexPairs[i].second;
		fingerTipContourIndices.push_back (indexOfSortedFingertip);
	}
	
	// This is temporary, and will be clobbered later during our refinement stage.
	fingerTipPoints.clear();
	for (int i=0; i<fingerTipContourIndices.size(); i++){
		int indexOfSortedFingertip = fingerTipContourIndices[i];
		ofVec2f aSortedFingertip = handContourNice [ indexOfSortedFingertip ];
		fingerTipPoints.push_back (aSortedFingertip);
	}
	 
}


//============================================================
void HandContourAnalyzerAndMeshBuilder::locateFingerCrotches(){

	//--------------------------------------
	// FIND FINGER CROTCHES
	handContourNiceCurvatures = buildCurvatureAnalysis (handContourNice, sampleOffset);
	handContourPossibleCrotchIndices = findCrotches (handContourNiceCurvatures, crotchAngleCutoff, crotchNeighborDistance);

	fingerCrotchPointsTmp.clear();
	handContourCrotchIndicesTmp.clear();
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
}


//============================================================
void HandContourAnalyzerAndMeshBuilder::identifyThumbCrotchAndHandType (){
	
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
	
	// Store the (sorted) crotch points
	fingerCrotchPoints.clear();
	handContourCrotchIndicesSorted.clear();
	for (int i=0; i<crotchParametrizationIndexPairs.size(); i++){
		int indexOfSortedCrotch = crotchParametrizationIndexPairs[i].second;
		ofVec2f aSortedCrotch = handContourNice [ indexOfSortedCrotch ];
		handContourCrotchIndicesSorted.push_back (indexOfSortedCrotch);
		fingerCrotchPoints.push_back (aSortedCrotch);
	}
	
	// Score the crotches by their distance to the centroid, and to each other.
	// The highest score is most likely the thumb, which is the furthest from all the others.
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
	
	if (indexOfThumbCrotch == 3){
		currentHandType	= HAND_RIGHT;
	} else if (indexOfThumbCrotch == 0){
		currentHandType	= HAND_LEFT;
	} else {
		currentHandType	= HAND_ERROR;
	}
}

//============================================================
void HandContourAnalyzerAndMeshBuilder::computeHandOrientationAndSideLine(){
	
	handOrientation = getOrientation (handContourNice.getVertices(), handCentroid);
	
	float dq = handRadius * 0.10;
	float qx = handCentroid.x + dq * sinf(handOrientation);
	float qy = handCentroid.y + dq * cosf(handOrientation);
	
	float sp1x = qx - handRadius * cosf(handOrientation);
	float sp1y = qy + handRadius * sinf(handOrientation);
	float sp2x = qx + handRadius * cosf(handOrientation);
	float sp2y = qy - handRadius * sinf(handOrientation);
	sideLineP1.set(sp1x, sp1y);
	sideLineP2.set(sp2x, sp2y);
}

//============================================================
void HandContourAnalyzerAndMeshBuilder::locatePinkySide(){
	
	if (fingerTipContourIndices.size() >= 2){ // should be >=7, really
	
		// Search for the pinky side.
		contourIndexOfPinkySide = fingerTipContourIndices[ 0 ]; // initial (very) bad guess
		if (currentHandType == HAND_RIGHT){
			
			int nPointsOnContour			 = handContourNice.size();
			int contourIndexOfPinkyTip       = fingerTipContourIndices[ 0 ];
			int contourIndexOfPinkysideWrist = fingerTipContourIndices[ fingerTipContourIndices.size() - 1];
			
			int startIndex = contourIndexOfPinkysideWrist;
			int endIndex   = contourIndexOfPinkyTip;
			if (contourIndexOfPinkyTip < contourIndexOfPinkysideWrist) {    // which it will almost never be
				endIndex = contourIndexOfPinkysideWrist + nPointsOnContour; // thus exceeding the bounds; mod it later!
			}
			
			// Search for the point on contour handContourNice,
			// between contourIndexOfPinkyTip and contourIndexOfPinkysideWrist,
			// which is closest to the sideLine.
			float leastDistance = 99999;
			int   contourIndexOfLeastDistantPoint = max(0, startIndex-1);
			
			for (int i=startIndex; i<endIndex; i++){
				int safeIndex = i%nPointsOnContour; // bounds-safe index modding happens here
				ofVec2f ithPoint = handContourNice[safeIndex];
				float dist = distanceFromPointToLine (sideLineP1, sideLineP2, ithPoint);
				if (dist < leastDistance){
					leastDistance = dist;
					contourIndexOfLeastDistantPoint = safeIndex;
				}
			}
			contourIndexOfPinkySide = contourIndexOfLeastDistantPoint;
			
		} else if (currentHandType == HAND_LEFT){
			// handle the left hand another day.
		}
		
	} else {
		// we seem to lack for fingertips.
		ofLog(OF_LOG_NOTICE, "Hand lacks sufficient fingers to detect pinky side vertex.");
		currentHandType == HAND_ERROR;
	}
}


//============================================================
void HandContourAnalyzerAndMeshBuilder::locatePointerSide(){
	
	if ((fingerTipContourIndices.size() >= 4) &&
		(handContourCrotchIndicesSorted.size() >= 4)){
	
		// Search for the POINTER side.
		if (currentHandType == HAND_RIGHT){
			
			contourIndexOfPointerSide = fingerTipContourIndices[3]; // initial (very) bad guess
			
			int nPointsOnContour				= handContourNice.size();
			int contourIndexOfPointerTip		= fingerTipContourIndices[3];
			int contourIndexOfITCrotch			= handContourCrotchIndicesSorted[3];
			
			int startIndex = contourIndexOfPointerTip;
			int endIndex   = contourIndexOfITCrotch;
			
			if (startIndex > endIndex) {      // which it will almost never be
				endIndex += nPointsOnContour; // thus exceeding the bounds; mod it later!
			}
			
			// Search for the point on contour handContourNice,
			// between contourIndexOfPointerTip and contourIndexOfITCrotch (index-thumb crotch)
			// which is closest to the sideLine.
			float leastDistance = 99999;
			int   contourIndexOfLeastDistantPoint = max(0, startIndex-1);
			
			for (int i=startIndex; i<endIndex; i++){
				int safeIndex = i%nPointsOnContour; // bounds-safe index modding happens here
				ofVec2f ithPoint = handContourNice[safeIndex];
				float dist = distanceFromPointToLine (sideLineP1, sideLineP2, ithPoint);
				if (dist < leastDistance){
					leastDistance = dist;
					contourIndexOfLeastDistantPoint = safeIndex;
				}
			}
			contourIndexOfPointerSide = contourIndexOfLeastDistantPoint;
			
		} else if (currentHandType == HAND_LEFT){
			// handle the left hand another day.
		}
	
	} else {
		// we seem to lack for fingertips.
		ofLog(OF_LOG_NOTICE, "Hand lacks sufficient fingers to detect pointer side vertex."); 
		currentHandType == HAND_ERROR;
	}
	
}


//============================================================
void HandContourAnalyzerAndMeshBuilder::locateThumbBase(){
	int nFingerTipIndices = fingerTipContourIndices.size();
	if (nFingerTipIndices >= 7) {
		contourIndexOfThumbBase = locateContourFeature (fingerTipContourIndices [4], // thumb tip
													   fingerTipContourIndices [nFingerTipIndices - 2], 0.05, 0.00, true);
	}
}

//============================================================
void HandContourAnalyzerAndMeshBuilder::locatePalmBase(){
	int nFingerTipIndices = fingerTipContourIndices.size();
	if (nFingerTipIndices >= 7) {
		contourIndexOfPalmBase = locateContourFeature (fingerTipContourIndices[nFingerTipIndices - 1],
													   contourIndexOfPinkySide, 0.00, 0.50, true);
	}
}

//============================================================
int HandContourAnalyzerAndMeshBuilder::locateContourFeature(int startFeatureIndex,
															int endFeatureIndex,
															float startFeatureIndexOffsetPercent,
															float endFeatureIndexInsetPercent,
															bool bConcave){
		
	// search for the point with the highest or lowest curvature
	// located between startFeatureIndex and endFeatureIndex
	
	int nPointsOnContour  = handContourNice.size();
	int indexOfFeature = 0;
	
	if (currentHandType == HAND_RIGHT){
		
		// Clean up the range limits
		int startIndex = startFeatureIndex;
		int endIndex   = endFeatureIndex;
		if (startIndex > endIndex) {     
			endIndex += nPointsOnContour; // thus exceeding the bounds; mod it later.
		}
		// and limit the search to a certain percentage of the range.
		int startIndexOffset = (int)(startFeatureIndexOffsetPercent * (endIndex-startIndex));
		int endIndexOffset   = (int)(endFeatureIndexInsetPercent    * (endIndex-startIndex));
		startIndex = startIndex + startIndexOffset;
		endIndex   = endIndex   - endIndexOffset;
		indexOfFeature = ((startIndex + endIndex)/2)%nPointsOnContour; // a bad initial guess
		
		if (bConcave){
			float greatestNegativeCurvature = 0;
			for (int i=startIndex; i<endIndex; i++){
				int safeIndex = i%nPointsOnContour; // bounds-safe index modding
				float aCurvature = handContourNiceCurvatures[safeIndex];
				if (aCurvature < greatestNegativeCurvature){
					greatestNegativeCurvature = aCurvature;
					indexOfFeature = safeIndex;
				}
			}
			
		} else {
			float greatestPositiveCurvature = 0;
			for (int i=startIndex; i<endIndex; i++){
				int safeIndex = i%nPointsOnContour; // bounds-safe index modding
				float aCurvature = handContourNiceCurvatures[safeIndex];
				if (aCurvature > greatestPositiveCurvature){
					greatestPositiveCurvature = aCurvature;
					indexOfFeature = safeIndex;
				}
			}
		}
		
	} else {
		// handle the left hand another day.
		indexOfFeature = 0; 
		
	}
	
	return indexOfFeature;
}


//============================================================
void HandContourAnalyzerAndMeshBuilder::locateThumbKnuckle(){
	
	// search for the point with the highest curvature
	// located between contourIndexOfThumbTip and contourIndexOfThumbBase
	
	int nFingerTipIndices = fingerTipContourIndices.size();
	int nPointsOnContour  = handContourNice.size();
	
	if (nFingerTipIndices >= 7) {
		
		// Search for the POINTER side.
		if (currentHandType == HAND_RIGHT){
			
			int contourIndexOfThumbTip = fingerTipContourIndices[4];
			
			
			int startIndex = contourIndexOfThumbTip; 
			int endIndex   = contourIndexOfThumbBase;
			if (startIndex > endIndex) {      // which it will almost never be
				endIndex += nPointsOnContour; // thus exceeding the bounds; mod it later!
			}
			// limit the search to the middle 30% of the range.
			int startIndexOffset = (int)(0.35 * (endIndex-startIndex));
			int endIndexOffset   = (int)(0.30 * (endIndex-startIndex));
			startIndex = startIndex + startIndexOffset;
			endIndex   = endIndex   - endIndexOffset;
			
			float greatestCurvature = 0;
			int indexOfFeature = ((startIndex + endIndex)/2)%nPointsOnContour; // a bad guess
			
			for (int i=startIndex; i<endIndex; i++){
				int safeIndex = i%nPointsOnContour; // bounds-safe index modding
				float aCurvature = handContourNiceCurvatures[safeIndex];
				if (aCurvature > greatestCurvature){
					greatestCurvature = aCurvature;
					indexOfFeature = safeIndex;
				}
			}
			contourIndexOfThumbKnuckle = indexOfFeature;
			
		} else {
			// handle the left hand another day.
			
		}
		
	} else {
		
		// we seem to lack for fingertips.
		ofLog(OF_LOG_NOTICE, "Hand lacks sufficient fingers to detect palm base vertex.");
		currentHandType == HAND_ERROR;
	}
}


//============================================================
void HandContourAnalyzerAndMeshBuilder::computeHandmarkStatistics(){
	
	// Compute running averages and running standard deviations for each point.

	
	if ((currentHandType == HAND_LEFT) || (currentHandType == HAND_RIGHT)){
		for (int i=0; i<N_HANDMARKS; i++){
	
			if (currentHandExistsFrameCount == 1){
				Handmarks[i].pointHistory.clear();
				Handmarks[i].pointAvg.set     (0,0);
				Handmarks[i].pointStDv.set    (0,0);
				
            } else {
				
				Handmarks[i].pointHistory.push_back( Handmarks[i].point );
				if ( Handmarks[i].pointHistory.size() > HANDMARK_HISTORY_LENGTH){
					Handmarks[i].pointHistory.erase (Handmarks[i].pointHistory.begin());
				}
				
				// compute average 
				int nValues = Handmarks[i].pointHistory.size();
				float mx = 0;
				float my = 0;
				for (int j=0; j<nValues; j++){
					float dx = Handmarks[i].pointHistory[j].x;
					float dy = Handmarks[i].pointHistory[j].y;
					mx += dx;
					my += dy;
				}
				mx /= (float)nValues;
				my /= (float)nValues;
				Handmarks[i].pointAvg.set(mx,my); // radius, angle
				
				// compute standard deviation
				float sx = 0;
				float sy = 0;
				for (int j=0; j<nValues; j++){
					float dx = Handmarks[i].pointHistory[j].x;
					float dy = Handmarks[i].pointHistory[j].y;
					sx += (dx-mx)*(dx-mx);
					sy += (dy-my)*(dy-my);
				}
				sx = sqrtf (sx/(float)nValues);
				sy = sqrtf (sy/(float)nValues);
				Handmarks[i].pointStDv.set(sx,sy);
				
				
            }
	
			
		}
	} else {
		
		currentHandExistsFrameCount = 0;
		for (int i=0; i<N_HANDMARKS; i++){
			Handmarks[i].pointHistory.clear();
			Handmarks[i].pointAvg.set     (0,0);
			Handmarks[i].pointStDv.set    (0,0);
		}
		
	}
}



//============================================================
void HandContourAnalyzerAndMeshBuilder::refineHandmarksBasedOnMotionStDev(){
	
	// For each Handmark
	for (int i=0; i<N_HANDMARKS; i++){
		if (Handmarks[i].type != HANDMARK_INVALID){
			
			// If it has a history longer than two frames
			int np = Handmarks[i].pointHistory.size();
			if (np > 2){
				
				// Check if the distance from the most recent point, to the 2nd-most-recent point,
				// (the "jump") is greater than ~2 standard deviations in the recent history of its position.
				float px    = Handmarks[i].point.x;
				float py    = Handmarks[i].point.y;
				float prevx = Handmarks[i].pointHistory[np-1].x;
				float prevy = Handmarks[i].pointHistory[np-1].y;
				
				float rx = Handmarks[i].pointStDv.x;
				float ry = Handmarks[i].pointStDv.y;
				float rh = sqrtf(rx*rx + ry*ry);
				
				float dx = px - prevx;
				float dy = py - prevy;
				float dh = sqrtf(dx*dx + dy*dy);
				
				// If the jump exceeds the motion threshold,
				if (dh > (tooMuchMotionThresholdInStDevs * rh)){
					
					// If the jump is within reason
					float unreasonableJumpAmount = handRadius * 0.2;
					if (dh < unreasonableJumpAmount){
						// Consider the previous point, P, and the current point Q.
						// Compute a position some percentage (halfway?) between P & Q; 
						// Find the nearest point on the handContourNice to that position,
						// With the additional constraint that it lie within LIMIT points of Q. 
						// Reassign Handmarks[i].point and Handmarks[i].index to that result.
						
						float A = indexInterpolationAlpha;
						float B = 1.0-A;
						
						float mx = A*px + B*prevx;
						float my = A*py + B*prevy;
						
						int nPointsOnContour = handContourNice.size(); 
						int indexOfCurrentPoint = Handmarks[i].index;
						int indexSearchLimit = DESIRED_N_CONTOUR_POINTS / 25; 
						int startIndex = (indexOfCurrentPoint - indexSearchLimit + nPointsOnContour)%nPointsOnContour;
						int endIndex   = (indexOfCurrentPoint + indexSearchLimit + nPointsOnContour)%nPointsOnContour;
						if (startIndex > endIndex) {      // which it will almost never be
							endIndex += nPointsOnContour; // thus exceeding the bounds; mod it later!
						}
						
						int indexOfClosest = 0;
						float minDistance = 99999;
						for (int j=startIndex; j<endIndex; j++){
							int safeIndex = j%nPointsOnContour; 
							ofVec2f pointOnNiceContour = handContourNice[j];
							float ijDist = ofDist(mx, my, pointOnNiceContour.x, pointOnNiceContour.y);
							if (ijDist < minDistance){
								minDistance = ijDist;
								indexOfClosest = j;
							}
						}
						
						Handmarks[i].point = handContourNice[indexOfClosest];
						Handmarks[i].index = indexOfClosest;
						
					}
					// Else if the jump is not within reason (for example, fingertips could have been misnumbered)
					else {
						
						
					}
					
				} 
			}
		}
	}
}



//============================================================
void HandContourAnalyzerAndMeshBuilder::buildMesh(){
	handMesh.clear();
	// handMesh.disableNormals();
	//handMesh.addVertex(ofPoint(x,y,0));

	
	
	
	/*
	
	int nContourPoints = handContourNice.size();
	for (int f=0; f<5; f++){
		
		// find the contour indices over which to search
		int findex0 = (fingerTipIndices[f] - 1 + N_HANDMARKS)%N_HANDMARKS;
		int findex1 = (fingerTipIndices[f] + 1 + N_HANDMARKS)%N_HANDMARKS;
		int index0 = Handmarks[findex0].index;
		int index1 = Handmarks[findex1].index;
		if (index0 > index1){ index1 += nContourPoints; }
	
	}
	*/
	
	
	/*
	 //add one vertex to the mesh for each pixel
	 for (int y = 0; y < height; y++){
	 for (int x = 0; x<width; x++){
	 mainMesh.addVertex(ofPoint(x,y,0));
	 }
	 }
	 */
	
	/*
	for (int y = 0; y<height-1; y++){
		for (int x=0; x<width-1; x++){
			mainMesh.addIndex(x+y*width);				// 0
			mainMesh.addIndex((x+1)+y*width);			// 1
			mainMesh.addIndex(x+(y+1)*width);			// 10
			
			mainMesh.addIndex((x+1)+y*width);			// 1
			mainMesh.addIndex((x+1)+(y+1)*width);		// 11
			mainMesh.addIndex(x+(y+1)*width);			// 10
		}
	}
	*/
	
	//handMesh.addTriangle(<#ofIndexType index1#>, <#ofIndexType index2#>, <#ofIndexType index3#>);
	//handMesh.addVertex(<#const ofVec3f &v#>);
	
	// mainMesh.drawWireframe();
	// mainMesh.drawFaces();
}




//============================================================
void HandContourAnalyzerAndMeshBuilder::assembleHandmarksPreliminary(){
	
	
	
	//--------------------------------------
	// ASSEMBLE HANDMARKS
	// Assumes indexOfThumbCrotch == 3 ! >> RIGHT HANDS ONLY FOR NOW
	// Assume nFingerTipIndices >= 7!
	
	int nFingerTipIndices			= fingerTipContourIndices.size();
	int contourIndexOfPinkyTip		= fingerTipContourIndices[0];
	int contourIndexOfRingTip		= fingerTipContourIndices[1];
	int contourIndexOfMiddleTip		= fingerTipContourIndices[2];
	int contourIndexOfPointerTip	= fingerTipContourIndices[3];
	int contourIndexOfThumbTip		= fingerTipContourIndices[4];
	
	int contourIndexOfPRCrotch		= handContourCrotchIndicesSorted[0];
	int contourIndexOfRMCrotch		= handContourCrotchIndicesSorted[1];
	int contourIndexOfMICrotch		= handContourCrotchIndicesSorted[2];
	int contourIndexOfITCrotch		= handContourCrotchIndicesSorted[3];
	
	int contourIndexOfPinkysideWrist = fingerTipContourIndices[nFingerTipIndices - 1];
	int contourIndexOfThumbsideWrist = fingerTipContourIndices[nFingerTipIndices - 2];
	
	if (currentHandType == HAND_LEFT){
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
	
	Handmarks[HANDMARK_POINTER_SIDE].index		= contourIndexOfPointerSide;
	Handmarks[HANDMARK_POINTER_SIDE].point		= handContourNice [contourIndexOfPointerSide];
	Handmarks[HANDMARK_POINTER_SIDE].type		= HANDMARK_POINTER_SIDE;
	
	Handmarks[HANDMARK_IT_CROTCH].index			= contourIndexOfITCrotch;
	Handmarks[HANDMARK_IT_CROTCH].point			= handContourNice [contourIndexOfITCrotch];
	Handmarks[HANDMARK_IT_CROTCH].type			= HANDMARK_IT_CROTCH;
	
	Handmarks[HANDMARK_THUMB_TIP].index			= contourIndexOfThumbTip;
	Handmarks[HANDMARK_THUMB_TIP].point			= handContourNice [contourIndexOfThumbTip];
	Handmarks[HANDMARK_THUMB_TIP].type			= HANDMARK_THUMB_TIP;
	
	Handmarks[HANDMARK_THUMB_KNUCKLE].index		= contourIndexOfThumbKnuckle;
	Handmarks[HANDMARK_THUMB_KNUCKLE].point		= handContourNice [contourIndexOfThumbKnuckle];
	Handmarks[HANDMARK_THUMB_KNUCKLE].type		= HANDMARK_THUMB_KNUCKLE;
	
	Handmarks[HANDMARK_THUMB_BASE].index		= contourIndexOfThumbBase;
	Handmarks[HANDMARK_THUMB_BASE].point		= handContourNice [contourIndexOfThumbBase];
	Handmarks[HANDMARK_THUMB_BASE].type			= HANDMARK_THUMB_BASE;
	
	Handmarks[HANDMARK_THUMBSIDE_WRIST].index	= contourIndexOfThumbsideWrist;
	Handmarks[HANDMARK_THUMBSIDE_WRIST].point	= handContourNice [contourIndexOfThumbsideWrist];
	Handmarks[HANDMARK_THUMBSIDE_WRIST].type	= HANDMARK_THUMBSIDE_WRIST;
	
	Handmarks[HANDMARK_PINKYSIDE_WRIST].index	= contourIndexOfPinkysideWrist;
	Handmarks[HANDMARK_PINKYSIDE_WRIST].point	= handContourNice [contourIndexOfPinkysideWrist];
	Handmarks[HANDMARK_PINKYSIDE_WRIST].type	= HANDMARK_PINKYSIDE_WRIST;
	
	Handmarks[HANDMARK_PALM_BASE].index			= contourIndexOfPalmBase;
	Handmarks[HANDMARK_PALM_BASE].point			= handContourNice [contourIndexOfPalmBase];
	Handmarks[HANDMARK_PALM_BASE].type			= HANDMARK_PALM_BASE;
	
	Handmarks[HANDMARK_PINKY_SIDE].index		= contourIndexOfPinkySide;
	Handmarks[HANDMARK_PINKY_SIDE].point		= handContourNice [contourIndexOfPinkySide];
	Handmarks[HANDMARK_PINKY_SIDE].type			= HANDMARK_PINKY_SIDE;
	
}




//============================================================
void HandContourAnalyzerAndMeshBuilder::refineFingertips(){
	
	/*
	int fingerTipIndices[] = {
		HANDMARK_PINKY_TIP,
		HANDMARK_RING_TIP,
		HANDMARK_MIDDLE_TIP,
		HANDMARK_POINTER_TIP,
		HANDMARK_THUMB_TIP
	};
	 */
	
	int nContourPoints = handContourNice.size();
	for (int f=0; f<5; f++){
		
		// find the contour indices over which to search
		int findex0 = (fingerTipIndices[f] - 1 + N_HANDMARKS)%N_HANDMARKS;
		int findex1 = (fingerTipIndices[f] + 1 + N_HANDMARKS)%N_HANDMARKS;
		int index0 = Handmarks[findex0].index;
		int index1 = Handmarks[findex1].index;
		if (index0 > index1){ index1 += nContourPoints; }
		
		// get the (local) orientation of the f'th finger (sub)contour
		vector<ofPoint> pts;
		ofVec2f com = ofVec2f(0,0);
		for (int i=index0; i<index1; i++){
			int indexSafe = i%nContourPoints;
			pts.push_back(handContourNice[indexSafe]);
			com += handContourNice[indexSafe];
		}
		com /= (float)(index1 - index0); 
		float angle = getOrientation (pts, com);
		fingerOrientations[f] = angle;
		
		// Rotate the copy of the finger, so that it is axis-aligned
		float newX, newY;
		for (int i=0; i<pts.size(); i++) {
			ofPoint pt = pts[i];
			// translate the point so that its center is on the origin
			pt.x = pt.x - com.x;
			pt.y = pt.y - com.y;
			// rotate the point
			newX = (pt.x * cos(angle)) - (pt.y * sin(angle));
			newY = (pt.x * sin(angle)) + (pt.y * cos(angle));
			// translate the point back
			pt.x = newX + com.x;
			pt.y = newY + com.y;
		}

		// Find the topmost point of the axis-aligned copy of the finger. 
		float mostTop =  99999;
		int indexOfTopmost = 0;
		for (int i=0; i<pts.size(); i++) {
			ofPoint pt = pts[i];
			if (pt.y < mostTop){
				mostTop = pt.y;
				indexOfTopmost = i;
			}
		}
		
		// Reassign the tip to that location. 
		indexOfTopmost = (indexOfTopmost + index0)%nContourPoints;
		Handmarks[fingerTipIndices[f]].index = indexOfTopmost;
		Handmarks[fingerTipIndices[f]].point = handContourNice[indexOfTopmost];

		
	}
	

	
}




//============================================================
ofMesh& HandContourAnalyzerAndMeshBuilder::getMesh(){
	return handMesh;
}

//============================================================
vector<int>& HandContourAnalyzerAndMeshBuilder::getJoints(){
	return joints;
}

//============================================================
float HandContourAnalyzerAndMeshBuilder::distanceFromPointToLine (ofVec2f linePt1, ofVec2f linePt2,  ofVec2f aPoint){
	// see http://paulbourke.net/geometry/pointlineplane/
	
	float p1x = linePt1.x;
	float p1y = linePt1.y;
	float p2x = linePt2.x;
	float p2y = linePt2.y;
	
	float dx = p2x - p1x;
	float dy = p2y - p1y;
	float lineMag2 = dx*dx + dy*dy;
	
	float p3x  = aPoint.x;
	float p3y  = aPoint.y;
	float u   = ((p3x-p1x)*(p2x-p1x) + (p3y-p1y)*(p2y-p1y)) / lineMag2;
	
	// intersection point
	float inx = p1x + u * (p2x - p1x);
	float iny = p1y + u * (p2y - p1y);
	
	float dist = ofDist (p3x,p3y, inx,iny);
	return dist;
}


	
