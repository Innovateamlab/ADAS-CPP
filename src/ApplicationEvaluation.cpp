#include "header.hpp"
#include "Flags.hpp"
#include "Evaluation.hpp"

using namespace std; 
using namespace cv;

string getPath(string filepath);
string getName(string filepath);
std::vector<Rect> getRects(std::vector<RecognizedShape> shapes);

int applicationEvaluation (Parameters parameters) 
{	
	if(parameters.list == "")
	{
		cout << "'-list fileList' needed in Evaluation mode." << endl;
		return 1;
	}
	
	cout << "Evaluation started" << endl;
	
	cv::Mat image;
	string listFile = parameters.list;
	string image_name, image_path, listFilePath;
	
	
	int lineNumber = 0;
	std::vector<Annotation> annotations;
	Result globalResult = initResult(), result;
	
	ifstream fichier(listFile.c_str(), ios::in);
	
	if(parameters.annotation != "")
	{
		annotations = loadAnnotations(parameters.annotation);
	}
	
	//Get listFilePath from listFile
	listFilePath = getPath(listFile);
	
	while(1)
	{	
		if (getline(fichier, image_name))
		{
			std::stringstream stream;
			stream << listFilePath;
			stream << image_name;
			cout<<"Reading image : "<<stream.str()<<endl; 
			image = imread(stream.str());
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
		
		if(parameters.annotation != "")
		{
			std::vector<cv::Rect> detectionRects = getRects(shapeR);
			std::vector<cv::Rect> blueRects = getRects(shapeB);
			detectionRects.insert(detectionRects.end(), blueRects.begin(), blueRects.end());
			
			std::vector<cv::Rect> trueRects;
			
			image_name = getName(image_name);
			
			if(annotations[lineNumber].filename.find(image_name) != std::string::npos)
			{
				trueRects = annotations[lineNumber].boundingRects;
				
				for(int i=0;i<annotations[lineNumber].boundingRects.size();i++)
				{
					cv::Rect rect = annotations[lineNumber].boundingRects[i];
					
					cv::Point pt(rect.x, rect.y);
					cv::rectangle(image,pt,pt+cv::Point(rect.width,rect.height),CV_RGB(255,0,0),2);
				}
				lineNumber++;
			}
			
			
			result = getDectectionResult(trueRects,detectionRects); 
			globalResult = globalResult + result;
		}
		                
		displayRecognizedShapes(image, shapeB);
		displayRecognizedShapes(image, shapeR);
		
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
		
		if(parameters.show || result.truePositives != 0)
		{
			imshow("Image", image);
			cv::waitKey(0);
		}
	}
	
	if(parameters.annotation != "")
	{
		std::cout << printResult(globalResult);
	}
	
	cout<<"\n Read done"<< endl;
	
	return 0;
}

string getPath(string filepath)
{
	size_t last_slash_index = filepath.find_last_of("/");
	if(std::string::npos != last_slash_index)
	{
		filepath.erase(last_slash_index + 1, std::string::npos);
	}
	else
	{
		filepath = "";
	}
	
	return filepath;
}

string getName(string filepath)
{
	size_t last_slash_index = filepath.find_last_of("/");
	if(std::string::npos != last_slash_index)
	{
		filepath.erase(0, last_slash_index+1);
	}
	
	return filepath;
}

std::vector<cv::Rect> getRects(std::vector<RecognizedShape> shapes)
{
	std::vector<cv::Rect> rects;
	
	for(int i=0;i<shapes.size();i++)
	{
		rects.push_back(shapes[i].boundingRect);
	}
	
	return rects;
}

