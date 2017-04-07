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
	
	cv::Mat image, image_gray;
	string listFile = parameters.list;
	string image_name, image_path, listFilePath;
	
	timespec timer_fps_start, timer_fps_end;
	
	int lineNumber = 0;
	std::vector<Annotation> annotations;
	Result globalResult = initResult(), result;
	
	vector<Rect> signs;
	vector<double> secondsElapsed;
	
	CascadeClassifier classifier;
	if(parameters.classifier != "")
	{
		classifier.load(parameters.classifier);
	}
	
	
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
		
		//Start timer
		clock_gettime ( CLOCK_MONOTONIC, &timer_fps_start );
		
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
		
		//classifiers
		if(parameters.classifier != "")
		{
			cvtColor(image, image_gray, CV_BGR2GRAY);
			equalizeHist(image_gray,image_gray);
			
			classifier.detectMultiScale(image_gray, signs, 1.12, 5);
			for(size_t i =0;i<signs.size();i++)
			{
				Point center(signs[i].x + signs[i].width*0.5, signs[i].y + signs[i].height*0.5 );
				float radius = (signs[i].width*0.4 + signs[i].height*0.4) / 2;
				circle(image, center, radius, Scalar(0,0,255),3 ,8, 0);
			}
		}
		
		//End timer
		clock_gettime ( CLOCK_MONOTONIC, &timer_fps_end );
		double elapsed = (timer_fps_end.tv_sec - timer_fps_start.tv_sec); 
		elapsed += (timer_fps_end.tv_nsec - timer_fps_start.tv_nsec) / 1000000000.0; 
		secondsElapsed.push_back( elapsed );
		
		if(parameters.annotation != "")
		{
			std::vector<cv::Rect> detectionRects = getRects(shapeR);
			std::vector<cv::Rect> blueRects = getRects(shapeB);
			detectionRects.insert(detectionRects.end(), blueRects.begin(), blueRects.end());
			detectionRects.insert(detectionRects.end(), signs.begin(), signs.end());
		
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
		
		if(parameters.show)//|| result.truePositives != 0 || result.falsePositives != 0)
		{
			imshow("Image", image);
			cv::waitKey(0);
		}
	}
	
	if(parameters.annotation != "")
	{
		std::cout << printResult(globalResult);
	}
	
	double mean = 0;
	for(int i=0;i<secondsElapsed.size();i++)
		mean += secondsElapsed[i];
	mean /= secondsElapsed.size();
	cout << "fps : " << 1.0/mean << endl;
	
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

