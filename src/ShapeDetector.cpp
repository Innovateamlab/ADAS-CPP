#include "header.hpp"

using namespace std;
using namespace cv;

// Our lamps:
// (These are wiringPi pin numbers)

bool isTriangle(Mat &source, std::vector<cv::Point> &approx, std::vector<std::vector<cv::Point> > &contours);
bool isSquare(Mat &source, std::vector<cv::Point> &approx, std::vector<std::vector<cv::Point> > &contours);

std::vector<RecognizedShape> shapeDetectorBlue(cv::Mat source, std::vector<std::vector<cv::Point> > contours) 
{
	// Find shape
	cv::Mat img_cropped;
	std::vector<cv::Point> approx;
	cv::Mat dst = source.clone();
	int thickness = 2;
	cv::Mat imgC;
	
	std::vector<RecognizedShape> recognizedShapes;
	RecognizedShape recognizedShape;
	
	for (int i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);		

		// Skip small or non-convex objects 
		if (std::fabs(cv::contourArea(approx)) < 300 || !cv::isContourConvex(approx))
			continue;

		Rect boundingRect = cv::boundingRect(approx);
		boundingRect = resizeToSquare(boundingRect);
		
		if(boundingRect.width <= 0.15*source.cols)
			continue;
		
		Mat img_cropped = source(boundingRect);

		/// Square
		if (isSquare(source,approx,contours)) 
		{
			recognizedShape.matrix = img_cropped;
			recognizedShape.label = "BLUE_RECT";
			recognizedShape.boundingRect = boundingRect;
				
			recognizedShapes.push_back(recognizedShape);
		}
		/// Circle
		else
		{	
			double area = cv::contourArea(approx);
			int radius = boundingRect.width / 2;

			if (std::abs(1 - ((double)boundingRect.width / boundingRect.height)) <= 0.2 && std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
			{
				recognizedShape.matrix = img_cropped;
				recognizedShape.label = "BLUE_CIRC";
				recognizedShape.boundingRect = boundingRect;
				
				recognizedShapes.push_back(recognizedShape);
			}
		}
	}
		
	return recognizedShapes;
}

std::vector<RecognizedShape> shapeDetectorRed(cv::Mat source, std::vector<std::vector<cv::Point> > contours) 
{
	cv::Mat img_cropped;
	std::vector<cv::Point> approx;
	cv::Mat dst = source.clone();
	cv::Mat image_gray;
	
	std::vector<RecognizedShape> recognizedShapes;
	RecognizedShape recognizedShape;
	
	
	//Contours avec les couleurs
	for (int i = 0; i < contours.size(); i++)
	{
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.04, true);

		if (std::fabs(cv::contourArea(approx)) < 300 || !cv::isContourConvex(approx))
			continue;
			
		Rect boundingRect = cv::boundingRect(approx);
		boundingRect = resizeToSquare(boundingRect);
		
		if(boundingRect.width <= 0.15*source.cols)
			continue;
		
		Mat img_cropped = source(boundingRect);
	
		if(isTriangle(source,approx,contours)){
			
			recognizedShape.matrix = img_cropped;
			recognizedShape.label = "RED_TRI";
			recognizedShape.boundingRect = boundingRect;
			
			recognizedShapes.push_back(recognizedShape);
		}
		//Cercle
		else
		{
			double area = cv::contourArea(approx);
			int radius = boundingRect.width / 2;

			if (abs(1 - ((double)boundingRect.width / boundingRect.height)) <= 0.2 && abs(1 - (area / (CV_PI * radius * radius))) <= 0.2)
			{
				recognizedShape.matrix = img_cropped;
				recognizedShape.label = "RED_CIRC";
				recognizedShape.boundingRect = boundingRect;
				
				recognizedShapes.push_back(recognizedShape);
			}
		}
		
	}
	
	return recognizedShapes;
}

bool isTriangle(Mat &source, std::vector<cv::Point> &approx, std::vector<std::vector<cv::Point> > &contours)
{
	if (approx.size() == 3)
	{
		//Autres conditions
		
		//Equilateral
		double norm1 = sqrt((approx[0].x-approx[1].x)*(approx[0].x-approx[1].x) + (approx[0].y-approx[1].y)*(approx[0].y-approx[1].y));
		double norm2 = sqrt((approx[1].x-approx[2].x)*(approx[1].x-approx[2].x) + (approx[1].y-approx[2].y)*(approx[1].y-approx[2].y));
		double norm3 = sqrt((approx[2].x-approx[0].x)*(approx[2].x-approx[0].x) + (approx[2].y-approx[0].y)*(approx[2].y-approx[0].y));
		if(abs(norm1-norm2) > 0.2*norm1 || abs(norm2-norm3) > 0.2*norm1 || abs(norm3-norm1) > 0.2*norm1)
			return false;
		
		return true;
	}
	
	
	return false;
}

bool isSquare(Mat &source, std::vector<cv::Point> &approx, std::vector<std::vector<cv::Point> > &contours)
{
	if (approx.size() == 4) 
	{
		cv::Rect r = cv::boundingRect(approx);
		float ar = r.width/r.height;
		//Ratio entre largeur et hauteur
		if(ar >= 0.98 && ar <= 1.02)
		{
			//Autres conditions
			return true;
		}
	}
	
	return false;
}

std::vector<RecognizedShape> applyClassifier(CascadeClassifier &classifier, string label, Mat &image_gray, Mat &image_color)
{
	std::vector<RecognizedShape> list;
	RecognizedShape recognizedShape;
	vector<Rect> signs;
	
	classifier.detectMultiScale(image_gray, signs, 1.12, 5);
	for(size_t i =0;i<signs.size();i++)
	{
		recognizedShape.matrix = image_color(signs[i]);
		recognizedShape.label = label;
		recognizedShape.boundingRect = signs[i];
		
		list.push_back(recognizedShape);
	}
	
	return list;
}


