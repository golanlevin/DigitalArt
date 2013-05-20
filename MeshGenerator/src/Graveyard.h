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





#endif
