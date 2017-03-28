#include "header.hpp"

using namespace std; 
using namespace cv;

// Our lamps:
// (These are wiringPi pin numbers)
#define	RED		0
#define	YELLOW	1


void MatToTxtFile(cv::Mat image)
{
	
	int width = image.size().width;
    int height = image.size().height;
    int g;
    unsigned char array[height * width];
	//save array in text  
    ofstream fichier("array.txt", ios::out | ios::trunc);
    if (fichier)
    {
		//fichier << "[ ";
		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{ 
				g = array[width * j + i];
				fichier << g << " ";
			}
		} 
		//fichier << " ]";
	}
	fichier.close();
}

double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

unsigned char MatToUChar(cv::Mat image)
{
	//Convert cv::Mat to uchar (0 à 255)
    int width = image.size().width;
    int height = image.size().height;
    unsigned char *data = (unsigned char*)(image.data) ; // data is the pointer of the elements of image's pixel data 
	unsigned char array[height * width]; // array équivalent à data 
	int g; // gray level 
 	
	for (int i=0; i<height; i++)
	{	
		for (int j=0; j<width; j++)
		{ 	
			g = data[width * j + i];
			//array[width * j + i] = g; /* Calcul trés lourd! Le programme "break".
			
		}
	} 
	
	return *data;
}


void setLabel(cv::Mat& im, const std::string label, cv::Rect boundingRect)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = boundingRect;

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255,255,255), CV_FILLED);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(0,0,0), thickness, 8);
}

void displayShape(cv::Mat &frame, RecognizedShape &shape)
{
	cv::Point pt(shape.boundingRect.x, shape.boundingRect.y);
	cv::rectangle(frame,pt,pt+cv::Point(shape.boundingRect.width,shape.boundingRect.height),CV_RGB(255,0,255),2);
	
	setLabel(frame, shape.label ,shape.boundingRect);
}


