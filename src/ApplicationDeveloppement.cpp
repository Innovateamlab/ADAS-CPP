#include "header.hpp"
#include "Flags.hpp"

using namespace std;
using namespace cv;

int applicationDeveloppement(Parameters parameters) 
{
	cout << "Developpement started" << endl;
	
	int INTERVAL_SHAPE = 3, INTERVAL_GLOBAL = 1;
	int pipeDescriptor = -1;
	Data data;
	
	if(!parameters.noPipe)
	{
		pipeDescriptor = setupNamedPipe(O_WRONLY);
		runningThread(pipeDescriptor);
	}
	
	
	int fpsCount = 0;
	time_t timer_global = 0;
	time_t timer_shape = 0;
	time_t timer_fps = 0;
	time_t timer_end = 0;
	
	cv::Mat image, image_gray;
	raspicam::RaspiCam_Cv camera;
	vector<Rect> signs, signs_2;
	
	CascadeClassifier classifier, classifier_2;
	if(parameters.classifier != "")
	{
		classifier.load(parameters.classifier);
		classifier_2.load("../cascade_limitation_vitesse.xml");
	}
		
	std::vector<RecognizedShape> shapeB, shapeR;
	
	//set camera params
	camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 ); // CV_8UC3 = frame RGB; CV_8UC1 = frame gray;
	camera.set( CV_CAP_PROP_FRAME_WIDTH,  640);
	camera.set( CV_CAP_PROP_FRAME_HEIGHT, 480);
	//camera.set(CV_CAP_PROP_FPS, 90);
	
	if (!camera.open()) {cerr<<"Error opening the camera"<<endl; return -1;}
	
	while(1)
	{
		//Start capture
		camera.grab();
		camera.retrieve (image);
		
		//save global
		if(canSave(timer_global, timer_end, INTERVAL_GLOBAL))	
		{
			parameters.counts[1]++;
			stringstream filename_global;
			filename_global<<filepathGlobal<<(parameters.counts[1])<<fileFormat;
			imwrite(filename_global.str(),image);
			time(&timer_global);
		}

		if(parameters.classifier == "")
		{
			// Image preprocessing : get the preprocessed image (in hsv)
			cv::Mat hsv = preprocessing(image);
		

			// Défine range of red and blue color in HSV
			cv::Mat blueMask = SetBlueMask(hsv);
			cv::Mat redMask = SetRedMask(hsv);
			
			if(parameters.debug >= 2)
			{
				imshow("blueMask", blueMask);
				imshow("redMask", redMask);
			}
			
			// find contours in the thresholded image and initialize the shape detector
			std::vector<std::vector<cv::Point> > contoursB;
			findContours(blueMask.clone(), contoursB, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			
			std::vector<std::vector<cv::Point> > contoursR;
			findContours(redMask.clone(), contoursR, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			
			if(parameters.debug >= 3)
			{
				cv::Mat blueContours = Mat::zeros(image.size(), CV_8UC3);
				std::vector<Vec4i> hierarchy;
				
				for(int i=0;i<contoursB.size();i++)
				{
					drawContours(blueContours, contoursB, i, CV_RGB(0,255,0), 2, 8, hierarchy, 0 , Point() );
				}
				
				imshow("blueContours", blueContours);
			}
			
			
			// find shape
			shapeB = shapeDetectorBlue(image, contoursB);
			shapeR = shapeDetectorRed(image, contoursR);
		}
		else
		{
			/*cv::Mat hsv = preprocessing(image);
			cv::Mat blueMask = SetBlueMask(hsv);
			cv::Mat redMask = SetRedMask(hsv);
			cv::Mat mask = blueMask + redMask;*/
			
			cvtColor(image, image_gray, CV_BGR2GRAY);
			equalizeHist(image_gray,image_gray);
			
			classifier.detectMultiScale(image_gray, signs, 1.12, 5);
			for(size_t i =0;i<signs.size();i++)
			{
				Point center(signs[i].x + signs[i].width*0.5, signs[i].y + signs[i].height*0.5 );
				float radius = (signs[i].width*0.4 + signs[i].height*0.4) / 2;
				circle(image, center, radius, Scalar(0,0,255),3 ,8, 0);
			}
			
			classifier_2.detectMultiScale(image_gray, signs_2, 1.12, 5);
			for(size_t i =0;i<signs.size();i++)
			{
				Point center(signs_2[i].x + signs_2[i].width*0.5, signs_2[i].y + signs_2[i].height*0.5 );
				float radius = (signs_2[i].width*0.4 + signs_2[i].height*0.4) / 2;
				circle(image, center, radius, Scalar(0,0,255),3 ,8, 0);
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
				data.flag = LIGHT_RED;
				sprintf (data.message, "Save Red");
				write(pipeDescriptor,&data, sizeof(Data));
			}
			if(saveB && !parameters.noPipe)
			{
				data.flag = LIGHT_BLUE;
				sprintf (data.message, "Save Blue");
				write(pipeDescriptor,&data, sizeof(Data));
			}
			
			if(saveR || saveB)
			{
				time ( &timer_shape );
			}
		}
		
		if(parameters.classifier == "")
		{
			displayRecognizedShapes(image, shapeB);
			displayRecognizedShapes(image, shapeR);
		}
		
		if(saveR)
			imwrite(filenameR + "_marked" + fileFormat,image);
		if(saveB)
			imwrite(filenameB + "_marked" + fileFormat,image);
		
		getFPS(timer_fps, timer_end, fpsCount);
		
		if(parameters.debug >= 1)
		{
			imshow("Image", image);
		}
		waitKey(20);
	}
	
	camera.release(); // Close the PiCam device
	cout<< "\n Release done" << endl;
	
	return 0;
}
