#include "header.hpp"
//#include "Flags.hpp"
#include <omp.h>

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
	else if(parameters.mode == MODE_EVALUATION)
	{
		parameters.noPipe = true;
		parameters.noSave = true;
		applicationEvaluation(parameters);
	}
	else if(parameters.mode == MODE_DEVELOPPEMENT){
		parameters.noPipe = true;
		applicationDeveloppement(parameters);
	}
	else
	{
		cout << "No mode to start \n";
		cout << "use -mode <EMB | DBG | DEV> \n";
	}
	
	return 0;
}


/****** Functions *******/

Parameters manageParameters(int argc, char **argv)
{
	Parameters parameters = initParameters();
	
	for(int i=1; i<argc; i++)
	{
		string command = string(argv[i]);
		
		if(command == "-list"){
			parameters.list = string(argv[i+1]);
			parameters.mode = MODE_EVALUATION;
			i+=1;
		} else if(command == "-counts") {
			for(int j=0;j<3;j++) parameters.counts[j] = atoi(argv[i+j+1]);
			i+=3;
		} else if(command == "-debug") {
			parameters.debug = atoi(argv[i+1]);
			i+=1;
		} else if(command == "-classifier") {
			parameters.classifier = string(argv[i+1]);
			i+=1;
		} else if(command == "-annotation") {
			parameters.annotation = string(argv[i+1]);
			i+=1;
		} else if(command == "-mode") {
			string subCommand = string(argv[i+1]);
			
			if(subCommand == "EMB" || subCommand == "EMBARQUE")
			{
				parameters.mode = MODE_EMBARQUE;
			}
			else if(subCommand == "EVA" || subCommand == "EVALUATION")
			{
				parameters.mode = MODE_EVALUATION;
			}
			else if(subCommand == "DEV" || subCommand == "DEVELOPPEMENT")
			{
				parameters.mode = MODE_DEVELOPPEMENT;
			}
			i+=1;
		} else if(command == "-custom1") {
			parameters.custom1 = atof(argv[i+1]);
			i+=1;
		} else if(command == "-custom2") {
			parameters.custom2 = atof(argv[i+1]);
			i+=1;
		} else if(command == "--noPipe" || command == "--nopipe") {
			parameters.noPipe = true;	
		} else if(command == "--noSave" || command == "--nosave") {
			parameters.noSave = true;
		} else if(command == "--show") {
			parameters.show = true;
		} else {
			cout << command << " is not a switch" << endl;
		}
	}
	
	return parameters;
}

Parameters initParameters()
{
	Parameters parameters;
	parameters.mode = -1;
	parameters.list = "";
	parameters.classifier = "";
	for(int i=0;i<3;i++) parameters.counts[i] = 0;
	parameters.debug = 0;
	parameters.noPipe = false;
	parameters.noSave = false;
	parameters.show = false;
	parameters.custom1 = 0;
	parameters.custom2 = 0;
	
	return parameters;
}



bool save_image(cv::Mat frame, RecognizedShape shape, string color, int &count, string &filename_ptr, bool verbose)
{
	stringstream filename;
	
	if(color == "BLUE")
	{
		if(shape.matrix.size().width >10 && shape.matrix.size().height>10)
		{
			count ++;
			
			if(shape.label == "BLUE_RECT")
			{
				filename<<filepathBlue<<(count)<<"_RECT";
				filename_ptr = filename.str();				//filename without format extension
				filename<<fileFormat;
				imwrite(filename.str(),frame);
				if(verbose)
					cout<<"Blue image saved at "<<filename.str()<<endl;
				return true;
			}
			else if(shape.label == "BLUE_CIRC")
			{
				filename<<filepathBlue<<(count)<<"_CIRC";
				filename_ptr = filename.str();
				filename<<fileFormat;
				imwrite(filename.str(),frame);
				if(verbose)
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
				filename<<filepathRed<<(count)<<"_TRI";
				filename_ptr = filename.str();				
				filename<<fileFormat;
				imwrite(filename.str(),frame);
				if(verbose)
					cout<<"Red image saved at "<<filename.str()<<endl;
				return true;
			}
			else if(shape.label == "RED_CIRC")
			{
				filename<<filepathRed<<(count)<<"_CIRC";
				filename_ptr = filename.str();
				filename<<fileFormat;
				imwrite(filename.str(),frame);
				if(verbose)
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

