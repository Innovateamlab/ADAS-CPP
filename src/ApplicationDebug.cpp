#include "header.hpp"
#include "Flags.hpp"

using namespace std; 
using namespace cv;

string get_image_path(string file_path);

int applicationDebug ( int argc, char **argv ) 
{
	int INTERVAL_SHAPE = 3, INTERVAL_GLOBAL = 5;

	int countRed = 0, countBlue = 0;
	int countGlobal = 0;
	//bool usePipe = false;
	bool doSave = true;

	Data data;
	if(argc == 4)
	{
		countRed = atoi(argv[1]);
		countBlue = atoi(argv[2]);
		countGlobal = atoi(argv[3]);
	}
	
	if(argv[1] == "noSave")
	{
		doSave = false;
	}
	
	
	//int pipeDescriptor = setupNamedPipe(O_WRONLY);
	//runningThread(pipeDescriptor);
	
	int nCount=0;
	time_t timer_begin,timer_end;
	time_t timer_start_interval = 0, timer_end_interval = 0;
	time_t begin = 0, fin = 0;
	
	cv::Mat image;
	time ( &timer_begin ); // Lance le chrono
	
	string file_path= argv[1];
	ifstream fichier(file_path.c_str(), ios::in);
	
	string path = get_image_path(file_path);
	while(1)
	{	
		string image_name;
		if (getline(fichier, image_name))
		{
				stringstream image_path;
				image_path<<path<<image_name;
				image = imread(image_path.str()); 
				cout<<"Image_path : "<<image_path.str()<<endl;
				cout<<"read image : "<<(countGlobal)<<endl; 
				cv::waitKey(10);
				countGlobal ++;
		}
		else
		{
			fichier.close();
			return 0;
		}
		// Image preprocessing : get the preprocessed image (in hsv)
		cv::Mat hsv = preprocessing(image);
	
		// Défine range of red and blue color in HSV
		cv::Mat blueMask = SetBlueMask(hsv);
		cv::Mat redMask = SetRedMask(hsv);
		

#if	DEBUG>=2
		cv::imshow("redmask", redMask);
		cv::imshow("bluemask", blueMask);
		cv::waitKey(50);
#endif
		// find contours in the thresholded image and initialize the shape detector
		std::vector<std::vector<cv::Point> > contoursB;
		findContours(blueMask.clone(), contoursB, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		
		std::vector<std::vector<cv::Point> > contoursR;
		findContours(redMask.clone(), contoursR, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		
		// find shape
		std::vector<RecognizedShape> shapeB = shapeDetectorBlue(image, contoursB);
		std::vector<RecognizedShape> shapeR = shapeDetectorRed(image, contoursR);
		
		displayRecognizedShapes(image, shapeB);
		displayRecognizedShapes(image, shapeR);

		//save image
		if(doSave && canSave(timer_start_interval, timer_end_interval, INTERVAL_SHAPE))
		{	
			bool saveR = false;
			bool saveB = false;
			
			//Try to save
			if(shapeR.size() != 0)
				saveR = save_image(image, shapeR[0],"RED", countRed);
			if(shapeB.size() != 0)
				saveB = save_image(image, shapeB[0],"BLUE", countBlue);
			
			if(saveR)
			{
				/*
				data.flag = LIGHT_RED;
				sprintf (data.message, "Save Red");
				write(pipeDescriptor,&data, sizeof(Data));*/
	
			}
			if(saveB)
			{
				/*data.flag = LIGHT_BLUE;
				sprintf (data.message, "Save Blue");
				write(pipeDescriptor,&data, sizeof(Data));*/

			}
			
			if(saveR || saveB)
			{
				time ( &timer_start_interval );
			}
		}
		
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

