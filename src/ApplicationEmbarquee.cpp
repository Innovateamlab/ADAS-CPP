#include "header.hpp"
#include "Flags.hpp"

using namespace std;
using namespace cv;

int INTERVAL_SHAPE = 3, INTERVAL_GLOBAL = 5;

int countRed = 0, countBlue = 0;
int countGlobal = 0;

bool usePipe = true;
bool doSave = true;
Data data;

int applicationEmbarquee( int argc, char **argv ) 
{
	if(argc ==  4)
	{
		countRed = atoi(argv[1]);
		countBlue = atoi(argv[2]);
		countGlobal = atoi(argv[3]);
	}
	
	if(argv[1] == "noSave")
	{
		doSave = false;
	}
	
	
	int pipeDescriptor = setupNamedPipe(O_WRONLY);
	runningThread(pipeDescriptor);
	
	int nCount=0;
	time_t timer_begin,timer_end;
	time_t timer_start_interval = 0, timer_end_interval = 0;
	time_t begin = 0, fin = 0;
	
	cv::Mat image;
	raspicam::RaspiCam_Cv camera;
	
	//set camera params
	camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 ); // CV_8UC3 = frame RGB; CV_8UC1 = frame gray;
	camera.set( CV_CAP_PROP_FRAME_WIDTH,  640);
	camera.set( CV_CAP_PROP_FRAME_HEIGHT, 480);
	//camera.set(CV_CAP_PROP_FPS, 90);
	

	if (!camera.open()) {cerr<<"Error opening the camera"<<endl; return -1;}
	cout<<"Camera opened..."<<endl;
	
	time ( &timer_begin ); // Lance le chrono
	while(1)
	{
		//Start capture
		camera.grab();
		camera.retrieve (image);
		
		//save global
		if (canSave(begin, fin, INTERVAL_GLOBAL))	
		{
			countGlobal ++;
			stringstream filename_global;
			filename_global<<filepathGlobal<<(countGlobal)<<fileFormat;
			imwrite(filename_global.str(),image);
			cout<<"Global image saved at "<<filename_global.str()<<endl;
			time(&begin);
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
		
#if	DEBUG>=1
		cv::imshow("frame", image);
		cv::waitKey(50);
#endif

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
			
			if(saveR && usePipe)
			{
				data.flag = LIGHT_RED;
				sprintf (data.message, "Save Red");
				write(pipeDescriptor,&data, sizeof(Data));
	
			}
			if(saveB&& usePipe)
			{
				data.flag = LIGHT_BLUE;
				sprintf (data.message, "Save Blue");
				write(pipeDescriptor,&data, sizeof(Data));

			}
			
			if(saveR || saveB)
			{
				time ( &timer_start_interval );
			}
		}
		
#if	DEBUG>=1
		//show time statistics
		getFPS(timer_begin, timer_end, nCount);
#endif
	}
	
	camera.release(); // Close the PiCam device
	cout<< "\n Release done" << endl;
	
	return 0;
}
