#include <ctime>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include </usr/local/include/raspicam/raspicam_cv.h>
#include </usr/local/include/raspicam/raspicam.h>
#include </usr/local/include/raspicam/raspicam_still_cv.h>
#include </usr/local/include/raspicam/raspicamtypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include </usr/local/include/opencv2/opencv.hpp>
#include <wiringPi.h>

#include "header.hpp"

#define TAILLE_BUFFER 2*sizeof(int)

using namespace std;
using namespace cv;

string filepathRed = "ImagesSave/Red/imgCropRed_", filepathBlue = "ImagesSave/Blue/imgCropBlue_";
string filepathGlobal = "ImagesSave/Global/imgGlobal_";
string fileFormat = ".jpg";
int CAPTURE_INTERVAL_S = 3;

int countRed = 0, countBlue = 0;
int countGlobal = 0;

bool save_image(RecognizedShape shape, string color);
bool canSave(time_t &start, time_t &end);

int main ( int argc,char **argv ) 
{

	int descripteur[2];
	// descripteur[0] = sortie du tube
	// descripteur[1] = entrée du tube
	
	int buffer[TAILLE_BUFFER];
	//buffer[4] = FAIL;
	if(pipe(descripteur) !=0)
	{
		cout<<"Error creating tube"<<endl;
		return EXIT_FAILURE;
		buffer[4] = FAIL;
	}
	
	pid_t pid;
	pid = fork();
	
	if (pid == -1)
	{
		cout<<"Error creating processus"<<endl;
		return EXIT_FAILURE;
		buffer[4] = FAIL;
	}
	
	if (pid != 0 ){
		close(descripteur[0]);
	int nCount=100;
    time_t timer_begin,timer_end;
    time_t timer_start_interval = 0, timer_end_interval = 0;
    
    cv::Mat image;
    
	raspicam::RaspiCam_Cv camera;
	
	//set camera params
    camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 ); // CV_8UC3 = frame RGB; CV_8UC1 = frame gray;
    camera.set( CV_CAP_PROP_FRAME_WIDTH, 640 );
    camera.set( CV_CAP_PROP_FRAME_HEIGHT, 480 );
    //camera.set(CV_CAP_PROP_FPS, 90);
    

    if (!camera.open()) {cerr<<"Error opening the camera"<<endl; return -1;}
    cout<<"Camera opened..."<<endl;
    
    while(1)
    {
		
#if	DEBUG>0
		
		time ( &timer_begin ); // Lance le chrono
		cout<<"Capturing "<<nCount<<" frames ...."<<endl;
		
		for ( int i=0; i<nCount; i++ ) 
		{	
#endif
			//Start capture
			camera.grab();
			camera.retrieve (image);
			
#if	DEBUG>0
		
			if ( i%5==0 )  cout<<"\r captured "<<i<<" images"<<std::flush;
#endif
        
			// Image preprocessing : get the preprocessed image (in hsv)
			cv::Mat hsv = preprocessing(image);
		
			// Défine range of red and blue color in HSV
			cv::Mat blueMask = SetBlueMask(hsv);
			cv::Mat redMask = SetRedMask(hsv);
			

#if	DEBUG>1
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
			RecognizedShape shapeB = shapeDetectorBlue(image, contoursB);
			RecognizedShape shapeR = shapeDetectorRed(image, contoursR);
			
			
#if	DEBUG>1
			/*if(shapeB.matrix.size().width >10 && shapeB.matrix.size().height>10)
			{
				cv::imshow("blueCrop", shapeB.matrix);
				cv::waitKey(50);
			}*/
#endif	
			
			// blink led if shape founded
#if	DEBUG>0	
			cv::imshow("frame", image);
			cv::waitKey(50);
			//digitalWrite (0, HIGH) ;
			//digitalWrite (0,  LOW) ; 
#endif
			// save global image every 3 seconds
			
				stringstream filename_global;
				filename_global<<filepathGlobal<<(countGlobal)<<fileFormat;
				imwrite(filename_global.str(),image);
				cout<<"Global image saved at "<<filename_global.str()<<endl;
				countGlobal ++;
				sleep(10);
			//save image
			
			if(canSave(timer_start_interval, timer_end_interval))
			{	
				//Try to save
				bool saveR = save_image(shapeR,"RED");
				bool saveB = save_image(shapeB,"BLUE");
				
				if(saveR)
				{
						buffer[0] = LIGHT_RED;
						write(descripteur[1],buffer, sizeof(int));
				}
				if(saveB)
				{
						buffer[0] = LIGHT_BLUE;
						write(descripteur[1],buffer, sizeof(int));
				}
				
				if(saveR || saveB)
				{
					time ( &timer_start_interval );
				}
				
				saveB = false;
				saveR = false;
				
			}
#if	DEBUG>0
		}
		
		//show time statistics
		time ( &timer_end ); // Arrete le chrono
		double secondsElapsed = difftime ( timer_end,timer_begin );
		cout<< "\n" << secondsElapsed  <<" seconds for "<< nCount<<"  frames : FPS = "<<  ( float ) (nCount/secondsElapsed) << endl;

		// cout<< "\n Appuyez sur 'Entrée' pour terminer" <<endl;
		//getchar();
#endif
	}
	
	camera.release(); // Close the PiCam device
	cout<< "\n Release done" << endl;
	
		return 0;
	}
	else{
		while(1){
			close(descripteur[1]);
			read(descripteur[0], buffer, TAILLE_BUFFER);
			test_Flags (buffer);
		}
	}
}

bool save_image(RecognizedShape shape, string color)
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
				imwrite(filename.str(),shape.matrix);
				cout<<"Blue image saved at "<<filename.str()<<endl;
				return true;
			}
			else if(shape.label == "BLUE_CIRC")
			{
				filename<<filepathBlue<<(countBlue)<<"_CIRC"<<fileFormat;
				imwrite(filename.str(),shape.matrix);
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
				imwrite(filename.str(),shape.matrix);
				cout<<"Red image saved at "<<filename.str()<<endl;
				return true;
			}
			else if(shape.label == "RED_CIRC")
			{
				filename<<filepathRed<<(countRed)<<"_CIRC"<<fileFormat;
				imwrite(filename.str(),shape.matrix);
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

bool canSave(time_t &start, time_t &end)
{
	if(start == 0)
		return true;
	time ( &end );
	double secondsElapsed = difftime (end, start);
	return secondsElapsed>=3;
	
}
