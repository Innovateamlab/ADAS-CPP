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
#include <wiringPi.h>


#define DEBUG	0
#define LIGHT_BLUE 2
#define LIGHT_RED 3
#define FAIL 4
typedef struct RecognizedShape
{
	cv::Mat matrix;
	std::string label;
} RecognizedShape;


//Utils.cpp
void MatToTxtFile(cv::Mat image);
unsigned char MatToUChar(cv::Mat image);
double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);
void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour);

//Reshape.cpp
cv::Rect resizeToSquare(cv::Rect r);
cv::Mat SetSizeSquareMat(cv::Mat imageCropped, unsigned short int NewSize);
cv::Mat FeatureRotation(cv::Mat img_cropped, cv::Rect s, int x_p, int NbCorners);

//ShapeDetector.cpp
RecognizedShape shapeDetectorBlue(cv::Mat source, std::vector<std::vector<cv::Point> > contours);
RecognizedShape shapeDetectorRed(cv::Mat source, std::vector<std::vector<cv::Point> > contours); 

//Mask.cpp
cv::Mat preprocessing(cv::Mat image_RGB);
cv::Mat SetBlueMask(cv::Mat hsv);
cv::Mat SetRedMask(cv::Mat hsv);
cv::Mat SetCustomMask(cv::Mat hsv, uchar lower[], uchar upper[]);

//Flags
void test_Flags(int buffer[2*sizeof(int)]);

#endif // MATH_H_INCLUDED
