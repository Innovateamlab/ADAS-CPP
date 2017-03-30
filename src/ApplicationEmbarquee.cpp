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
	
	cv::Mat image;
	raspicam::RaspiCam_Cv camera;
	
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
		if(!parameters.noSave && canSave(timer_shape, timer_end, INTERVAL_SHAPE))
		{	
			bool saveR = false;
			bool saveB = false;
			
			//Try to save
			if(shapeR.size() != 0)
				saveR = save_image(image, shapeR[0],"RED", parameters.counts[0]);
			if(shapeB.size() != 0)
				saveB = save_image(image, shapeB[0],"BLUE", parameters.counts[2]);
			
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
	}
	
	camera.release(); // Close the PiCam device
	cout<< "\n Release done" << endl;
	
	return 0;
}
