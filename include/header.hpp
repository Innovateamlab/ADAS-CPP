#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED

#include <ctime>
#include <cmath>
#include <fstream>
#include <iostream>
#include </usr/local/include/raspicam/raspicam_cv.h>
#include </usr/local/include/raspicam/raspicam.h>
#include </usr/local/include/raspicam/raspicam_still_cv.h>
#include </usr/local/include/raspicam/raspicamtypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include </usr/local/include/opencv2/opencv.hpp>



#define DEBUG 0

typedef struct RecognizedShape
{
	cv::Mat matrix;
	std::string label;
	cv::Rect boundingRect;
} RecognizedShape;


//Utils.cpp
void MatToTxtFile(cv::Mat image);
unsigned char MatToUChar(cv::Mat image);
double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);
void setLabel(cv::Mat& im, const std::string label, cv::Rect boundingRect);
void displayShape(cv::Mat &frame, RecognizedShape &shape);

//Reshape.cpp
cv::Rect resizeToSquare(cv::Rect r);
cv::Mat SetSizeSquareMat(cv::Mat imageCropped, unsigned short int NewSize);
cv::Mat FeatureRotation(cv::Mat img_cropped, cv::Rect s, int x_p, int NbCorners);

//ShapeDetector.cpp
std::vector<RecognizedShape> shapeDetectorBlue(cv::Mat source, std::vector<std::vector<cv::Point> > contours);
std::vector<RecognizedShape> shapeDetectorRed(cv::Mat source, std::vector<std::vector<cv::Point> > contours); 

//Mask.cpp
cv::Mat preprocessing(cv::Mat image_RGB);
cv::Mat SetBlueMask(cv::Mat hsv);
cv::Mat SetRedMask(cv::Mat hsv);
cv::Mat SetCustomMask(cv::Mat hsv, uchar lower[], uchar upper[]);

#endif
