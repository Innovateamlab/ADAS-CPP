#include "header.hpp"
#include "Flags.hpp"

using namespace std; 
using namespace cv;

string get_image_path(string file_path);
int applicationDebug (Parameters parameters) 
{	
	cv::Mat image;
	string file_path = parameters.list;
	string path = get_image_path(file_path);
	string image_name;
	
	ifstream fichier(file_path.c_str(), ios::in);
	
	while(1)
	{	
		if (getline(fichier, image_name))
		{
			cout<<"Reading image : "<<image_name<<endl; 
			image = imread(image_name); 
		}
		else
		{
			fichier.close();
			break;
		}
		// Image preprocessing : get the preprocessed image (in hsv)
		cv::Mat hsv = preprocessing(image);
	
		// Défine range of red and blue color in HSV
		cv::Mat blueMask = SetBlueMask(hsv);
		cv::Mat redMask = SetRedMask(hsv);

		// find contours in the thresholded image and initialize the shape detector
		std::vector<std::vector<cv::Point> > contoursB;
		findContours(blueMask.clone(), contoursB, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		
		std::vector<std::vector<cv::Point> > contoursR;
		findContours(redMask.clone(), contoursR, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		
		// find shape
		std::vector<RecognizedShape> shapeB = shapeDetectorBlue(image, contoursB);
		std::vector<RecognizedShape> shapeR = shapeDetectorRed(image, contoursR);
		
		bool saveR = false;
		bool saveB = false;
		
		string filenameR, filenameB;
		
		//save image
		if(!parameters.noSave)
		{
			string filenameR, filenameB;	
			if(shapeR.size() != 0)
				saveR = save_image(image, shapeR[0],"RED", parameters.counts[0],filenameR);
			if(shapeB.size() != 0)
				saveB = save_image(image, shapeB[0],"BLUE", parameters.counts[2],filenameB);
		}
		
		displayRecognizedShapes(image, shapeB);
		displayRecognizedShapes(image, shapeR);
		
		if(parameters.show)
		{
			imshow("Image", image);
			cv::waitKey(0);
		}
		
		if(saveR)
			imwrite(filenameR + "_marked" + fileFormat,image);
		if(saveB)
			imwrite(filenameB + "_marked" + fileFormat,image);
	}

	cout<<"\n Read done"<< endl;
	
	return 0;
}

string get_image_path(string file_path)
{
	size_t pos = 0;
	string delimiter ="/";
	string result = "";
	
	
	while((pos = file_path.find(delimiter)) != std::string::npos)
	{
		result += file_path.substr(0,pos + delimiter.length());
		file_path = file_path.substr(pos + delimiter.length(), file_path.length());
	}
	cout<<"file_path : "<<result<<endl;
	return result;
}

