#include "header.hpp"

using namespace std; 
using namespace cv;


cv::Mat SetBlueMask(cv::Mat hsv)
{
	cv::Mat blueMask, blueMask1, blueMask2;
	uchar blueLower1_data[3] = {0,50,50};
	uchar blueUpper1_data[3] = {15,255,255};
	uchar blueLower2_data[3] = {160,50,50};
	uchar blueUpper2_data[3] = {180,255,255};
	
	cv::Mat blueLower1 = cv::Mat(1,3,CV_8UC1, blueLower1_data);
	cv::Mat blueUpper1 = cv::Mat(1,3,CV_8UC1, blueUpper1_data);
	cv::Mat blueLower2 = cv::Mat(1,3,CV_8UC1, blueLower2_data);
	cv::Mat blueUpper2 = cv::Mat(1,3,CV_8UC1, blueUpper2_data);	

	inRange(hsv, blueLower1, blueUpper1, blueMask1);
	inRange(hsv, blueLower2, blueUpper2, blueMask2);
	blueMask = blueMask1+blueMask2;

	return blueMask;
}

cv::Mat blueMask_(cv::Mat image, float threshold)
{
	Mat_<uchar> mask = Mat::zeros(image.rows, image.cols, CV_8UC1);
	
	float red,green,blue;

	Mat_<Vec3b> _I = image.clone();
	
	for(int i=0;i<image.rows;i++)
	{
		for(int j=0;j<image.cols;j++)
		{
			blue = (float) _I(i,j)[0];
			green = (float) _I(i,j)[1];
			red = (float) _I(i,j)[2];
			
			if(blue/red > threshold && blue/green > threshold)
				mask(i,j) = 255;
			else
				mask(i,j) = 0;
		}
	}

 	
	return mask;
}

cv::Mat redMask_(cv::Mat image, float threshold)
{
	Mat_<uchar> mask = Mat::zeros(image.rows, image.cols, CV_8UC1);
	
	float red,green,blue;

	Mat_<Vec3b> _I = image.clone();

	for(int i=0;i<image.rows;i++)
	{
		for(int j=0;j<image.cols;j++)
		{
			blue = (float) _I(i,j)[0];
			green = (float) _I(i,j)[1];
			red = (float) _I(i,j)[2];
			
			if(red/blue > threshold && red/green > threshold)
				mask(i,j) = 255;
			else
				mask(i,j) = 0;
		}
	}
 	
	return mask;
}


cv::Mat SetRedMask(cv::Mat hsv)
{
	cv::Mat redMask;
	
	//Saturation = 0
	uchar redLower_data[3]= {120,50,50};
	uchar redUpper_data[3] = {135,255,255};
	
	
	//Saturation = 100
	/*uchar redLower_data[3]= {122,50,50};
	uchar redUpper_data[3] = {130,255,255};
	*/
	cv::Mat redLower = cv::Mat(1,3,CV_8UC1, redLower_data);
	cv::Mat redUpper = cv::Mat(1,3,CV_8UC1,redUpper_data);	
		
	inRange(hsv, redLower, redUpper, redMask);

	return redMask;
}

cv::Mat SetCustomMask(cv::Mat hsv, uchar lower[], uchar upper[])
{
	cv::Mat mask;
	
	cv::Mat colorLower = cv::Mat(1,3,CV_8UC1, lower);
	cv::Mat colorUpper = cv::Mat(1,3,CV_8UC1,upper);	
		
	inRange(hsv, colorLower, colorUpper, mask);

	return mask;
}


cv::Mat preprocessing(cv::Mat image_RGB)
{
	//Image preprocessing
	//Median blur
	cv::Mat blur = image_RGB;
	//medianBlur(image_RGB,blur,3);	 
	//cv::imwrite("raspicam_cv_image_blur.jpg",blur);
    //cout<<"Image saved at raspicam_cv_image_blur.jpg"<<endl;
    
    // Convert into HSV
    cv::Mat hsv;
    cvtColor(blur, hsv, CV_RGB2HSV);
    //cv::imwrite("raspicam_cv_image_hsv.jpg",blur);
    //cout<<"Image saved at raspicam_cv_image_hsv.jpg"<<endl;
     //getchar();
     
     return hsv;
}

