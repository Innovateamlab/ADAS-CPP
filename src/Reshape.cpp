#include "header.hpp"

using namespace std; 
using namespace cv;

cv::Rect resizeToSquare(cv::Rect r)
{
	cv::Rect s;
	
	// Same x & y for the corrected rectangle (square)
	s.x = r.x; s.y=r.y;
	// Set les h et w à la même taille
	if (r.width > r.height)
		{
			s.width=r.height;
			s.height = s.width;
		}
			
	else 
	{
		s.height = r.width;
		s.width = s.height;
		
	}
		
	return s;
}


cv::Mat SetSizeSquareMat(cv::Mat imageCropped, unsigned short int NewSize)
{
	cv::Mat imageCroppedResize;
	// Resize to NewSize x NewSize
	resize(imageCropped,imageCroppedResize, Size(NewSize,NewSize),0,0,INTER_CUBIC);
	
	return imageCroppedResize;	
}


// what if nb of corner different from 3 or 4 ?

cv::Mat FeatureRotation(cv::Mat img_cropped, cv::Rect s, int x_p, int NbCorners)
{
	cv::Mat dst;
	float Angle = 0;
	if (NbCorners == 4)
	{
		//cout << "\n\napprox : " << approx << "\ns.x " << s.x << "\nx_p : "<< x_p << "\n r.width : "<< r.width << endl;
		//cout << "\napprox : " << approx << "\ncontours" << contours[i] << endl;
		int x_rot = (x_p - (int)(s.x + s.width/2));
		if (x_rot >= 0)
			Angle = 47 - (180/3.1415)*acos((float)((float)x_rot/((float)((s.height)/sqrt(2)))));
		else 
			Angle = 90 + 45 - (180/3.1415)*acos((float)((float)x_rot/((float)((s.height)/sqrt(2)))));
		//cout<<"\nAngle : " << Angle << "\nx_rot : "<< x_rot << "\ns.h / 2 : "<<(s.height)/2 << endl;
					
		cv::Point2f center(s.width/2.0, s.height/2.0);
		cv::Mat rot = cv::getRotationMatrix2D(center,Angle,1.0);
		cv::Rect bbox = cv::RotatedRect(center, img_cropped.size() ,Angle).boundingRect();
		rot.at<double>(0,2) += bbox.width/2.0 - center.x;
		rot.at<double>(1,2) +=bbox.height/2.0 - center.y;
		cv::warpAffine(img_cropped, dst, rot, bbox.size());
	
		return dst;
	}
	else if (NbCorners == 3)
	{
		//cout << "\n\napprox : " << approx << "\ns.x " << s.x << "\nx_p : "<< x_p << "\n r.width : "<< r.width << endl;
		//cout << "\napprox : " << approx << "\ncontours" << contours[i] << endl;
		int x_rot = (x_p - (int)(s.x + s.width/2));
		Angle = 90 - (180/3.1415)*acos((float)((float)x_rot/((float)(sqrt((s.height*s.height +s.width*s.width)/4 )))));
		
		//cout<<"\nAngle : " << Angle << "\nx_rot : "<< x_rot << "\ns.h / 2 : "<<(s.height)/2 << endl;
					
		cv::Point2f center(s.width/2.0, s.height/2.0);
		cv::Mat rot = cv::getRotationMatrix2D(center,Angle,1.0);
		cv::Rect bbox = cv::RotatedRect(center, img_cropped.size() ,Angle).boundingRect();
		rot.at<double>(0,2) += bbox.width/2.0 - center.x;
		rot.at<double>(1,2) +=bbox.height/2.0 - center.y;
		cv::warpAffine(img_cropped, dst, rot, bbox.size());
		
		return dst;
	}
	
#if	DEBUG>1
	cout << "NbCorners différents de 3 et 4\n"; 
#endif	
	
	return dst;
}
