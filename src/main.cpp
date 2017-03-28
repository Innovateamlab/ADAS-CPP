#include "header.hpp"
#include "Flags.hpp"

#define TAILLE_BUFFER 2*sizeof(int)

using namespace std;
using namespace cv;

string filepathRed = "ImagesSave/Red/imgCropRed_", filepathBlue = "ImagesSave/Blue/imgCropBlue_";
string filepathGlobal = "ImagesSave/Global/imgGlobal_";
string fileFormat = ".jpg";
int INTERVAL_SHAPE = 3, INTERVAL_GLOBAL = 3;

int countRed = 0, countBlue = 0;
int countGlobal = 0;
bool usePipe = true;
bool doSave = true;

bool save_image(cv::Mat frame, RecognizedShape shape, string color);
bool canSave(time_t &start, time_t &end, int interval);
float getFPS(time_t &timer_begin, time_t &timer_end, int &nCount);
void gpioSetup();
void displayRecognizedShapes(cv::Mat &frame, std::vector<RecognizedShape> &shapes);

int main ( int argc, char **argv ) 
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
	
	gpioSetup();	
	int pipeDescriptor = setupNamedPipe(O_WRONLY);
	
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
		
		// save global image every 10 seconds	
		if (canSave(begin, fin, INTERVAL_GLOBAL))	
		{
			countGlobal ++;
			stringstream filename_global;
			filename_global<<filepathGlobal<<(countGlobal)<<fileFormat;
			imwrite(filename_global.str(),image);
			cout<<"Global image saved at "<<filename_global.str()<<endl;
			time(&begin);
			if(usePipe)
			{
				int err = write(pipeDescriptor,"5", sizeof("5"));
				if(err == -1)
				{
					char buffer[256];
					char * message = strerror_r(errno, buffer, 256);
					cout << "(Write ./ShapeColorDectector.a) " << errno << " : " << message << endl;
					usePipe = false;
				}
			}
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
		
#if	DEBUG>=1
		displayRecognizedShapes(image, shapeB);
		displayRecognizedShapes(image, shapeR);
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
				saveR = save_image(image, shapeR[0],"RED");
			if(shapeB.size() != 0)
				saveB = save_image(image, shapeB[0],"BLUE");
			
			if(saveR && usePipe)
			{
				write(pipeDescriptor,"0", sizeof("0"));
				/*
				digitalWrite (LIGHT_RED, HIGH) ;
				delay(500);
				digitalWrite (LIGHT_RED,  LOW) ; 
				delay(500);		*/
			}
			if(saveB&& usePipe)
			{
				write(pipeDescriptor,"1", sizeof("1"));
				/*digitalWrite (LIGHT_BLUE, HIGH) ;
				delay(500);
				digitalWrite (LIGHT_BLUE,  LOW) ; 
				delay(500);*/
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


/****** Functions *******/

bool save_image(cv::Mat frame, RecognizedShape shape, string color)
{
	stringstream filename;
	
	if(color == "BLUE")
	{
		if(shape.matrix.size().width >10 && shape.matrix.size().height>10)
		{
			countBlue ++;
			
			if(shape.label == "BLUE_RECT")
			{
				filename<<filepathBlue<<(countBlue)<<"_RECT"<<fileFormat;
				imwrite(filename.str(),frame);
				cout<<"Blue image saved at "<<filename.str()<<endl;
				return true;
			}
			else if(shape.label == "BLUE_CIRC")
			{
				filename<<filepathBlue<<(countBlue)<<"_CIRC"<<fileFormat;
				imwrite(filename.str(),frame);
				cout<<"Blue image saved at "<<filename.str()<<endl;
				return true;
			}
			else
			{
				countBlue --;	
				return false;
			}
		}
	}
	else
	{
		if(shape.matrix.size().width >10 && shape.matrix.size().height>10)
		{
			countRed ++;
			
			if(shape.label == "RED_TRI")
			{
				filename<<filepathRed<<(countRed)<<"_TRI"<<fileFormat;
				imwrite(filename.str(),frame);
				cout<<"Red image saved at "<<filename.str()<<endl;
				return true;
			}
			else if(shape.label == "RED_CIRC")
			{
				filename<<filepathRed<<(countRed)<<"_CIRC"<<fileFormat;
				imwrite(filename.str(),frame);
				cout<<"Red image saved at "<<filename.str()<<endl;
				return true;
			}
			else
			{
				countRed --;
				return false;	
			}
		}
	}
	
	return false;
}

bool canSave(time_t &start, time_t &end, int interval)
{
	if(start == 0)
		return true;
	time ( &end );
	double secondsElapsed = difftime (end, start);
	return secondsElapsed>=interval;
	
}

float getFPS(time_t &timer_begin, time_t &timer_end, int &nCount)
{
	if(nCount < 100)
	{
		nCount++;
		return -1;
	}
	
	time ( &timer_end ); // Arrete le chrono
	double secondsElapsed = difftime ( timer_end,timer_begin );
	float fps = ( float ) (nCount/secondsElapsed);
	nCount = 0;
	cout<< "\nFPS = "<<  fps << endl;
	time ( &timer_begin );
	return fps;
}

void gpioSetup() //GPIO pins initialisation
{
	wiringPiSetup();	
	pinMode(LIGHT_BLUE, OUTPUT);
	pinMode(LIGHT_RED, OUTPUT);
	pinMode(FAIL, OUTPUT);
}



void displayRecognizedShapes(cv::Mat &frame, std::vector<RecognizedShape> &shapes)
{
	for(int i=0; i<shapes.size(); i++)
	{
		displayShape(frame, shapes[i]);
	}		
}

