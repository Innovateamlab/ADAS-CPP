#include <iostream>
#include <wiringPi.h>
#include "Flags.hpp"

using namespace std;

#define MAX_BUFFER 1024
void gpioSetup();
int main(void) 
{	
	gpioSetup();
	int pipeDescriptor = setupNamedPipe(O_RDONLY);
	char buffer[MAX_BUFFER];
	
	Data data;
	
	while(1)
	{
		int err = read(pipeDescriptor, &data, sizeof(Data));
		if(err == -1)
		{
			char * message = strerror_r(errno, buffer, MAX_BUFFER);
			cout << "(Read ./Flags.a) " << errno << " : " << message << endl;
			return -1;
		}
		else
		{
			if (data.flag == LIGHT_BLUE){
				digitalWrite (LIGHT_BLUE, HIGH) ;
				delay(500);
				digitalWrite (LIGHT_BLUE,  LOW) ; 
				delay(500);
			}
			else if (data.flag == LIGHT_RED){
				digitalWrite (LIGHT_RED, HIGH) ;
				delay(500);
				digitalWrite (LIGHT_RED,  LOW) ; 
				delay(500);
			}
			cout << "Received (" << data.flag << ") : "<< data.message << endl;
		}
	}
	
	
	/*
	while(1)
	{
		read(pipeDescriptor, buffer, MAX_BUFFER);
		cout << "Received " << buffer << endl;
	}*/
	
	/*int color_flag = buffer[0];
	int fail_flag = buffer[4];
	
	wiringPiSetup () ;
	pinMode (LIGHT_BLUE, OUTPUT);
	pinMode (LIGHT_RED, OUTPUT);
	pinMode (FAIL, OUTPUT);
	
	if (color_flag == LIGHT_BLUE)
	{
		digitalWrite (LIGHT_BLUE, HIGH) ; delay (200) ;
		digitalWrite (LIGHT_BLUE,  LOW) ; delay (500) ;
	}
	if (color_flag == LIGHT_RED)
	{
		digitalWrite (LIGHT_RED, HIGH) ; delay (200) ;
		digitalWrite (LIGHT_RED,  LOW) ; delay (500) ;
	}
	if (fail_flag == FAIL) // 3 time green blink when error 
	{
		while(1){
			digitalWrite (FAIL, HIGH) ; delay (200) ;
			digitalWrite (FAIL,  LOW) ; delay (500) ;
			digitalWrite (FAIL, HIGH) ; delay (200) ;
			digitalWrite (FAIL,  LOW) ; delay (500) ;
			digitalWrite (FAIL, HIGH) ; delay (200) ;
			digitalWrite (FAIL,  LOW) ; delay (500) ;
		}
	}*/
	
	unlink(PIPE_FILENAME);
}

void gpioSetup() //GPIO pins initialisation
{
	wiringPiSetup();	
	pinMode(LIGHT_BLUE, OUTPUT);
	pinMode(LIGHT_RED, OUTPUT);
	pinMode(FAIL, OUTPUT);
}

