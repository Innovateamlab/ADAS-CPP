#include "header.hpp"
//#include "Flags.hpp"

using namespace std;
using namespace cv;

string filepathGlobal = "ImagesSave/Global/imgGlobal_";
string filepathRed = "ImagesSave/Red/imgCropRed_";
string filepathBlue = "ImagesSave/Blue/imgCropBlue_";
string fileFormat = ".jpg";

int main ( int argc, char **argv ) 
{
	
	applicationEmbarquee(argc, argv);
	
	return 0;
}


/****** Functions *******/

bool save_image(cv::Mat frame, RecognizedShape shape, string color, int &count)
{
	stringstream filename;
	
	if(color == "BLUE")
	{
		if(shape.matrix.size().width >10 && shape.matrix.size().height>10)
		{
			count ++;
			
			if(shape.label == "BLUE_RECT")
			{
				filename<<filepathBlue<<(count)<<"_RECT"<<fileFormat;
				imwrite(filename.str(),frame);
				cout<<"Blue image saved at "<<filename.str()<<endl;
				return true;
			}
			else if(shape.label == "BLUE_CIRC")
			{
				filename<<filepathBlue<<(count)<<"_CIRC"<<fileFormat;
				imwrite(filename.str(),frame);
				cout<<"Blue image saved at "<<filename.str()<<endl;
				return true;
			}
			else
			{
				count --;	
				return false;
			}
		}
	}
	else
	{
		if(shape.matrix.size().width >10 && shape.matrix.size().height>10)
		{
			count ++;
			
			if(shape.label == "RED_TRI")
			{
				filename<<filepathRed<<(count)<<"_TRI"<<fileFormat;
				imwrite(filename.str(),frame);
				cout<<"Red image saved at "<<filename.str()<<endl;
				return true;
			}
			else if(shape.label == "RED_CIRC")
			{
				filename<<filepathRed<<(count)<<"_CIRC"<<fileFormat;
				imwrite(filename.str(),frame);
				cout<<"Red image saved at "<<filename.str()<<endl;
				return true;
			}
			else
			{
				count --;
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



void displayRecognizedShapes(cv::Mat &frame, std::vector<RecognizedShape> &shapes)
{
	for(int i=0; i<shapes.size(); i++)
	{
		displayShape(frame, shapes[i]);
	}		
}

