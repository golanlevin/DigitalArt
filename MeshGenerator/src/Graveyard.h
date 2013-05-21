//
//  Graveyard.h
//  ofApp
//
//  Created by Golan Levin on 5/20/13.
//
//

#ifndef ofApp_Graveyard_h
#define ofApp_Graveyard_h

// JUST IGNORE THIS STUFF, GOLAN IS PUTTING RANDOM CODE HERE


/*
 //imitate(dst, src);
 //copy (thresholded, thresholdedCleaned);
 */



/*
 int elementSize = 5;
 cv::medianBlur(thresholded, thresholdedCleaned, elementSize);
 */

/*
 if (bDoMorphologicalOpening){
 bool bUseRoundMorphologicalStructuringElement = true;
 cv::Mat structuringElement = Mat();
 if (bUseRoundMorphologicalStructuringElement){
 structuringElement = cv::getStructuringElement(cv::MORPH_ELLIPSE,
 cv::Size(2*elementSize + 1, 2*elementSize + 1),
 cv::Point(elementSize, elementSize) );
 }
 
 cv::erode  (thresholded, tempGrayscaleMat1, structuringElement);
 cv::dilate (tempGrayscaleMat1, tempGrayscaleMat2, structuringElement);
 
 cv::medianBlur(tempGrayscaleMat2, thresholdedCleaned, elementSize);
 
 } else {
 // Just do median only
 cv::medianBlur(thresholded, thresholdedCleaned, elementSize);
 }
 */



/*
 bool bComputeAngleAroundCentroid = false;
 if (bComputeAngleAroundCentroid){
 float angleAroundCentroid[maxNCrotchesToConsider];
 for (int i=0; i<maxNCrotchesToConsider; i++){
 ofVec2f a = fingerCrotchPointsTmp[i];
 ofVec2f b = handCentroid;
 ofVec2f c = ofVec2f(100, 0);
 a -= b;
 float angle = a.angle(c);
 angleAroundCentroid[i] = (angle < 0)? angle + 360: angle;
 }
 }
 */


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


#endif
