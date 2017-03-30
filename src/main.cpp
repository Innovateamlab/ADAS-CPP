#include "header.hpp"
//#include "Flags.hpp"

using namespace std;
using namespace cv;

string filepathGlobal = "ImagesSave/Global/imgGlobal_";
string filepathRed = "ImagesSave/Red/imgCropRed_";
string filepathBlue = "ImagesSave/Blue/imgCropBlue_";
string fileFormat = ".jpg";

Parameters manageParameters(int argc, char **argv);
Parameters initParameters();

int main ( int argc, char **argv ) 
{
	Parameters parameters = manageParameters(argc, argv);
	
	if(parameters.mode == MODE_EMBARQUE)
	{
		applicationEmbarquee(parameters);
	}
	else if(parameters.mode == MODE_DEBUG){
		//applicationDebug(argc,argv);
	}
	else if(parameters.mode == MODE_DEVELOPPEMENT){
		parameters.noPipe = true;
		parameters.noSave = true;
		applicationDeveloppement(parameters);
	}
	else
	{
		cout << "No mode to start \n";  
	}
	
	
	return 0;
}


/****** Functions *******/

Parameters manageParameters(int argc, char **argv)
{
	Parameters parameters = initParameters();
	
	for(int i=1; i<argc; i++)
	{
		if(string(argv[i]) == "-list"){
			parameters.list = string(argv[i+1]);
			parameters.mode = MODE_DEBUG;
			i+=1;
		} else if(string(argv[i]) == "-counts") {
			for(int j=0;j<3;j++) parameters.counts[j] = atoi(argv[i+j+1]);
			i+=3;
		} else if(string(argv[i]) == "-debug") {
			parameters.debug = atoi(argv[i+1]);
			parameters.mode = MODE_DEVELOPPEMENT;
			i+=1;
		} else if(string(argv[i]) == "--noPipe") {
			parameters.noPipe = true;	
		} else if(string(argv[i]) == "--noSave") {
			parameters.noSave = true;
		} else {
			cout << "Not a switch" << endl;
		}
	}
	
	return parameters;
}

Parameters initParameters()
{
	Parameters param;
	param.mode = MODE_EMBARQUE;
	param.list = "";
	for(int i=0;i<3;i++) param.counts[i] = 0;
	param.debug = 0;
	param.noPipe = false;
	param.noSave = false;
	
	return param;
}



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

