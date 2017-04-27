#include "header.hpp"
#include "Flags.hpp"

using namespace std;
using namespace cv;

int applicationEmbarquee(Parameters parameters) 
{
	int INTERVAL_SHAPE = 3, INTERVAL_GLOBAL = 5;
	int pipeDescriptor = -1;
	Data data;
	
	if(!parameters.noPipe)
	{
		pipeDescriptor = setupNamedPipe(O_WRONLY);
		runningThread(pipeDescriptor);
	}
	
	time_t timer_global = 0;
	time_t timer_shape = 0;
	time_t timer_end = 0;
	
	cv::Mat image, image_gray;
	raspicam::RaspiCam_Cv camera;
	
	//set camera params
	camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 ); // CV_8UC3 = frame RGB; CV_8UC1 = frame gray;
	camera.set( CV_CAP_PROP_FRAME_WIDTH,  640);
	camera.set( CV_CAP_PROP_FRAME_HEIGHT, 480);
	
	if (!camera.open()) {cerr<<"Error opening the camera"<<endl; return -1;}
	
	vector<Rect> signs;
	CascadeClassifier classifier;
	if(parameters.classifier != "")
	{
		classifier.load(parameters.classifier);
	}
	
	while(1)
	{
		//Start capture
		camera.grab();
		camera.retrieve (image);
		
		//save global
		if(!parameters.noSave && canSave(timer_global, timer_end, INTERVAL_GLOBAL))	
		{
			parameters.counts[1]++;
			stringstream filename_global;
			filename_global<<filepathGlobal<<(parameters.counts[1])<<fileFormat;
			imwrite(filename_global.str(),image);
			time(&timer_global);
		}

		// Image preprocessing : get the preprocessed image (in hsv)
		cv::Mat hsv = preprocessing(image);
	
		// Défine range of red and blue color in HSV
		cv::Mat blueMask = SetBlueMask(hsv);
		cv::Mat redMask = SetRedMask(hsv);
		
		dilate(redMask,redMask, Mat(), Point(2,2));
		dilate(blueMask,blueMask, Mat(), Point(2,2));
		
		// find contours in the thresholded image and initialize the shape detector
		std::vector<std::vector<cv::Point> > contoursB;
		findContours(blueMask.clone(), contoursB, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		
		std::vector<std::vector<cv::Point> > contoursR;
		findContours(redMask.clone(), contoursR, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		
		// find shape
		std::vector<RecognizedShape> shapeB = shapeDetectorBlue(image, contoursB);
		std::vector<RecognizedShape> shapeR = shapeDetectorRed(image, contoursR);
		
		//find shape with classifier
		if(parameters.classifier != "")
		{
			cvtColor(image, image_gray, CV_BGR2GRAY);
			equalizeHist(image_gray,image_gray);
			
			classifier.detectMultiScale(image_gray, signs, 1.12, 5);
			for(size_t i =0;i<signs.size();i++)
			{
				RecognizedShape recognizedShape;
				
				recognizedShape.matrix = image(signs[i]);
				recognizedShape.label = "RED_CIRC";
				recognizedShape.boundingRect = signs[i];
				
				shapeR.push_back(recognizedShape);
			}
		}
		
		bool saveR = false;
		bool saveB = false;
		
		string filenameR, filenameB;
		
		//save image
		if(!parameters.noSave && canSave(timer_shape, timer_end, INTERVAL_SHAPE))
		{		
			//Try to save
			if(shapeR.size() != 0)
				saveR = save_image(image, shapeR[0],"RED", parameters.counts[0], filenameR);
			if(shapeB.size() != 0)
				saveB = save_image(image, shapeB[0],"BLUE", parameters.counts[2], filenameB);
			
			if(saveR && !parameters.noPipe)
			{
				if(shapeR[0].label == "RED_CIRC")
				{
					data.flag = RED_CIRCLE;
				}
				else if(shapeR[0].label == "RED_TRI")
				{
					data.flag = RED_TRIANGLE;
				}
				sprintf (data.message, "Save Red");
				write(pipeDescriptor,&data, sizeof(Data));
				
			}
			if(saveB && !parameters.noPipe)
			{
				if(shapeR[0].label == "BLUE_CIRC")
				{
					data.flag = BLUE_CIRCLE;
				}
				else if(shapeR[0].label == "BLUE_RECT")
				{
					data.flag = BLUE_SQUARE;
				}
				sprintf (data.message, "Save Blue");
				write(pipeDescriptor,&data, sizeof(Data));
			}
			
			if(saveR || saveB)
			{
				time ( &timer_shape );
			}
		}
		
		displayRecognizedShapes(image, shapeB);
		displayRecognizedShapes(image, shapeR);
		
		if(saveR)
			imwrite(filenameR + "_marked" + fileFormat,image);
		if(saveB)
			imwrite(filenameB + "_marked" + fileFormat,image);
	}
	
	camera.release(); // Close the PiCam device
	cout<< "\n Release done" << endl;
	
	return 0;
}
