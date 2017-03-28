#include <iostream>

#include "Flags.hpp"

using namespace std;

#define MAX_BUFFER 1024

int setupNamedPipe();

int main(void) 
{	
	int pipeDescriptor = setupNamedPipe(O_RDONLY);
	char buffer[MAX_BUFFER];
	
	
	for(int i=0;i<5;i++)
	{
		int err = read(pipeDescriptor, buffer, MAX_BUFFER);
		if(err == -1)
		{
			char * message = strerror_r(errno, buffer, MAX_BUFFER);
			cout << "(Read ./Flags.a) " << errno << " : " << message << endl;
			return -1;
		}
		else
		{
			cout << "Received " << buffer << endl;
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
