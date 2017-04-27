#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED

#include <sys/time.h>
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
#define MODE_EMBARQUE 0
#define MODE_EVALUATION 1
#define MODE_DEVELOPPEMENT 2


extern std::string filepathGlobal;
extern std::string filepathRed;
extern std::string filepathBlue;
extern std::string fileFormat;

typedef struct RecognizedShape
{
	cv::Mat matrix;
	std::string label;
	cv::Rect boundingRect;
} RecognizedShape;

typedef struct Parameters
{
	int mode;
	std::string list, classifier, annotation;
	int counts[3];			//0 : RedCounts  1 : GlobalCounts  2 : BlueCounts
	int debug;
	bool noPipe, noSave, show;
	float custom1, custom2;
} Parameters;

//Applications
int applicationEmbarquee(Parameters parameters);
int applicationDeveloppement(Parameters parameters) ;
int applicationDebug(Parameters parameters);
int applicationEvaluation(Parameters parameters);

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
cv::Mat blueMask_(cv::Mat image, float threshold);
cv::Mat redMask_(cv::Mat image, float threshold);
cv::Mat SetCustomMask(cv::Mat hsv, uchar lower[], uchar upper[]);

//main.cpp
bool save_image(cv::Mat frame, RecognizedShape shape, std::string color, int &count, std::string &filename, bool verbose = true);
bool canSave(time_t &start, time_t &end, int interval);
float getFPS(time_t &timer_begin, time_t &timer_end, int &nCount);
void displayRecognizedShapes(cv::Mat &frame, std::vector<RecognizedShape> &shapes);


#endif
